<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <meta name="viewport" content="initial-scale=1.0">
  <title>Upload Respondents</title>
  <link rel="stylesheet" href="css/standardize.css">
  <link rel="stylesheet" href="css/upload-grid.css">
  <link rel="stylesheet" href="css/upload.css">
  <link rel="stylesheet" type="text/css" href="stylesheet.css">
</head>
<body id="container" class="body page-upload clearfix" onload="setListener()">

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


  <img class="image" src="images/blacklist2.png">
  <p class="text text-4">Format: First Name \t Last Name \t Email Address \t Phone Number \t Zip Code \n</p>
  <p class="text text-1">Matches: Name + Email, Name + Phone, Name + Zip, or Email + Phone + Zip</p>
  
<form action="FileUploadController" method="post">  
  <button class="_button _button-4" type="submit" name="actionCode" value="2">Go Back</button>
</form>

<form action="FileUploadController" method="post" enctype="multipart/form-data">
  <input type="button" class="_button _button-2" id="fileButton" value="Choose File" onclick="document.getElementById('fileUpload').click();" />
  <input type="file" style="display:none;" id="fileUpload" name="file"/>
  
  <!--  <input id="fileUpload" class="fileInput" type="file" name="file"/> -->
  <p class="text text-3 fileLabel" id="fileLabel" >none selected</p>
  <button class="_button _button-3" id="uploadButton" type="submit" name="actionCode" value="1">Upload</button> 
</form>

  <p class="text text-2">${requestScope['Matches']}</p>
  ${requestScope['DownloadButton']}
  <div class="element"><div class="container">${requestScope['MatchTable']}</div></div>
  
	
	
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
	var input = document.getElementById('fileUpload');
	var label = document.getElementById('fileLabel');
	var upload = document.getElementById('uploadButton');
	upload.style.visibility = "hidden";

	input.addEventListener( 'change', function( e )
	{
		var fileName = '';
		if( this.files && this.files.length > 1 )
			fileName = ( this.getAttribute( 'data-multiple-caption' ) || '' ).replace( '{count}', this.files.length );
		else
			fileName = e.target.value.split( '\\' ).pop();

		if( fileName ) {
			label.innerHTML = fileName;
			upload.style.visibility = "visible";
		}

	});
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
