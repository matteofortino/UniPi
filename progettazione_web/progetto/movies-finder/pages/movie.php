<?php
session_start();
if (!isset($_SESSION["username"])) {
  header("Location: ./login.html");
}
?>
<!DOCTYPE html>
<html lang="it">

<head>
  <meta charset="UTF-8" />
  <meta http-equiv="X-UA-Compatible" content="IE=edge" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <link rel="stylesheet" href="../css/movie.css" />
  <title>Movie</title>
</head>

<body>
  <nav id="navbar">
    <a href="/movies-finder/index.php">Home</a>
    <a href="favourite.php">Favourites</a>
    <a href="friends.php" id="friends">Friends</a>
    <a href="../logout.php" id="logout">Logout</a>
  </nav>

  <div class="movie-container">
    <img src="#" id="img" alt="Movie Poster" />

    <label class="favorite-star">
      <input type="checkbox" id="favourite-checkbox" />
      <span class="star">★</span>
    </label>

    <div class="movie-details">
      <h1 id="title">film title</h1>
      <span id="release-date"></span>
      <div id="genres"></div>
      <div id="duration"></div>
      <div id="overview"></div>
      <div id="credits"></div>
    </div>
  </div>
  <p id="line"></p>
  <div id="collection">
    <h2>COLLECTION</h2>
    <div id="collection-container"></div>
  </div>

  <script type="module" src="../js/isFavourite.js"></script>
  <script type="module" src="../js/movie.js"></script>
</body>

</html>