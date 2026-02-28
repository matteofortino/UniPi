<?php
// 
session_start();
require_once __DIR__ . '/models/users.php';
require_once __DIR__ . '/utils/response.php';


if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
  (new Response(405, "Metodo non consentito"))->send();
  exit();
}

$users = Users::get_all();
(new Response(200, "Lista degli utenti", ["users" => $users]))->send();
exit();
