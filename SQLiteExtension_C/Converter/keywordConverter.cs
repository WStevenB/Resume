using System;
using System.IO;
using System.Collections.Generic;

class ConvertKeywords {
	public static void Main(string[] args) {
		string productionFile = args[0];
		string conversionFile = args[1];
		
		StreamReader sr = new StreamReader(productionFile);
		string productionText = sr.ReadToEnd();
		sr.Close();
		
		StreamReader sr2 = new StreamReader(conversionFile);
		string conversionText = sr2.ReadToEnd();
		sr2.Close();
		conversionText = conversionText.Trim();
		string[] conversionArray = conversionText.Split('\n');
		
		for(int i = 0; i<conversionArray.Length; i++) {
			string[] conversions = conversionArray[i].Split('\t');
			productionText = productionText.Replace(conversions[0], conversions[1]);
		}
		
		StreamWriter sw = new StreamWriter(System.IO.Path.GetFullPath(Directory.GetCurrentDirectory() + @"/convertedFile.txt"));
		sw.Write(productionText);
		sw.Close();
	}
}