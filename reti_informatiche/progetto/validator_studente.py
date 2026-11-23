#!/usr/bin/env python3
"""
Uso:
    python3 validator_studente.py [PROJECT_DIR]
"""

from __future__ import annotations

import argparse
import contextlib
import os
from pathlib import Path
import socket
import subprocess
import tempfile
import time
from dataclasses import dataclass
from typing import Callable, Optional

HOST = "127.0.0.1"
STARTUP_TIMEOUT = 3.0
CLIENT_TIMEOUT = 8.0
SOCKET_TIMEOUT = 3.0
MAX_LINE = 512


class ValidationError(RuntimeError):
    pass


class _StartupFailed(Exception):
    pass


PUBLIC_TESTS = (
    "STORAGE REGISTRATION / PUT ROUTING",
    "END-TO-END PUT/GET + FILE INTEGRITY",
    "FILE SIZES 0/1/4095/4096/4097",
    "FRAGMENTED TCP CONTROL + PAYLOAD",
    "LIST",
    "NOT_FOUND ERROR",
)


class BufferedSocket:
    def __init__(self, sock: socket.socket):
        self.sock = sock
        self.buf = bytearray()

    def send_line(self, line: str) -> None:
        self.sock.sendall(line.encode("ascii") + b"\n")

    def recv_line(self) -> str:
        while True:
            pos = self.buf.find(b"\n")
            if pos >= 0:
                raw = bytes(self.buf[:pos])
                del self.buf[: pos + 1]
                try:
                    return raw.decode("ascii")
                except UnicodeDecodeError as exc:
                    raise ValidationError("received a non-ASCII control line") from exc
            if len(self.buf) > MAX_LINE:
                raise ValidationError("received a control line longer than 512 bytes")
            data = self.sock.recv(4096)
            if not data:
                raise ValidationError("connection closed while waiting for a control line")
            self.buf.extend(data)

    def recv_exact(self, count: int) -> bytes:
        if count < 0:
            raise ValidationError("negative byte count")
        out = bytearray()
        if self.buf:
            take = min(count, len(self.buf))
            out += self.buf[:take]
            del self.buf[:take]
        while len(out) < count:
            data = self.sock.recv(min(4096, count - len(out)))
            if not data:
                raise ValidationError(
                    f"connection closed after {len(out)}/{count} payload bytes"
                )
            out.extend(data)
        return bytes(out)


@dataclass
class TestResult:
    name: str
    ok: bool
    detail: str = ""


class ManagedProcess:
    def __init__(self, argv: list[str], cwd: Path, log_path: Path):
        self.log_file = open(log_path, "wb")
        self.proc = subprocess.Popen(
            argv,
            cwd=cwd,
            stdin=subprocess.DEVNULL,
            stdout=self.log_file,
            stderr=subprocess.STDOUT,
            start_new_session=True,
        )

    def stop(self) -> None:
        if self.proc.poll() is None:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=1.5)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=1.5)
        self.log_file.close()


@contextlib.contextmanager
def tcp_connection(port: int):
    sock = socket.create_connection((HOST, port), timeout=SOCKET_TIMEOUT)
    sock.settimeout(SOCKET_TIMEOUT)
    try:
        yield BufferedSocket(sock)
    finally:
        sock.close()


def free_ports(count: int) -> list[int]:
    """Reserve `count` distinct free ports (sockets kept open until all are chosen)."""
    socks = []
    try:
        for _ in range(count):
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.bind((HOST, 0))
            socks.append(s)
        return [int(s.getsockname()[1]) for s in socks]
    finally:
        for s in socks:
            s.close()


def log_tail(path: Path, limit: int = 800) -> str:
    try:
        text = path.read_bytes()[-limit:].decode("utf-8", errors="replace").strip()
    except OSError:
        return ""
    return f"\nlast output of the process:\n{text}" if text else ""


def wait_for_port(port: int, proc: ManagedProcess, timeout: float = STARTUP_TIMEOUT) -> None:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if proc.proc.poll() is not None:
            raise ValidationError(f"process exited early with code {proc.proc.returncode}")
        try:
            with socket.create_connection((HOST, port), timeout=0.15):
                return
        except OSError:
            time.sleep(0.05)
    raise ValidationError(f"port {port} did not become reachable")


def make_payload(size: int) -> bytes:
    pattern = (
        b"Computer Networks - MiniStore validation file.\n"
        b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n"
    )
    if size == 0:
        return b""
    return (pattern * ((size // len(pattern)) + 1))[:size]


def parse_store(line: str) -> tuple[int, str, int]:
    parts = line.split()
    if len(parts) != 4 or parts[0] != "STORE":
        raise ValidationError(f"expected 'STORE <id> <ip> <port>', got: {line!r}")
    sid = int(parts[1])
    ip = parts[2]
    port = int(parts[3])
    return sid, ip, port


def parse_found(line: str) -> tuple[int, str, int, int]:
    parts = line.split()
    if len(parts) != 5 or parts[0] != "FOUND":
        raise ValidationError(
            f"expected 'FOUND <id> <ip> <port> <size>', got: {line!r}"
        )
    return int(parts[1]), parts[2], int(parts[3]), int(parts[4])


def request_store(coord_port: int, name: str, size: int, fragmented: bool = False):
    sock = socket.create_connection((HOST, coord_port), timeout=SOCKET_TIMEOUT)
    sock.settimeout(SOCKET_TIMEOUT)
    bs = BufferedSocket(sock)
    message = f"PUT {name} {size}\n".encode("ascii")
    if fragmented:
        cuts = [1, 2, 5, 9, 13, len(message)]
        start = 0
        for end in cuts:
            end = min(end, len(message))
            if end > start:
                sock.sendall(message[start:end])
                time.sleep(0.01)
                start = end
        if start < len(message):
            sock.sendall(message[start:])
    else:
        sock.sendall(message)
    line = bs.recv_line()
    sock.close()
    return parse_store(line)


def upload_raw(storage_port: int, name: str, payload: bytes, fragmented: bool = False) -> None:
    sock = socket.create_connection((HOST, storage_port), timeout=SOCKET_TIMEOUT)
    sock.settimeout(SOCKET_TIMEOUT)
    bs = BufferedSocket(sock)
    header = f"UPLOAD {name} {len(payload)}\n".encode("ascii")
    if fragmented:
        for byte in header:
            sock.sendall(bytes([byte]))
        line = bs.recv_line()
        if line != "READY":
            raise ValidationError(f"expected READY, got {line!r}")
        step_pattern = [1, 7, 31, 113, 509]
        pos = 0
        i = 0
        while pos < len(payload):
            step = step_pattern[i % len(step_pattern)]
            sock.sendall(payload[pos : pos + step])
            pos += step
            i += 1
    else:
        sock.sendall(header)
        line = bs.recv_line()
        if line != "READY":
            raise ValidationError(f"expected READY, got {line!r}")
        sock.sendall(payload)
    line = bs.recv_line()
    expected = f"STORED {name} {len(payload)}"
    if line != expected:
        raise ValidationError(f"expected {expected!r}, got {line!r}")
    sock.close()


def download_raw(coord_port: int, name: str) -> bytes:
    with tcp_connection(coord_port) as bs:
        bs.send_line(f"GET {name}")
        line = bs.recv_line()
    _sid, ip, port, size = parse_found(line)
    if ip != HOST:
        raise ValidationError(f"expected storage IP {HOST}, got {ip}")
    sock = socket.create_connection((HOST, port), timeout=SOCKET_TIMEOUT)
    sock.settimeout(SOCKET_TIMEOUT)
    bs = BufferedSocket(sock)
    bs.send_line(f"DOWNLOAD {name}")
    line = bs.recv_line()
    parts = line.split()
    if len(parts) != 2 or parts[0] != "DATA":
        raise ValidationError(f"expected 'DATA <size>', got {line!r}")
    announced = int(parts[1])
    if announced != size:
        raise ValidationError(
            f"coordinator announced {size} bytes but storage announced {announced}"
        )
    data = bs.recv_exact(size)
    sock.close()
    return data


def list_raw(coord_port: int) -> list[tuple[str, int, int]]:
    with tcp_connection(coord_port) as bs:
        bs.send_line("LIST")
        first = bs.recv_line()
        parts = first.split()
        if len(parts) != 2 or parts[0] != "FILES":
            raise ValidationError(f"expected 'FILES <n>', got {first!r}")
        count = int(parts[1])
        rows = []
        for _ in range(count):
            line = bs.recv_line()
            p = line.split()
            if len(p) != 4 or p[0] != "FILE":
                raise ValidationError(f"invalid FILE line: {line!r}")
            rows.append((p[1], int(p[2]), int(p[3])))
        end = bs.recv_line()
        if end != "END":
            raise ValidationError(f"expected END, got {end!r}")
        return rows


def run_client(project: Path, args: list[str]) -> subprocess.CompletedProcess:
    return subprocess.run(
        [str(project / "client"), *args],
        cwd=project,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        timeout=CLIENT_TIMEOUT,
        text=True,
    )


def execute_test(name: str, fn: Callable[[], None]) -> TestResult:
    try:
        fn()
        return TestResult(name, True, "")
    except Exception as exc:
        return TestResult(name, False, str(exc))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project_dir", nargs="?", default=".")
    args = parser.parse_args()
    project = Path(args.project_dir).resolve()
    report_path = project / "validation_report.txt"
    results: list[TestResult] = []

    def build_test() -> None:
        if not (project / "Makefile").exists():
            raise ValidationError("Makefile not found")
        subprocess.run(["make", "clean"], cwd=project, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=10)
        cp = subprocess.run(
            ["make"], cwd=project, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=20, text=True
        )
        if cp.returncode != 0:
            raise ValidationError("make failed:\n" + cp.stdout[-2000:])
        missing = [name for name in ("coordinator", "storage", "client") if not (project / name).exists()]
        if missing:
            raise ValidationError("missing executable(s): " + ", ".join(missing))

    results.append(execute_test("BUILD", build_test))
    if not results[-1].ok:
        _write_report(report_path, results)
        print(report_path.read_text())
        return 1

    with tempfile.TemporaryDirectory(prefix="ministore_public_") as td:
        temp = Path(td)
        logs = temp / "logs"
        logs.mkdir()
        storage_dir = temp / "storage1"
        storage_dir.mkdir()
        coord_port, storage_port = free_ports(2)
        coordinator: Optional[ManagedProcess] = None
        storage: Optional[ManagedProcess] = None
        try:
            stage = "coordinator"
            try:
                coordinator = ManagedProcess(
                    [str(project / "coordinator"), str(coord_port)], project, logs / "coordinator.log"
                )
                wait_for_port(coord_port, coordinator)
                stage = "storage"
                storage = ManagedProcess(
                    [
                        str(project / "storage"),
                        str(coord_port),
                        str(storage_port),
                        str(storage_dir),
                    ],
                    project,
                    logs / "storage.log",
                )
                wait_for_port(storage_port, storage)
            except Exception as exc:
                detail = f"not executed: {stage} did not start ({exc})" + log_tail(logs / f"{stage}.log")
                for name in PUBLIC_TESTS:
                    results.append(TestResult(name, False, detail))
                raise _StartupFailed() from exc
            time.sleep(0.15)

            def registration_test() -> None:
                deadline = time.monotonic() + STARTUP_TIMEOUT
                while True:
                    try:
                        _sid, ip, port = request_store(coord_port, "probe.txt", 1)
                        break
                    except ValidationError as exc:
                        if "ERR NO_STORAGE" not in str(exc) or time.monotonic() > deadline:
                            raise
                        time.sleep(0.05)
                if ip != HOST or port != storage_port:
                    raise ValidationError(f"coordinator returned {ip}:{port}, expected {HOST}:{storage_port}")

            results.append(execute_test("STORAGE REGISTRATION / PUT ROUTING", registration_test))

            def e2e_test() -> None:
                src = temp / "source.txt"
                dst = temp / "downloaded.txt"
                payload = make_payload(17321)
                src.write_bytes(payload)
                remote = "e2e.txt"
                cp = run_client(project, [str(coord_port), "put", str(src), remote])
                if cp.returncode != 0:
                    raise ValidationError("client put failed: " + cp.stdout[-1000:])
                cp = run_client(project, [str(coord_port), "get", remote, str(dst)])
                if cp.returncode != 0:
                    raise ValidationError("client get failed: " + cp.stdout[-1000:])
                if not dst.exists():
                    raise ValidationError("download destination was not created")
                if dst.read_bytes() != payload:
                    raise ValidationError("downloaded file differs byte-for-byte from the source")

            results.append(execute_test("END-TO-END PUT/GET + FILE INTEGRITY", e2e_test))

            def boundary_test() -> None:
                for size in (0, 1, 4095, 4096, 4097):
                    remote = f"b{size}.txt"
                    payload = make_payload(size)
                    _sid, _ip, port = request_store(coord_port, remote, size)
                    upload_raw(port, remote, payload)
                    got = download_raw(coord_port, remote)
                    if got != payload:
                        raise ValidationError(f"content mismatch for {size}-byte file")

            results.append(execute_test("FILE SIZES 0/1/4095/4096/4097", boundary_test))

            def fragmented_test() -> None:
                remote = "fragmented.txt"
                payload = make_payload(6003)
                _sid, _ip, port = request_store(coord_port, remote, len(payload), fragmented=True)
                upload_raw(port, remote, payload, fragmented=True)
                got = download_raw(coord_port, remote)
                if got != payload:
                    raise ValidationError("fragmented transfer produced different content")

            results.append(execute_test("FRAGMENTED TCP CONTROL + PAYLOAD", fragmented_test))

            def list_test() -> None:
                rows = list_raw(coord_port)
                names = [r[0] for r in rows]
                if names != sorted(names):
                    raise ValidationError("LIST is not sorted lexicographically by remote name")
                for required in ("e2e.txt", "fragmented.txt"):
                    if required not in names:
                        raise ValidationError(f"LIST does not contain {required}")

            results.append(execute_test("LIST", list_test))

            def not_found_test() -> None:
                with tcp_connection(coord_port) as bs:
                    bs.send_line("GET definitely_missing.txt")
                    line = bs.recv_line()
                if line != "ERR NOT_FOUND":
                    raise ValidationError(f"expected 'ERR NOT_FOUND', got {line!r}")

            results.append(execute_test("NOT_FOUND ERROR", not_found_test))

        except _StartupFailed:
            pass
        finally:
            if storage is not None:
                storage.stop()
            if coordinator is not None:
                coordinator.stop()

    _write_report(report_path, results)
    print(report_path.read_text())
    return 0 if all(r.ok for r in results) else 1


def _write_report(path: Path, results: list[TestResult]) -> None:
    lines = ["MiniStore - public validation report", "=" * 40, ""]
    passed = 0
    for result in results:
        status = "PASS" if result.ok else "FAIL"
        lines.append(f"[{status:4}] {result.name}")
        if result.detail:
            for detail_line in result.detail.splitlines():
                lines.append(f"       {detail_line}")
        if result.ok:
            passed += 1
    lines += ["", f"Result: {passed}/{len(results)} public tests passed.", ""]
    lines.append(
        "Passing these public tests is not a proof of complete correctness and does not determine the project grade."
    )
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    raise SystemExit(main())
