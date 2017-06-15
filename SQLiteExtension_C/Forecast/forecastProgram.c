#include "sqlite3.c"
#include <stdio.h>


void saveToFile(char* text) {
	FILE *f = fopen("/Users/walterstevenbabcock/Desktop/results.txt", "w");
	if (f == NULL) {
    	printf("Error opening file!\n");
	}
	fprintf(f, "%s", text);
	fclose(f);	
}


int main(int argc, char *argv[]) {
	
	//validate input
	if(argc != 2) return 9;
    if(strlen(argv[1]) > 100) return 10;
    
    //connect to sqlite database file
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open("/Users/walterstevenbabcock/Desktop/test5", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 8;
    }
    
    //copy input into local variable
    //test first letter is S for SELECT
    char text[100];
    strcpy(text, argv[1]);
    if(text[0] != 'S' && text[0] != 's') return 8;
    
    
    rc = sqlite3_prepare_v2(db, text, -1, &res, 0);    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 7;
    }
    
    //calculate the number of columns in result of query
    //* is taken to mean 4 since there is only one table in this db
    //otherwise commas are counted until the FROM keyword
    int columns = 1;
    for(int i = 0; i<100; i++) {
    	if(text[i] == ',') columns++;
    	else if(text[i] == '*') {
    		columns = 4;
    		break;
    	}
    	else if(text[i] == 'F' || text[i] == 'f') {
    		if(text[i+1] == 'R' || text[i+1] == 'r')
    			if(text[i+2] == 'O' || text[i+2] == 'o')
    				if(text[i+3] == 'M' || text[i+3] == 'm')
    					break;
    	}
    }    
    
    
    //loop through results and save to file for display in web browser
    char result [10000];
    
    rc = sqlite3_step(res);
    if(rc != SQLITE_ROW) {
    	strcpy(result, "NO RESULTS");
    	saveToFile(&result);
    	return 1;
    }
    strcpy(result, sqlite3_column_text(res, 0));
    strcat(result, "&nbsp;&nbsp;&nbsp;&nbsp;");
    for(int i = 1; i<columns; i++) {
    	strcat(result, sqlite3_column_text(res, i));
    	strcat(result, "&nbsp;&nbsp;&nbsp;&nbsp;");
    }
    strcat(result, "<br>");
    rc = sqlite3_step(res);
    while (rc == SQLITE_ROW) {
        for(int i = 0; i<columns; i++) {
        	strcat(result, sqlite3_column_text(res, i));
        	strcat(result, "&nbsp;&nbsp;&nbsp;&nbsp;");
        }
        strcat(result, "<br>");
        rc = sqlite3_step(res);
    }
    saveToFile(&result);  
    sqlite3_finalize(res);
    sqlite3_close(db);
    
    return 1;
}