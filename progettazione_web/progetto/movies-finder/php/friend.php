<?php
session_start();
// ini_set("display_errors", 1);
// ini_set("display_startup_errors", 1);
// error_reporting(E_ALL);

require_once __DIR__ . "/models/friend_requests.php";
require_once __DIR__ . "/utils/response.php";


if ($_SERVER["REQUEST_METHOD"] !== "POST") {
    (new Response(405, "Metodo non consentito"))->send();
    exit();
}

$input = json_decode(file_get_contents("php://input"), true);

$me = $_SESSION["username"];
$him = $input["receiver"] ?? null;
$action = $input["action"] ?? null;

if (!$action) {
    (new Response(400, "Paramentri mancanti"))->send();
    exit();
}

if (
    !$him &&
    $action != "get_requests" &&
    $action != "get_friends" &&
    $action != "pending"
) {
    (new Response(400, "Parametri non validi"))->send();
    exit();
}

switch ($action) {
    case "get_friends":
        $friend = FriendRequests::get_friends($me);
        (new Response(200, "Lista degli amici", ["friends" => $friend]))->send();
        break;
    case "get_requests":
        $requests = FriendRequests::get_requests($me);
        (new Response(200, "Lista delle richeste di amicizia", [
            "requests" => $requests,
        ]))->send();
        break;
    case "add":
        $added = FriendRequests::accept_request($him, $me);
        if (!$added) {
            (new Response(500, "Qualcosa e' andato storto"))->send();
            exit();
        }
        (new Response(200, "Amicizia accettata"))->send();
        break;
    case "delete":
        $deleted = FriendRequests::reject_request($him, $me);
        if (!$deleted) {
            (new Response(500, "Qualcosa e' andato storto"))->send();
            exit();
        }
        (new Response(200, "Amicizia rifiutata"))->send();
        break;
    case "send":
        $is_friend = FriendRequests::is_friend($me, $him);
        if ($is_friend) {
            (new Response(200, "Richesta gia inviata"))->send();
            exit();
        }
        $sent = FriendRequests::send_request($me, $him);
        if (!$sent) {
            (new Response(500, "Qualcosa e' andato storto"))->send();
            exit();
        }
        (new Response(200, "Richesta inviata"))->send();
        break;
    case "pending":
        $pendings = FriendRequests::get_pending($me);
        (new Response(200, "Lista deglle richeste pendenti", [
            "pendings" => $pendings,
        ]))->send();
        break;
}
