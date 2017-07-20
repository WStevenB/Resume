<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Forgot Password</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/forgotpassword-grid.css">
  <link rel="stylesheet" href="css/forgotpassword.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-forgotpassword clearfix">
  <img class="image image-1" src="images/par(2).png">
  <p class="_button _button-1">Send confirmation code to reset password</p>

<form action="ForgotPassController" method="post">  
  <div class="container clearfix">
    <p class="text">Email:</p>
    <input class="_input" type="text" name="email">
  </div>
  <button class="_button _button-2" type="submit" name="actionCode" value="1">Submit</button>
</form>  
  
  <p class="text text-2">${requestScope['Error']}</p>
  
  <div class="element"></div>
<form action="ForgotPassController" method="post">  
  <button class="_button _button-3" type="submit" name="actionCode" value="2">Cancel</button>
</form>  
  <img class="image image-2" src="images/blacklist2.png">
  
</body>
</html>