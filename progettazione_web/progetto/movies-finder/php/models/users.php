<?php
require_once __DIR__ . '/../utils/database.php';

class Users extends Database
{

  // 🔹 Return the entire Users table
  public static function get_all()
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    $result = $db->query("SELECT * FROM Users");
    if (!$result) return false;

    $rows = [];
    while ($row = $result->fetch_assoc()) {
      $rows[] = $row;
    }

    return $rows;
  }

  // 🔹 Return a single user by username
  public static function get_user($username)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    $result = $db->query("SELECT * FROM Users WHERE username = ?", 's', $username);
    if (!$result || $db->get_rows_number($result) === 0) {
      return false;
    }

    return $result->fetch_assoc();
  }
  public static function insert_user($username, $hashedPassword)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    return $db->query("INSERT INTO Users (username, password) VALUES (?, ?)", 'ss', $username, $hashedPassword);
  }
}
