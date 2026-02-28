<?php

session_start();
require_once __DIR__ . "/models/favourites.php";
require_once __DIR__ . "/utils/response.php";


if ($_SERVER["REQUEST_METHOD"] !== "GET") {

    (new Response(405, "Metodo non consentito"))->send();
    exit();
}

$favourites = Favourites::get_favourites($_SESSION["username"]);
if ($favourites === false) {
    (new Response(500, "Errore durante il recupero dei preferiti"))->send();
    exit();
}
(new Response(200, "Lista dei preferiti", ["favourites" => $favourites]))->send();
exit();
