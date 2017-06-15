#include "findStarts.cpp"

using namespace std;

//recursive algorithm begins at a start point and expands until finding no point to jump to next
//used in removing trees and buildings
void explore(SDoublePlane* p, int count, int limit, int x, int y, vector< vector<int> >* records, int errors) {
	int errorLim = 3;
	vector<int> here;
	here.push_back(x);
	here.push_back(y);
	records->push_back(here);
	(*p)[y][x] = 90;
	if(count < limit) {
		int found = 0;
		if(x-1 >= 0 && (*p)[y][x-1] == 200) {
			found = 1;
			explore(p, count+1, limit, x-1, y, records, errors);
		}
		if(y-1 >= 0 && (*p)[y-1][x] == 200) {
			found = 1;	
			explore(p, count+1, limit, x, y-1, records, errors);
		}
		if(x+1 < p->cols() && (*p)[y][x+1] == 200) {
			found = 1;
			explore(p, count+1, limit, x+1, y, records, errors);
		}
		if(y+1 < p->rows() && (*p)[y+1][x] == 200) {
			found = 1;
			explore(p, count+1, limit, x, y+1, records, errors);
		}
		if(found == 0 && errors < errorLim) {
			for(int xx = 2; x-xx >= 0 && xx < 5 && found == 0; xx++) {
				if((*p)[y][x-xx] == 200) {
					found = 1;
					explore(p, count+1, limit, x-xx, y, records, errors+1);
				}
			}
			for(int xx = 2; x+xx < p->cols() && xx < 5 && found == 0; xx++) {
				if((*p)[y][x+xx] == 200) {
					found = 1;
					explore(p, count+1, limit, x+xx, y, records, errors+1);
				}
			}
			for(int yy = 2; y+yy < p->rows() && yy < 5 && found == 0; yy++) {
				if((*p)[yy+y][x] == 200) {
					found = 1;
					explore(p, count+1, limit, x, y+yy, records, errors+1);
				}
			}
			for(int yy = 2; y-yy >= 0 && yy < 5 && found == 0; yy++) {
				if((*p)[y-yy][x] == 200) {
					found = 1;
					explore(p, count+1, limit, x, y-yy, records, errors+1);
				}
			}
		}
	}
}
