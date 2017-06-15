#include "fullSweep.cpp"

using namespace std;

//this function receives the rotated edge map, estimated number of cars, and a dimension to create car templates
vector<DetectedBox> finalGo(SDoublePlane* p, SDoublePlane* inputIm, SDoublePlane* rotH, vector< vector<int> >* fPoints, int changeDimFactor) {
  SDoublePlane input_image = *inputIm;
  SDoublePlane rotMed = *p;
  SDoublePlane rotHigh = *rotH;
  
  int vertDist = scanForDim(&rotMed, 0, 0);
  for(int i = 0; i<5; i++) vertDist = scanForDim(&rotMed, vertDist, 1);
  vertDist = scanForDim(&rotMed, vertDist, 1);
  int yDim = vertDist + changeDimFactor;
  int xDim = (yDim)/2;
  
  if(yDim < 15 || xDim < 15) {
  	 yDim = input_image.cols()/23;
  	 xDim = yDim/2;
  	 if(input_image.rows() > input_image.cols()) {
  	 	yDim = input_image.rows()/23;
  	 	xDim = yDim/2;
  	 }
  }
  
  *fPoints = findStartPoints(&rotHigh);
  *fPoints = removeExtra(*fPoints, rotHigh.cols(), rotHigh.rows(), xDim, yDim, 1);
  
  int max = 0;
  SDoublePlane lMed = rotMed;
  for(int y = 0; y<lMed.rows(); y++) {
  	for(int x = 0; x<lMed.cols(); x++) {
  		if(lMed[y][x] != 200) lMed[y][x] = 0;
  		else lMed[y][x] = 1;
  	}
  }
  
  //fullSweep will actually perform the template matching
  //records will be a hash table where the location of a car is stored at the index of its template score
  vector<vector<vector<int> > > records (yDim*10,vector<vector<int> >(0,vector <int>(2,4)));
  fullSweep(&lMed, xDim, yDim, &max, &records);
  
  //iterates from top to bottom of the records table to pull out best template match locations
  vector<DetectedBox> cars2;
  for(int i = records.size()-1; i>=5*yDim/4; i--) {
  	for(int ii = 0; ii<records[i].size(); ii++) {
  		if(records[i][ii].size() > 1) {
  			DetectedBox s;
      		s.row = records[i][ii][1];
      		s.col = records[i][ii][0];
      		s.width = xDim;
      		s.height = yDim;
      		s.confidence = i;
      		cars2.push_back(s);
  		}
  	}
  }
  
  //does a final check to eliminate any potential cars placed too close together
  vector<DetectedBox> cars3;
  int index = 0;
  for(int i = 0; i < fPoints->size() + fPoints->size()/8 && index < cars2.size(); i++) {
  	int good = 1;
  	for(int ii = 0; ii<cars3.size(); ii++) {
  		if(abs(cars2[index].row - cars3[ii].row) < yDim && abs(cars2[index].col - cars3[ii].col) < xDim) {
  			good = 0;
  			break;
  		}
  	}
  	if(good == 1) cars3.push_back(cars2[index]);
  	else i--;
  	index++;
  }
  return cars3;
}