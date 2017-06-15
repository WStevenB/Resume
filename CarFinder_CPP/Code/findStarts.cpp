#include "removeExtra.cpp"

using namespace std;

//finds points of high contrast in edge map
vector< vector<int> > findStartPoints(SDoublePlane *h) {
	vector< vector<int> > starts;
	SDoublePlane high = *h;
	for(int y = 0; y<high.rows(); y++) {
		for(int x = 0; x<high.cols(); x++) {
			if(high[y][x] == 200) {
				vector<int> s;
				s.push_back(x);
				s.push_back(y);
				starts.push_back(s);
			}
		}
	}
	return starts;
}

//finds starting points of cars so that it is intersection of horizontal and vertical edges that roughly match estimated dimension
vector< vector<int> > findStartPoints2(int *biggestDist, vector< vector<int> > *fS, SDoublePlane copy, SDoublePlane *h) {
	vector< vector<int> > firstSweep = *fS;
	SDoublePlane high = *h;
	vector< vector<int> > startPoints;
	for(int i = 0; i<firstSweep.size(); i++) {
  		vector<int> p;
  		p.push_back(firstSweep[i][0]);
  		p.push_back(firstSweep[i][1]);
  		vector<int> topRight = p;
  		vector<int> topLeft = p;
  		vector<int> bottomRight = p;
  		startPoints.push_back(p);
  		int keepGoing = 1;
  		int numForgiven = 0;
  		int forgiveFactor = 3;
  		
  		while(keepGoing == 1) {
  			if(topRight[1]+1 < high.rows() && high[topRight[1]+1][topRight[0]] == 200) {
  				topRight[1] = topRight[1]+1;
  				copy[topRight[1]][topRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topRight[0]+1 < high.cols() && high[topRight[1]][topRight[0]+1] == 200) {
  				topRight[0] = topRight[0]+1;
  				copy[topRight[1]][topRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topRight[0]+1 < high.cols() && topRight[1]+1 < high.rows() && high[topRight[1]+1][topRight[0]+1] == 200) {
  				topRight[0] = topRight[0]+1;
  				topRight[1] = topRight[1]+1;
  				copy[topRight[1]][topRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topRight[1]+1 < high.rows()) {
  				numForgiven++;
  				if(numForgiven > forgiveFactor) keepGoing = 0;
  				topRight[1] = topRight[1]+1;
  			}
  			else keepGoing = 0;
  		}
  		keepGoing = 1;
  		numForgiven = 0;
  		while(keepGoing == 1) {
  			if(topLeft[0]-1 >= 0 && high[topLeft[1]][topLeft[0]-1] == 200) {
  				topLeft[0] = topLeft[0]-1;
  				copy[topLeft[1]][topLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topLeft[1]+1 < high.rows() && high[topLeft[1]+1][topLeft[0]] == 200) {
  				topLeft[1] = topLeft[1]+1;
  				copy[topLeft[1]][topLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topLeft[0]-1 >= 0 && topLeft[1]+1 < high.rows() && high[topLeft[1]+1][topLeft[0]-1] == 200) {
  				topLeft[0] = topLeft[0]-1;
  				topLeft[1] = topLeft[1]+1;
  				copy[topLeft[1]][topLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(topLeft[0]-1 >= 0) {
  				numForgiven++;
  				if(numForgiven > forgiveFactor) keepGoing = 0;
  				topLeft[0] = topLeft[0]-1;
  			}
  			else keepGoing = 0;
  		}
  		keepGoing = 1;
  		numForgiven = 0;
  		while(keepGoing == 1) {
  			if(bottomRight[0]+1 < high.cols() && high[bottomRight[1]][bottomRight[0]+1] == 200) {
  				bottomRight[0] = bottomRight[0]+1;
  				copy[bottomRight[1]][bottomRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomRight[1]-1 >= 0 && high[bottomRight[1]-1][bottomRight[0]] == 200) {
  				bottomRight[1] = bottomRight[1]-1;
  				copy[bottomRight[1]][bottomRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomRight[0]+1 < high.cols() && bottomRight[1]-1 >= 0 && high[bottomRight[1]-1][bottomRight[0]+1] == 200) {
  				bottomRight[0] = bottomRight[0]+1;
  				bottomRight[1] = bottomRight[1]-1;
  				copy[bottomRight[1]][bottomRight[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomRight[0]+1 < high.cols()) {
  				numForgiven++;
  				if(numForgiven > forgiveFactor) keepGoing = 0;
  				bottomRight[0] = bottomRight[0]+1;
  			}
  			else keepGoing = 0;
  		}
  		int d1 = sqrt(pow(abs(firstSweep[i][0] - topLeft[0]),2) + pow(abs(firstSweep[i][1] - topLeft[1]),2));
  		int d2 = sqrt(pow(abs(firstSweep[i][0] - topRight[0]),2) + pow(abs(firstSweep[i][1] - topRight[1]),2));
  		int d3 = sqrt(pow(abs(firstSweep[i][0] - bottomRight[0]),2) + pow(abs(firstSweep[i][1] - bottomRight[1]),2));
  		if(d1>*biggestDist) *biggestDist = d1;
  		if(d2>*biggestDist) *biggestDist = d2;
		if(d3>*biggestDist) *biggestDist = d3;
		
		//if(d1+d2+d3 < 18) startPoints.pop_back();
  	/*}*/
  	
  }
  return startPoints;
}