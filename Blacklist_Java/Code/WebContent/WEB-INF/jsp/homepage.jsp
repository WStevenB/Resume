<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">



<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="initial-scale=1.0">
  <title>Blacklist Home</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/homepage-grid.css">
  <link rel="stylesheet" href="css/homepage.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-homepage clearfix" onload="setListener()">

 <!-- Start Floating Layer -->
<div id="FloatingLayer" style="position:absolute;width:350px;left:0px;top:0px;visibility:hidden; border:solid 1px #FF6600;">
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


  <img class="image image-1" src="images/par(2).png">
  <p class="text">Welcome ${requestScope['username']}</p>
  <form action="HomepageController" method="post">
    <button class="_button _button-2" type="submit" name="actionCode" value="1">Upload Respondents</button>
    <button class="_button _button-1" type="submit" name="actionCode" value="2">Chat Room</button>
    <div class="element"></div>
    <button class="_button _button-3" type="submit" name="actionCode" value="3">Update Account</button>
    <button class="_button _button-4" type="submit" name="actionCode" value="4">Sign Out</button>
  </form>
  <img class="image image-2" src="images/blacklist2.png">
  
  
	
	
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.3.1/jquery.min.js"></script>
<script>

var x = setInterval(function() {

    $("#messageContent").load("ChatAlertsController", function(responseTxt, statusTxt, xhr){
            if(responseTxt != "None" && responseTxt != "Fail") {
            	document.getElementById("FloatingLayer").style.visibility = "visible";
            }
    });
}, 8000);

function setListener() {
	window.addEventListener('scroll', resetAlert, true)
}

function resetAlert() {
	var top = document.documentElement.scrollTop;
	top = window.pageYOffset;
	document.getElementById("FloatingLayer").style.top = top;
}

function hideAlert() {
	document.getElementById("FloatingLayer").style.visibility = "hidden";
}
</script>

</body>
</html>