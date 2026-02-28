<?php
session_start();
require_once __DIR__ . '/models/users.php';
require_once __DIR__ . '/utils/response.php';


// Legge i dati dal body JSON
$input = json_decode(file_get_contents("php://input"), true);
$username = $input['username'] ?? null;
$password = $input['password'] ?? null;

// Controlla parametri
if (!$username || !$password) {
  (new Response(400, "Parametri mancanti"))->send();
  exit();
}

// Controlla se l’utente esiste già
$user = Users::get_user($username);
if ($user !== false) {
  (new Response(409, "Username già registrato"))->send();
  exit();
}

// Hash della password
$hashedPassword = password_hash($password, PASSWORD_BCRYPT);

// Inserisce il nuovo utente
$newUser = Users::insert_user($username, $hashedPassword);

if (!$newUser) {
  (new Response(500, "Errore durante la registrazione"))->send();
  exit();
}

// ✅ Registrazione ok
(new Response(201, "Registrazione completata con successo", ["username" => $username]))->send();
exit();
