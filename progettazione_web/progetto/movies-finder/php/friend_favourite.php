<?php
session_start();


require __DIR__ . "/utils/response.php";
require __DIR__ . "/models/favourites.php";


if ($_SERVER['REQUEST_METHOD'] != "GET") {
    (new Response(400, "Metodo non accettato"))->send();
    exit();
}



$him = $_GET["id"] ?? null;
if ($him === null) {
    (new Response(400, "Username amico non valido"))->send();
    exit();
}


$favourites = Favourites::get_favourites($him);
if ($favourites === false) {
    (new Response(500, "Errore durante il recupero dei preferiti"))->send();
    exit();
}
(new Response(200, "Lista dei preferiti", ["favourites" => $favourites]))->send();
exit();
