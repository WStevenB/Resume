#include "finalGo.cpp"

using namespace std;

//this function will convert cars from a rotated edge map back to the original orientation
vector<DetectedBox> convertCars(SDoublePlane* p1, SDoublePlane* p2, vector<DetectedBox> cars, float angle, int code) {
    angle = tan((float)(angle)*3.14159265/180);
    vector<DetectedBox> newCars;
    SDoublePlane pic = *p1;
    SDoublePlane pic2 = *p2;
    int height = cars[0].height;
    int width = cars[0].width;
    if(code == 1 || code == 2) {
    	height = cars[0].width;
    	width = cars[0].height;
    }
	
	int nH1 = 0*cos(angle) - pic2.cols()*sin(angle);
	int nH2 = pic2.rows()*cos(angle) - 0*sin(angle);
	int nH = abs(nH1) + abs(nH2);
	int nW = pic2.cols()*cos(angle) + pic2.rows()*sin(angle);
	int yOff = abs(nH1);
  
  for(int i = 0; i<cars.size(); i++) {
  	int y = cars[i].row - yOff;
  	int x = cars[i].col;
  	int nX = (x/cos(angle)) - (y*sin(angle));
  	nX = nX/(1-sin(angle)*sin(angle)); 
	int nY = (y+nX*sin(angle))/cos(angle);
	if(code == 1) {
		int temp = nX;
		nX = nY;
		nY = temp;
	}
	else if(code == 2) {
		int temp = nX;
		nX = pic.cols() -1 - nY;
		nY = temp;
	}
	else if(code == 3) {
		nX = pic.cols() -1 - nX;
	}
	DetectedBox s;
    s.row = nY;
    s.col = nX;
    s.width = width;
    s.height = height;
    s.confidence = cars[i].confidence;
    newCars.push_back(s);
  }
  
  return newCars;
}