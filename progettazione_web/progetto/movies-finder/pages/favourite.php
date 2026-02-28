<?php
session_start();
if (!isset($_SESSION["username"])) {
  header("Location: ./login.php");
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <link rel="stylesheet" href="../css/favourites.css" />
  <title>Favourites</title>
</head>

<body>
  <nav id="navbar">
    <a href="/movies-finder/index.php">Home</a>
    <a href="favourite.php">Favourites</a>
    <a href="friends.php" id="friends">Friends</a>
    <a href="../logout.php" id="logout">Logout</a>
  </nav>
  <h1>Your Favourite Movies</h1>
  <div id="favourites-container"></div>

  <script type="module" src="../js/favourite.js"></script>
</body>

</html>