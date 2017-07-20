<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Blacklist</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/index-grid.css">
  <link rel="stylesheet" href="css/index.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-index clearfix">
  <img class="image image-1" src="images/par(2).png">
  <form action="LoginController" method="post">
  <div class="container container-2 clearfix">
    <p class="text text-2">Email Address:</p>
    <input class="_input _input-2" type="text" name="email" value="${requestScope['email']}">
  </div>
  <div class="container container-1 clearfix">
    <p class="text text-1">Password:</p>
    <input class="_input _input-1" type="password" name="password">
  </div>
  <button class="_button _button-1" type="submit" name="actionCode" value="1">Sign In</button>
  </form>
  <div class="element"></div>
  <form action="LoginController" method="post">
  <button class="_button _button-2" type="submit" name="actionCode" value="2">New Account</button>
  <button class="_button _button-3" type="submit" name="actionCode" value="3">Forgot Password</button>
  </form>
  <img class="image image-2" src="images/blacklist2.png">
  <p class="text text-3">${requestScope['Error']}</p>

</body>
</html>
