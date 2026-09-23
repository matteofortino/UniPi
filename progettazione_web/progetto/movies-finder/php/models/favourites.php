
<?php
require_once __DIR__ . '/../utils/database.php';

class Favourites extends Database
{
  public static function insert_favourite($username, $movie_id)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    return $db->query("INSERT INTO Favourites (username, movie_id) VALUES (?, ?)", 'ss', $username, (string)$movie_id);
  }

  public static function delete_favourite($username, $movie_id)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    return $db->query("DELETE FROM Favourites WHERE username = ? AND movie_id = ?", 'ss', $username, (string)$movie_id);
  }

  public static function is_favourite($username, $movie_id)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    $result = $db->query("SELECT * FROM Favourites WHERE username = ? AND movie_id = ?", 'ss', $username, $movie_id);
    if (!$result || $db->get_rows_number($result) === 0) {
      return false;
    }
    return true;
  }
  /*
   * @return array|bool Ritorna un array di preferiti o false se fallisce.
   */
  public static function get_favourites($username)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    $result = $db->query("SELECT * FROM Favourites WHERE username = ?", 's', $username);
    if (!$result) return false;
    $rows = [];
    while ($row = $result->fetch_assoc()) {
      $rows[] = $row;
    }
    return $rows;
  }
}
