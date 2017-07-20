<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Update Account</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/changeinfo-grid.css">
  <link rel="stylesheet" href="css/changeinfo.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-changeinfo clearfix" onload="setListener()">
  <img class="image image-2" src="images/par(2).png">
  <p class="_button _button-2">Update Account Information</p>
<form action="UpdateAccountController" method="post">  
  <div class="container container-2 clearfix">
    <p class="text text-3">New Email:</p>
    <input class="_input _input-2" type="text" name="newEmail" value="${requestScope['email']}">
  </div>
  <div class="container container-4 clearfix">
    <p class="text text-6">New Name:</p>
    <input class="_input _input-4" type="text" type="text" name="newUsername" value="${requestScope['username']}">
  </div>
  <button class="_button _button-1" type="submit" name="actionCode" value="1">Submit</button>
</form>  
  <p class="text text-1">${requestScope['Error1']}</p>
  
 <form action="UpdateAccountController" method="post"> 
  <div class="container container-3 clearfix">
    <p class="text text-5">New Password:</p>
    <input class="_input _input-3" type="password" name="password">
  </div>
  <div class="container container-1 clearfix">
    <p class="text text-2">Confirm Password:</p>
    <input class="_input _input-1" type="password" name="confirmPassword">
  </div>
  <button class="_button _button-3" type="submit" name="actionCode" value="2">Submit</button>
</form>  
  <p class="text text-4">${requestScope['Error2']}</p>
  
  <img class="image image-1" src="images/blacklist2.png">
  
  <div class="element"></div>
<form action="UpdateAccountController" method="post">  
  <button class="_button _button-4" type="submit" name="actionCode" value="3">Go Back</button>
</form> 

<div id="hiddenContent" style="visibility:hidden;"></div>	
<!-- Start Floating Layer -->
<div id="FloatingLayer" style="z-index:1000;position:absolute;width:350px;left:0px;top:0px;visibility:hidden; border:solid 1px #FF6600;">
    <div id="titleBar" style="cursor:move; width:100%; position:relative; border-bottom:solid 1px #FF6600; background-color:#FF9933;">
        <div id="title" style="margin-right:30px; padding-left:3px;">
            <font face="Arial" color="#333333">Message From Website Admin</font>
        </div>
        <div id="closeX" style="cursor:hand; position:absolute; right:5px; top:0px;">
            <a href="#" onClick="hideAlert();return false"  style="text-decoration:none"><font color="#333333" size="2" face="arial">X</font></a>
        </div>
    </div>
    <div id="floatingContent" style="padding:3px; background-color:#CCCCCC; color:#333333;">
        <!-- place your HTML content here-->
	    <div id="messageContent">
	    	Hi, I'm Steve the website admin. Feel free to shoot me a message in the chatroom if you have any questions.
		</div>
		<br>
		<form action="ChatroomController" method="post">
			<input type="hidden" name="actionCode" value=901 />
			Go To Chatroom <input type="submit" />
		</form>
        <!-- End of content area -->
    </div>
</div>	
	
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.3.1/jquery.min.js"></script>
<script>

var x = setInterval(function() {
    $("#hiddenContent").load("ChatAlertsController", function(responseTxt, statusTxt, xhr){
            if(responseTxt != "None" && responseTxt != "Fail") {
            	document.getElementById("FloatingLayer").style.visibility = "visible";
            	document.getElementById('messageContent').innerHTML = responseTxt;
            }
    });
}, 8000);

function setListener() {
	document.addEventListener('scroll', resetAlert, true);
}

function resetAlert() {
	var top = $(window).scrollTop();
	$('#FloatingLayer').css('top', top);
}

function hideAlert() {
	document.getElementById("FloatingLayer").style.visibility = "hidden";
}
</script>

 
</body>
</html>
