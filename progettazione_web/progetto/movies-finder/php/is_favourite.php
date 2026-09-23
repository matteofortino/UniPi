<?php
session_start();
// ini_set("display_errors", 1);
// ini_set("display_startup_errors", 1);
// error_reporting(E_ALL);
require_once __DIR__ . "/models/favourites.php";
require_once __DIR__ . "/utils/response.php";

if (!isset($_SESSION['username'])) {
    (new Response(500, "Utente non autenticato"))->send();
    exit();
}

if ($_SERVER["REQUEST_METHOD"] === "GET") {

    $id = $_GET["id"] ?? null;
    if ($id === null) {
        (new Response(400, "ID mancante"))->send();
        exit();
    }

    $is_favourite = Favourites::is_favourite($_SESSION["username"], $id);
    if ($is_favourite === false) {
        (new Response(500, "Film non trovato tra i preferiti", [
            "isFavourite" => $is_favourite,
        ]))->send();
        exit();
    }
    (new Response(200, "Film trovato tra i preferiti", [
        "isFavourite" => $is_favourite,
    ]))->send();
    exit();
}

$input = json_decode(file_get_contents("php://input"), true);
if (!is_array($input)) {
    (new Response(400, "JSON non valido"))->send();
    exit();
}

$movie_id = $input["movieId"] ?? null;
$to_add = $input["toAdd"];

if ($movie_id === null || $to_add === null) {
    (new Response(400, "Parametri mancanti"))->send();
    exit();
}

if ($to_add) {
    $favourite = Favourites::insert_favourite($_SESSION["username"], $movie_id);

    if ($favourite === false) {
        (new Response(
            500,
            "Errore durante l'inserimento tra i preferiti",
        ))->send();
        exit();
    }
    (new Response(200, "Film aggiunto ai preferiti"))->send();
    exit();
}

if (!$to_add) {
    $favourite = Favourites::delete_favourite($_SESSION["username"], $movie_id);
    if ($favourite === false) {
        (new Response(500, "Errore durante la rimozione dai preferiti"))->send();
        exit();
    }
    (new Response(200, "Film rimosso dai preferiti"))->send();
    exit();
}
