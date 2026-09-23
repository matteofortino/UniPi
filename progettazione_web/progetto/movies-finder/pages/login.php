<!doctype html>
<html lang="en">

<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Log In</title>
    <link rel="stylesheet" href="../css/login.css" />
</head>

<body>
    <form class="form" method="post">
        <h1>Log In</h1>
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
        <button type="submit">Log In</button>
        <p>
            Do not have an account?
            <a href="../pages/signin.php">Sign In Here</a>
        </p>
    </form>
    <script src="../js/login.js"></script>
</body>

</html>