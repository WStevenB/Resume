<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Blacklist New Account</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/newaccount-grid.css">
  <link rel="stylesheet" href="css/newaccount.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-newaccount clearfix">
<img class="image image-1" src="images/par(2).png">
<form action="NewAccountController" method="post">
  <div class="container container-2 clearfix">
    <p class="text text-2">Email:</p>
    <input class="_input _input-2" type="text" name="email" value = ${requestScope['email']}>
  </div>
  
  <div class="container container-4 clearfix">
    <p class="text text-4">Screen Name:</p>
    <input class="_input _input-4" type="text" name="username" value = ${requestScope['username']}>
  </div>
  
  <div class="container container-3 clearfix">
    <p class="text text-3">Password:</p>
    <input class="_input _input-3" type="password" name="password">
  </div>
  
  <div class="container container-1 clearfix">
    <p class="text text-1">Confirm Password:</p>
    <input class="_input _input-1" type="password" name="confirmPassword">
  </div>
  
  ${requestScope['AccountButton']}
</form>

  <div class="element"></div>
<form action="NewAccountController" method="post">
  <button class="_button _button-2" type="submit" name="actionCode" value="2">${requestScope['Action']}</button>
</form>
  <img class="image image-2" src="images/blacklist2.png">
  <p class="text text-5">${requestScope['Error']}</p>
</body>
</html>
