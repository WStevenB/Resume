<?php
	$ExitCode = -1;
	$ReturnText = "";
	$ReturnCode = -1;
	$Result = "";
	
	if(isset($_GET['arg1'])) {
		$Query = '"'.$_GET['arg1'].'"';
		if(strlen($Query) > 2) {
			$ExitCode = exec('/Users/walterstevenbabcock/Desktop/IndependentStudy/Code/forecast '.$Query, $ReturnText, $ReturnCode);
		}
		if($ReturnCode == 1) {
			$logfile = '/Users/walterstevenbabcock/Desktop/results.txt';
    		$Result = file_get_contents($logfile);
		}
		else {
			$Result = "An error has occurred...";
		}
	}
	
	echo "FORECAST is a keyword I've added to SQL to improve the syntax and efficiency of database processes<br>";
	echo "It is used with queries involving both a function and a condition on the output of the function<br>";
	echo "Consider the following table...<br><br>";
	echo "Purchase<br>";
	echo "Customer  &nbsp;&nbsp;&nbsp;&nbsp;   DateOfSale  &nbsp;&nbsp;&nbsp;&nbsp;   Item  &nbsp;&nbsp;&nbsp;&nbsp;   Price  <br><br>";
	echo "Evan  &nbsp;&nbsp;&nbsp;&nbsp;   20170401  &nbsp;&nbsp;&nbsp;&nbsp;   Coca-cola  &nbsp;&nbsp;&nbsp;&nbsp;   2  <br>";
	echo "Sally  &nbsp;&nbsp;&nbsp;&nbsp;   20170211  &nbsp;&nbsp;&nbsp;&nbsp;   Pepsi  &nbsp;&nbsp;&nbsp;&nbsp;   1  <br>";
	echo "Dave  &nbsp;&nbsp;&nbsp;&nbsp;   20160311  &nbsp;&nbsp;&nbsp;&nbsp;   Dr Pepper  &nbsp;&nbsp;&nbsp;&nbsp;   3  <br>";
	echo "Vivian  &nbsp;&nbsp;&nbsp;&nbsp;   20151031  &nbsp;&nbsp;&nbsp;&nbsp;   Mountain Dew  &nbsp;&nbsp;&nbsp;&nbsp;   10  <br>";
	echo "Tyler  &nbsp;&nbsp;&nbsp;&nbsp;   20141212  &nbsp;&nbsp;&nbsp;&nbsp;   Sprite  &nbsp;&nbsp;&nbsp;&nbsp;   5  <br>";
	echo "Becky  &nbsp;&nbsp;&nbsp;&nbsp;   20130511  &nbsp;&nbsp;&nbsp;&nbsp;   Fanta  &nbsp;&nbsp;&nbsp;&nbsp;   4  <br><br>";
	echo "With traditional SQL, one query may resemble the following...<br>";
	echo "SELECT Total FROM (SELECT SUM(Price) AS Total FROM Purchase) WHERE Total > 10;<br><br>";
	echo "This statement has one query nested inside another<br>";
	echo "The syntax can become very confusing with more layers of nesting<br><br>";
	echo "It's tempting to instead write the following because it seems to make sense and read much better<br>";
	echo "SELECT SUM(Price) AS Total FROM Purchase WHERE Total > 10;<br><br>";
	echo "However, this is the result...<br>";
	echo "Error: misuse of aggregate: SUM()<br>";
	echo "The WHERE clause can only refer to the input of a query<br><br>";
	echo "That's where FORECAST comes into play<br>";
	echo "It is a set of conditions on the output of a query<br><br>";
	echo "SELECT SUM(Price) AS Total FROM Purchase FORECAST Total > 10;<br><br>";
	echo "Now use it to build more complex yet concise queries...<br>";
	echo "SELECT SUM(Price) AS Total FROM Purchase WHERE Price > 3 FORECAST Total > 10;<br><br>";
	echo "Very similar to WHERE, but most similar to another keyword called HAVING<br>";
	echo "I've found very modest improvement in the running time of some queries (less than 1%)<br>";
	echo "But improvement in syntax is its main benefit at this point<br>";
	echo "Below is a text field where you can enter your own queries to experiment with this new keyword<br>";
	echo "It is limited to SELECT statements<br><br><br>";
	
	echo "<div style='background-color:lightslategray;'>";
	echo 'Query Purchase Table<br><br>';
	echo '<form action="runForecast.php" method="get">';
	echo '<input style="width: 500px;" type="text" name="arg1" />';
	echo '<input type="submit" />';
	echo '</form><br><br>';
	echo $Result;
	echo "</div><br><br><br><br>";
	
	echo "Thanks for reading, and I hope you learned a bit about SQL!<br>";
	
	
	
	
?>