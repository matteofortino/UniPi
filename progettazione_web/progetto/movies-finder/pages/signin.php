<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Sign In</title>
  <link rel="stylesheet" href="../css/signin.css" />
</head>

<body>
  <form class="form" method="post">
    <h1>Sign In</h1>
    <label for="username">Username</label>
    <input
      type="text"
      id="username"
      placeholder="Username"
      name="username"
      required />
    <label for="password">Password</label>
    <input
      type="password"
      id="password"
      placeholder="Password"
      name="password"
      required />
    <p class="text"></p>
    <button type="submit">Sign In</button>
    <p>
      Already have an account? <a href="../pages/login.php">Log In Here</a>
    </p>
  </form>
  <script src="../js/signin.js"></script>
</body>

</html>