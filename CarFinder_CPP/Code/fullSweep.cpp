#include "scanForDim.cpp"

using namespace std;

void fullSweep(SDoublePlane* p, int xD, int yD, int* max, vector< vector< vector<int> > >* records) {
	SDoublePlane pic = *p;
	
	//generates a car template according to supplied dimension
	SDoublePlane vertFilter(yD,xD);
	for(int yy = 0; yy<yD; yy++) {
		for(int xx = 0; xx<xD; xx++) {
			vertFilter[yy][xx] = 0;
			if(yy<3 || yy>yD-3) vertFilter[yy][xx] = 1;
			if(xx<3 || xx>xD-3) vertFilter[yy][xx] = 1;
			if((yy<6 || yy>yD-6) && (xx<6 || xx>xD-6)) vertFilter[yy][xx] = 1;
		}
	}
	
	//convolves template through edge map
	//pushes location of car into hash table at index of its template score
	for(int y = 0; y<pic.rows()-yD; y++) {
		for(int x = 0; x<pic.cols()-xD; x++) {
			int total = 0;
			int yFac = 10;
			for(int yy = 0; yy<yD; yy++) {
				for(int xx = 0; xx<xD; xx++) {
					total += pic[yy+y][xx+x] * vertFilter[yy][xx];
				}
			}
			if(total > *max) *max = total;
			vector<int> oPair;
			oPair.push_back(x);
			oPair.push_back(y);
			(*records)[total].push_back(oPair);
		}
	}
}