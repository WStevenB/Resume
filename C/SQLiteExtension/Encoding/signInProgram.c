#include "sqlite3.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
	
	//validate input
	if(argc != 3) return 9;
    if(strlen(argv[1]) > 100 || strlen(argv[2]) > 100) return 10;
    
    // create connection objects and open my instance of sqlite db
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open("/Users/walterstevenbabcock/Desktop/test4", &db);
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 8;
    }
    
    //build query string from user inputs
    //this is highly insecure, but the point is actually to allow SQL injection
    //these attacks will be stopped using a new encoding technique later
    char text[500];
    strcpy(text, "SELECT COUNT(1) FROM User WHERE Name = '");
    strcat(text, argv[1]);
    strcat(text, "' AND Password = '");
    strcat(text, argv[2]);
    strcat(text, "';");
    
    rc = sqlite3_prepare_v2(db, text, -1, &res, 0);    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 7;
    }    
    
    //move to first row and copy result
    rc = sqlite3_step(res);
    char result[10];
    strcpy(result, sqlite3_column_text(res, 0));
    
    sqlite3_finalize(res);
    sqlite3_close(db);
    
    int cmp = strncmp(result, "0", 10);
    if(cmp != 0) printf("%s\n", "Sign In Success!");
    else printf("%s\n", "Sign In Failure...");
    
    if(cmp != 0) return 1;
    return 0;
}