using System;
using System.IO;
using System.Collections.Generic;

class EncodeSQLKeywords {
	public static void Main() {
		string fullPath = System.IO.Path.GetFullPath(Directory.GetCurrentDirectory() + @"/encodeKeywords.txt");
		StreamReader sr = new StreamReader(fullPath);
		string oldFileText = sr.ReadToEnd();
		sr.Close();
		
		string fullPath2 = System.IO.Path.GetFullPath(Directory.GetCurrentDirectory() + @"/oldKeywords.txt");
		StreamReader sr2 = new StreamReader(fullPath2);
		string text2 = sr2.ReadToEnd();
		sr.Close();
		text2 = text2.Trim();
		string[] oldKeywords = text2.Split('\n');
		
		
		Random rand = new Random();
		List<string> newKeywordsList = new List<string>();
		for(int i = 0; i<125; i++) newKeywordsList.Add(getNewKeyword(rand));
		
		
		string zText = "";
		int count = 0;
		for(int i = 0; i<newKeywordsList.Count; i++) {
			for(int ii = 0; ii<newKeywordsList[i].Length; ii++) {
				zText += "'" + newKeywordsList[i][ii].ToString() + "', ";
				count++;
			}
			if( i%2 == 1) zText += "\n";
		}
		oldFileText = oldFileText.Replace("valuesZText!", zText);
		oldFileText = oldFileText.Replace("lengthZText!", count.ToString());
		
		
		string lengthValues = "";
		for(int i = 0; i<125; i++) {
			lengthValues += "12, ";
			if(i%24 == 23) lengthValues += "\n";
		}
		oldFileText = oldFileText.Replace("lengthValues!", lengthValues);
		
		string offsetValues = "";
		count = 0;
		for(int i = 0; i<125; i++) {
			offsetValues += count.ToString() + ", ";
			count += 12;
			if(i%24 == 23) offsetValues += "\n";
		}
		oldFileText = oldFileText.Replace("offsetValues!", offsetValues);
		
		
		List<string> hashList = new List<string>();
		List<string> hashList2 = new List<string>();
		List< List<int> > collisions = new List< List<int> >();
		
		for(int i = 0; i<127; i++) {
			hashList.Add("0");
			hashList2.Add("0");
			collisions.Add(new List<int>());
		}
		
		for(int i = 0; i<125; i++) {
			string word = newKeywordsList[i].ToLower();
			int hash = ((word[2]*4) ^ (word[11]*3) ^ (word[6]*2)) % 127;
			hashList2[i] = hash.ToString();
			if(hashList[hash] == "0") hashList[hash] = (i+1).ToString();
			collisions[hash].Add(i+1);
		}
		
		string hashValues = "";
		for(int i = 0; i<127; i++) {
			hashValues += hashList[i] + ", ";
			if(i%24 == 23) hashValues += "\n";
		}
		oldFileText = oldFileText.Replace("hashValues!", hashValues);
		
		List<string> collisionList = new List<string>();
		for(int i = 0; i<125; i++) collisionList.Add("0");
		for(int i = 0; i<125; i++) {
			if(collisions[i].Count > 1) {
				for(int ii = 0; ii<collisions[i].Count-1; ii++) {
					collisionList[collisions[i][ii]-1] = (collisions[i][ii+1]).ToString();
				}
			}
		}
		string collisionValues = "";
		for(int i = 0; i<125; i++) {
			collisionValues += collisionList[i] + ", ";
			if(i%24==23) collisionValues += "\n";
		}
		oldFileText = oldFileText.Replace("nextValues!", collisionValues);
		
		
		string writePath = System.IO.Path.GetFullPath(Directory.GetCurrentDirectory() + @"/newFunction.txt");
		StreamWriter sw = new StreamWriter(writePath);
		sw.Write(oldFileText);
		sw.Close();
		
		
		
		
		
		string conversionText = "";
		for(int i = 0; i<oldKeywords.Length; i++) {
			conversionText += oldKeywords[i] + "\t" + newKeywordsList[i] + "\t" + hashList2[i] + "\n";
		}	
		string writePath2 = System.IO.Path.GetFullPath(Directory.GetCurrentDirectory() + @"/conversionFile.txt");
		StreamWriter sw2 = new StreamWriter(writePath2);
		sw2.Write(conversionText);
		sw2.Close();
		
	}
	
	public static string getNewKeyword(Random rand) {
		string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string word = "XZ";
		for(int i = 0; i<10; i++) {
			int num = rand.Next(0, chars.Length -1);
    		word += chars[num].ToString();
		}
		return word;
	}
}