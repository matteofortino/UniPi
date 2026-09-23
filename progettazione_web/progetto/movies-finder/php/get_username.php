<?php
session_start();
require __DIR__ . "/utils/response.php";

$username = $_SESSION["username"] ?? null;

if ($_SERVER["REQUEST_METHOD"] != "GET") {
    (new Response(500, "Metodo non consentito"))->send();
    exit();
}

if ($username === null) {
    (new Response(400, "Username non valido"))->send();
    exit();
}


(new Response(200, "Username valido", ["username" => $username]))->send();
exit();
