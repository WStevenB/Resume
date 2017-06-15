#include "detect.cpp"

using namespace std;

//removes cars from list that have been placed too close together
vector< vector<int> > removeExtra(vector< vector<int> > cars, int pWidth, int pHeight, int cWidth, int cHeight, int closeFactor) {
	vector< vector<int> > newCars;
	for(int i = 0; i<cars.size(); i++) {
		int good = 1;
		if(cars[i][0] < 0 || cars[i][0] >= pWidth || cars[i][1] < 0 || cars[i][1] >= pHeight) {
			good = 0;
		}
		else {
			for(int ii = 0; ii<newCars.size(); ii++) {
				if(abs(cars[i][0] - newCars[ii][0]) < cWidth/closeFactor && abs(cars[i][1] - newCars[ii][1]) < cHeight/closeFactor) {
      				good = 0;
      				break;
      			}
			}
		}
		if(good == 1) newCars.push_back(cars[i]);
	}
	return newCars;
}

//blacks out a particular area of an edge map
void clearArea(SDoublePlane* med, int y, int x) {
	for(int yy = 0; yy<50; yy++) {
		for(int xx = 0; xx<50; xx++) {
			(*med)[yy+y][xx+x] = 10;
		}
	}		
}

//if no high contrast points are found in an area, the corresponding area in the low and medium edge map is blacked out
void clearBlankAreas(SDoublePlane* h, SDoublePlane *med) {
	SDoublePlane high = *h;
	for(int y = 0; y<high.rows()-50; y+=50) {
		for(int x = 0; x<high.cols()-50; x+=50) {
			int foundOne = 0;
			for(int yy = 0; yy<50 && foundOne == 0; yy++) {
				for(int xx = 0; xx<50; xx++) {
					if(high[yy+y][xx+x] == 200) {
						foundOne = 1;
						break;
					}
				}
			}
			if(foundOne == 0) clearArea(med, y, x);
		}
	}
	
}

