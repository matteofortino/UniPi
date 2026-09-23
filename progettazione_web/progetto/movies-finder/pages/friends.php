<?php
session_start();
if (!isset($_SESSION["username"])) {
  header("Location: ./login.php");
}
?>
<!doctype html>
<html lang="en">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Document</title>
  <link rel="stylesheet" href="../css/friends.css" />
</head>

<body>
  <nav id="navbar">
    <a href="/movies-finder/index.php">Home</a>
    <a href="favourite.php">Favourites</a>
    <a href="friends.php" id="friends">Friends</a>
    <a href="../logout.php" id="logout">Logout</a>
  </nav>
  <div id="send-request">
    <input type="text" id="user-search" placeholder="Search users..." />
    <div id="dropdown"></div>
    <button type="button" id="send-button">Invia</button>
  </div>
  <h1>Sent Request</h1>
  <div id="sent-requests"></div>
  <h1>Pending Requests</h1>
  <div id="pending-requests"></div>
  <h1>Your Friends</h1>
  <div id="friends-container"></div>

  <script type="module" src="../js/friends.js"></script>
</body>

</html>