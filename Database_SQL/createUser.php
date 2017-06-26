<?php
$server = "127.0.0.1";
$user = "user";
$pwd = "password";
$db = "database";

$Key = $_GET['key'];
$Key = prepareArg($Key);

//check if key is correct before preceding
if($Key == 'ha7shd87h12ukj3hygas5782634ihfdjjas9df03984hjfsduhu') {

	//get other parameters
	$Email = $_GET['arg1'];
	$Name = $_GET['arg2'];
	$PasswordHash = $_GET['arg3'];
	
	//check parameters' length
	if(strlen($Email) < 100 && strlen($Name) < 100 && strlen($PasswordHash) < 100) {

		//create my sql connection object
		$conn = new mysqli($server, $user, $pwd, $db);
		
		//create prepared statement
		$stmt = $conn->prepare('INSERT INTO User VALUES(?,?,?,0);');
		
		//bind parameters to avoid sql injection
		$stmt->bind_param("sss", $Email, $Name, $PasswordHash);
		
		//execute statement
		$result = $stmt->execute();
		
		//if database insert was successful, respond with success
  		if($result) {echo "Success";}
  		
  		//otherwise respond with a fail message
		else {echo "Fail2";}
		
		//clean up and close connection objects
		$stmt->free_result();
		$conn->close();
	}
	
	//parameters were too long, suspicious
	else {echo "Fail1";}
}


?>