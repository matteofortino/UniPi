<?php
session_start();
if (!isset($_SESSION["username"])) {
  header("Location: pages/login.php");
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Home Page</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <nav id="navbar">
    <a href="index.php">Home</a>
    <a href="pages/favourite.php">Favourites</a>
    <a href="pages/friends.php" id="friends">Friends</a>
    <a href="guida.html" id="guida">Guida</a>
    <a href="logout.php" id="logout">Logout</a>
  </nav>
  <form id="form">
    <h1>Find your favourite movie</h1>
    <label for="search">Search Movie:</label>
    <input type="search" name="search" id="search">
    <button type="button" id="button">search</button>
  </form>
  <div class="movies-container"></div>
  <script type="module" src="./script.js"></script>
</body>

</html>