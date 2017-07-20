<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Chat Room</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/chatroom-grid.css">
  <link rel="stylesheet" href="css/chatroom.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-chatroom clearfix">
  <img class="image" src="images/blacklist2.png">
  <p class="text text-3">Chat with the website administrator</p>
<form action="ChatroomController" method="post">  
  <button class="_button _button-1" type="submit" name="actionCode" value="2">Go Back</button>
</form>  
<form action="ChatroomController" method="post">
  <div class="container container-2 clearfix">
    <p class="text text-1">New Message:</p>
    <input class="_input" type="text" name="message">
  </div>
  <div class="container container-3 clearfix">
    
    ${requestScope['alertCheckbox']}
    <button class="_button" type="submit" name="actionCode" value="1">Submit</button>
  </div>
</form>
  <div class="element">
    <div class="container _element">
      <div id='chatTable'> ${requestScope['messages']} </div>
      <br><br><br>
	  ${requestScope['partners']}
    </div>
  </div>
  
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.3.1/jquery.min.js"></script>
<script>

var x = setInterval(function() {

    $("#chatTable").load("ChatController", function(responseTxt, statusTxt, xhr){
            
    });
}, 8000);
</script>
  
</body>
</html>
