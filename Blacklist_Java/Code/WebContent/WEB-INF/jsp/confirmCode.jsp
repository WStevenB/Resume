<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Confirm Account</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/confirmationcode-grid.css">
  <link rel="stylesheet" href="css/confirmationcode.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-confirmationcode clearfix">
  <p class="_button _button-1">Enter confirmation code sent to your email</p>
  <img class="image image-1" src="images/par(2).png">
<form action="ConfirmCodeController" method="post">
  <div class="container clearfix">
    <p class="text">Code:</p>
    <input class="_input" type="password" name="confirmationCode">
  </div>
  ${requestScope['ConfirmButton']}
</form>
  <div class="element"></div>
<form action="ConfirmCodeController" method="post">  
  ${requestScope['ConfirmButton2']}
</form>
  <img class="image image-2" src="images/blacklist2.png">
  <p class="text text-2">${requestScope['Error']}</p>
</body>
</html>