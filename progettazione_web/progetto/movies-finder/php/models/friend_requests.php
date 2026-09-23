<?php
require_once __DIR__ . "/../utils/database.php";

class FriendRequests extends Database
{
  public static function send_request($sender, $receiver)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    return $db->query("INSERT INTO FriendRequests (sender, receiver) VALUES (?, ?)", 'ss', $sender, $receiver);
  }

  public static function reject_request($sender, $receiver)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    return $db->query("DELETE FROM FriendRequests WHERE (sender = ? AND receiver = ?)", "ss", $sender, $receiver);
  }
  public static function accept_request($sender, $receiver)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    return $db->query("UPDATE FriendRequests SET status = 'accepted' WHERE sender = ? AND receiver = ?", "ss", $sender, $receiver);
  }

  public static function get_requests($receiver)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }

    $result = $db->query("SELECT * FROM FriendRequests WHERE receiver = ? and status = 'pending'", "s", $receiver);

    if (!$result) return false;
    $rows = [];
    while ($row = $result->fetch_assoc()) {
      $rows[] = $row;
    }
    return $rows;
  }

  public static function get_friends($username)
  {

    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    $result = $db->query("SELECT * FROM FriendRequests WHERE (sender = ? OR receiver = ?) AND status = 'accepted'", "ss", $username, $username);

    if (!$result) return false;
    $rows = [];
    while ($row = $result->fetch_assoc()) {
      $rows[] = $row;
    }
    return $rows;
  }

  public static function get_pending($sender)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    $result = $db->query("SELECT * FROM FriendRequests WHERE sender = ? AND status = 'pending'", "s", $sender);
    if (!$result) return false;
    $rows = [];
    while ($row = $result->fetch_assoc()) {
      $rows[] = $row;
    }
    return $rows;
  }

  public static function is_friend($sender, $receiver)
  {
    $db = new self();
    if (!$db->connect('localhost', 'root', '', 'fortino_676770')) {
      return false;
    }
    $result = $db->query("SELECT * FROM FriendRequests WHERE (sender = ? AND receiver = ?) OR (sender = ? AND receiver = ?)", "ssss", $sender, $receiver, $receiver, $sender);
    if ($db->get_rows_number($result) <= 0)
      return false;
    else
      return true;
  }
}
