//declare sql server connection object
SqlConnection myConn;

//use try/catch for error handling, database processes are generally unsafe
try {

	//assign sql server connection via info in app.config file
	myConn = new SqlConnection(ConfigurationManager.ConnectionStrings["myString"].ConnectionString);
	myConn.Open();

	//build query string with parameters for each input
	string query = "INSERT INTO Item (Description, BinLocation, Notes, ItemLookupCode, " + 
		"Price, SalePrice, TaxID, Cost, ParentQuantity, " +
		"SupplierID, SubDescription1, SubDescription2, SubDescription3, WebItem, LastUpdated, " +
		"DateCreated, PictureName, ReorderPoint, BarcodeFormat) " +
		"VALUES (@description, @binlocation, @notes, @ilc, " +
		"@price, @salesprice, @taxid, @cost, @quantity, @supp, " +
		"@sub1, @sub2, @sub3, @web, @update, @create, @picname, @reorder, @barcode)";

	//new sql command
	SqlCommand itemInsert = new SqlCommand(query, myConn);

	//insert values into all parameters
	//some come from text boxes, other combo boxes, others have both but prefer combo boxes
	insertParameter(itemInsert, "@description", DbType.String, DescriptionTextBox.Text);
	
	string bin = LocationTextBox.Text;
	if(BinLocationCombo.SelectedIndex >= 0 && BinLocationCombo.Visible == true) bin = BinLocationCombo.SelectedItem.ToString();
	insertParameter(itemInsert, "@binlocation", DbType.String, bin);
	
	insertParameter(itemInsert, "@notes", DbType.String, NotesTextBox.Text);
	insertParameter(itemInsert, "@ilc", DbType.String, ItemLookupTextBox.Text);
	insertParameter(itemInsert, "@price", DbType.Currency, PriceTextBox.Text.Replace("$",""));
	insertParameter(itemInsert, "@salesprice", DbType.Currency, SalePriceTextBox.Text.Replace("$", ""));
	insertParameter(itemInsert, "@taxid", DbType.Int16, TaxTextBox.Text);
	insertParameter(itemInsert, "@cost", DbType.Currency, CostTextBox.Text.Replace("$", ""));
	insertParameter(itemInsert, "@quantity", DbType.Double, QuantityTextBox.Text);
	
	string s1 = SizeTextBox.Text;
	if (SizeCombo.SelectedIndex >= 0 && SizeCombo.Visible == true) s1 = SizeCombo.SelectedItem.ToString();
	insertParameter(itemInsert, "@sub1", DbType.String, s1);
	
	string s2 = PackageTextBox.Text;
	if (PackageCombo.SelectedIndex >= 0 && PackageCombo.Visible == true) s2 = PackageCombo.SelectedItem.ToString();
	insertParameter(itemInsert, "@sub2", DbType.String, s2);
	
	string s3 = ContainerTextBox.Text;
	if (ContainerCombo.SelectedIndex >= 0 && ContainerCombo.Visible == true) s3 = ContainerCombo.SelectedItem.ToString();
	insertParameter(itemInsert, "@sub3", DbType.String, s3);
	                        
	insertParameter(itemInsert, "@web", DbType.Boolean, WebItemCheckBox.Checked);
	
	string time = DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss tt");
	if (time[0] != '1') time = time.Substring(1);
	DateTime now = Convert.ToDateTime(time);
	insertParameter(itemInsert, "@update", DbType.DateTime, now);
	insertParameter(itemInsert, "@create", DbType.DateTime, now);
	
	insertParameter(itemInsert, "@picname", DbType.String, imageName);
	insertParameter(itemInsert, "@reorder", DbType.Double, ReorderTextBox.Text);
	
	int barcode = BarCodeCombo.SelectedIndex;
	if (barcode < 0) barcode = 0;
	insertParameter(itemInsert, "@barcode", DbType.Int16, barcode);

	//execute statement
	itemInsert.ExecuteNonQuery();
	
	//close database connection
	myConn.Close();
}

//if something went wrong, try to close connection and show error message
catch(Exception er) { 
	try {
		myConn.Close();
	}
	catch {}
	MessageBox.Show(er.Message); 
}




//used to parameterize sql commands in an effort to prevent injection attacks
private void insertParameter(MySqlCommand com, string name, DbType type, object val) {
    IDataParameter para = com.CreateParameter();
    para.ParameterName = name;
    para.DbType = type;
    para.Value = val;
    com.Parameters.Add(para);
}


