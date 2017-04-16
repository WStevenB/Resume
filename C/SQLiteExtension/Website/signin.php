<?php
	$Result = -1;
	$Result2 = -1;
	$ReturnText = "";
	$ReturnCode = -1;
	$ReturnText2 = "";
	$ReturnCode2 = -1;
	
	if(isset($_GET['arg1']) && isset($_GET['arg2'])) {
		$Name = '"'.$_GET['arg1'].'"';
		$Password = '"'.$_GET['arg2'].'"';
		if(strlen($Name) > 2 && strlen($Password) > 2) {
			$Result = exec('/Users/walterstevenbabcock/Desktop/IndependentStudyEncoded/run_signIn_encoded '.$Name.' '.$Password, $ReturnText, $ReturnCode);
		}
	}
	
	if(isset($_GET['arg3']) && isset($_GET['arg4'])) {
		$Name2 = '"'.$_GET['arg3'].'"';
		$Password2 = '"'.$_GET['arg4'].'"';
		if(strlen($Name2) > 2 && strlen($Password2) > 2) {
			$Result2 = exec('/Users/walterstevenbabcock/Desktop/IndependentStudy/Code/run_signIn '.$Name2.' '.$Password2, $ReturnText2, $ReturnCode2);
		}
	}
	echo "Below are two forms that imitate sign-in pages for a highly unsecure website<br><br>";
	echo "There are two accounts...<br>";
	echo "Steve  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   password<br>";
	echo "Dude  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   Sweet<br><br><br>";
	echo "There exists a common hacking weakness known as SQL injection<br>";
	echo "It ranks #1 on OWASP's list of website vulnerabilities<br><br>";
	echo "Try typing the following into both forms...<br>";
	echo "Steve      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      stuff' OR '1'='1<br><br><br>";
	echo "You sent information to a database which it used to form a query string resembling below...<br>";
	echo "SELECT COUNT(1) FROM User WHERE Name = 'Steve' AND Password = 'stuff' OR '1'='1';<br><br>";
	echo "Basically, you just tricked the database by providing an unexpected, always true condition<br>";
	echo "There are many inputs you could use to bypass the intent of the database if you'd like to experiment<br><br>";
	echo "There are complex security features to combat this, and complex attacks as well<br>";
	echo "The bottom form connects to a different database, and is not fooled by the above trick<br>";
	echo "A hack exists nevertheless... see if you can figure it out!<br>";
	echo "Click the bottom-most button for the solution<br><br><br><br>";
	
	echo "<div style='background-color:lightslategray;'>";
	echo 'With no security features<br><br>';
	echo '<form action="runSqlite.php" method="get">';
	echo '<input type="text" name="arg3" />';
	echo '<input type="text" name="arg4" />';
	echo 'Sign In <input type="submit" />';
	echo '</form>';
	if($ReturnCode2 != 1 && $ReturnCode2 != 2 && $ReturnCode2 != -1) {echo "Sign In Failed...";}
	if($ReturnCode2 == 1 || $ReturnCode2 == 2) {echo "Sign In Success!";}
	echo "</div><br><br><br><br><br><br>";
	
	
	echo "<div style='background-color:lightslategray;'>";
	echo 'Only one new security feature<br><br>';
	echo '<form action="runSqlite.php" method="get">';
	echo '<input type="text" name="arg1" />';
	echo '<input type="text" name="arg2" />';
	echo 'Sign In <input type="submit" />';
	echo '</form>';
	if($ReturnCode != 1 && $ReturnCode != 2 && $ReturnCode != -1) {echo "Sign In Failed...";}
	if($ReturnCode == 1 || $ReturnCode == 2) {echo "Sign In Success!";}
	echo "</div><br><br><br>";
	
	
	echo "<button style='color:orange;' onclick='document.getElementById(\"solution\").style.visibility = \"visible\";'><font size='5'>What's Going On?</font></button><br><br>";
	echo "<div id='solution' style='visibility:hidden'>";
	echo "Now try typing this into the bottom form...<br>";
	echo "Steve  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   asdf' XZNJUMKUANJO '1'='1  <br><br>";
	echo "It works to the detriment of the database's security, but who would ever think to type this in?<br>";
	echo "Hopefully no one! As an independent study project at IU, I've written programs to alter the source code of SQLite<br>";
	echo "All SQL keywords are randomly tokenized, and a conversion file is outputted for future use with queries and webserver files<br><br>";
	echo "So it will accept queries in the following form...<br>";
	echo "XZQJOUBRSALC COUNT(1) XZIBCLQUNDSU User XZHVMTORYDGW Name = 'Steve' XZGSEMFANFEO Password = 'password';<br>";
	echo "The keywords randomized above are SELECT FROM WHERE & AND<br><br>";
	echo "Other security measures are much more complex, but I think overlook the fundamental reason why a system can be hacked...<br>";
	echo "The hacker correctly assumes he or she knows the machine's language<br>";
	echo "I think a similar idea could be used to greatly improve operating system security<br>";
	echo "Thanks for reading, and I hope you learned a bit about web security!<br>";
	echo "</div><br><br><br><br><br><br><br><br>";
	
	
	
	
?>