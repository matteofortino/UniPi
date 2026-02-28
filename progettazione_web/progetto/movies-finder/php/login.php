<?php
session_start();
require_once __DIR__ . '/models/users.php';
require_once __DIR__ . '/utils/response.php';


// Legge i dati in formato JSON dal body della richiesta
$input = json_decode(file_get_contents("php://input"), true);
$username = $input['username'] ?? null;
$password = $input['password'] ?? null;

// Controlla parametri
if (!$username || !$password) {
	(new Response(400, "Parametri mancanti"))->send();
	exit();
}

// Recupera l'utente dal modello
$user = Users::get_user($username);
if ($user === false) {
	(new Response(401, "Username o password errati"))->send();
	exit();
}

// Verifica la password con password_verify()
if (!password_verify($password, $user['password'])) {
	(new Response(401, "Username o password errati"))->send();
	exit();
}


$_SESSION["username"] = $username;

(new Response(200, "Login effettuato con successo", ["username" => $username]))->send();
exit();
