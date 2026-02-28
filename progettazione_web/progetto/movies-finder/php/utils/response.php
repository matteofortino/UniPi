<?php
class Response
{
  private int $statusCode;
  private string $message;
  private mixed $data;

  public function __construct(
    int $statusCode = 200,
    string $message = "",
    mixed $data = null
  ) {
    $this->statusCode = $statusCode;
    $this->message = $message;
    $this->data = $data;
  }

  /** Imposta il codice HTTP */
  public function setStatusCode(int $code): void
  {
    $this->statusCode = $code;
  }

  /** Imposta il messaggio */
  public function setMessage(string $message): void
  {
    $this->message = $message;
  }

  /** Imposta i dati */
  public function setData(mixed $data): void
  {
    $this->data = $data;
  }

  /** Restituisce la risposta come array */
  public function toArray(): array
  {
    return [
      "message" => $this->message,
      "data"    => $this->data,
    ];
  }

  /** Invia la risposta JSON al client */
  public function send(): void
  {
    http_response_code($this->statusCode);
    header('Content-Type: application/json; charset=utf-8');
    echo json_encode($this->toArray(), JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
    exit;
  }
}
