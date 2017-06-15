#include "killTrees.cpp"

using namespace std;

//rotates an edge map to a particular angle
SDoublePlane rotateImage(SDoublePlane* p, float angle) {
	SDoublePlane pic = *p;
	
	int nH1 = 0*cos(angle) - pic.cols()*sin(angle);
	int nH2 = pic.rows()*cos(angle) - 0*sin(angle);
	int nH = abs(nH1) + abs(nH2);
	int nW = pic.cols()*cos(angle) + pic.rows()*sin(angle);
	int yOff = abs(nH1);
	
	SDoublePlane rPic(nH,nW);
	
	for(int w = 0; w<pic.cols(); w++) {
		for(int h = 0; h<pic.rows(); h++) {
			int nX = w*cos(angle) + h*sin(angle);
			int nY = h*cos(angle) - w*sin(angle);
			if(nX >= 0 && nX < nW && nY + yOff >= 0 && nY + yOff < nH) rPic[nY + yOff][nX] = pic[h][w];
		}
	}
	
	return rPic;
}

//flips image according to the code
//rotates until cars are thought to point straight up
SDoublePlane rotateImageWithCode(SDoublePlane* p, float angle, int code) {
	SDoublePlane pic = *p;
	if(code == 0) {
		return rotateImage(p, tan((float)(angle)*3.14159265/180));
	}
	else if(code == 1) {
		SDoublePlane pic90(pic.cols(),pic.rows());
  		for(int y = 0; y<pic.rows(); y++) {
  			for(int x = 0; x<pic.cols(); x++) {
  				pic90[x][y] = pic[y][x];
  			}
  		}
  		return rotateImage(&pic90, tan((float)(angle)*3.14159265/180));	
	}
	else if(code == 2) {
		SDoublePlane pic270(pic.cols(),pic.rows());
  		for(int y = 0; y<pic.rows(); y++) {
  			for(int x = 0; x<pic.cols(); x++) {
  				pic270[pic.cols() -1 - x][y] = pic[y][x];
  			}
  		}
  		return rotateImage(&pic270, tan((float)(angle)*3.14159265/180));
	}
	else {
		SDoublePlane pic180(pic.rows(),pic.cols());
  		for(int y = 0; y<pic.rows(); y++) {
  			for(int x = 0; x<pic.cols(); x++) {
  				pic180[y][pic.cols() -1 - x] = pic[y][x];
  			}
  		}
  		return rotateImage(&pic180, tan((float)(angle)*3.14159265/180));
	}	
}

//unflips and unrotates an edge map
//converts positions of cars to original orientation
vector<DetectedBox> reverseRotate(vector<DetectedBox> cars, SDoublePlane* p1, SDoublePlane* p2, float angle, int code, int shouldFlip) {
	vector<DetectedBox> newCars;
	SDoublePlane pic = *p1;
	SDoublePlane rPic = *p2;
	
	int cols = pic.cols();
	int rows = pic.rows();
	if(code == 1 || code == 2) {
		cols = pic.rows();
		rows = pic.cols();
	}
	
	angle = tan((float)(angle)*3.14159265/180);
	int nH1 = 0*cos(angle) - cols*sin(angle);
	int nH2 = rows*cos(angle) - 0*sin(angle);
	int nH = abs(nH1) + abs(nH2);
	int nW = cols*cos(angle) + rows*sin(angle);
	int yOff = abs(nH1);
	
	int xDeltaOrigin = 0;
	int yDeltaOrigin = 0;
	
	for(int w = 0; w<cols; w++) {
		for(int h = 0; h<rows; h++) {
			int nX = w*cos(angle) + h*sin(angle);
			int nY = h*cos(angle) - w*sin(angle) + yOff;
			for(int i = 0; i<cars.size(); i++) {
				if(abs(nX - cars[i].col) < 2 && abs(nY - cars[i].row) < 2 && cars[i].confidence != -1) {
					int x = w;
					int y = h;
					int width = cars[i].width;
					int height = cars[i].height;
					if(code == 1 && shouldFlip == 1) {
						int temp = x;
						x = y;
						y = temp;
						width = cars[i].height;
						height = cars[i].width;
					}
					else if(code == 2 && shouldFlip == 1) {
						int temp = x;
						x = pic.cols() - 1 - y;
						y = temp;
						width = cars[i].height;
						height = cars[i].width;
						xDeltaOrigin = width;
					}
					else if(code == 3 && shouldFlip == 1) {
						x = pic.cols() -1 - x;
						xDeltaOrigin = width;
					}
					DetectedBox s;
    				s.row = y - yDeltaOrigin;
    				s.col = x - xDeltaOrigin;
    				s.width = width;
    				s.height = height;
    				s.confidence = cars[i].confidence;
    				newCars.push_back(s);
    				cars[i].confidence = -1;
    				break;
				}
			}
		}
	}
	return newCars;
}

//computes a score of vertical line strength in an edge map
int calcVertScore(SDoublePlane* p) {
	SDoublePlane pic = *p;
	int score = 0;
	int forgiveFactor = 2;
	int hitFactor = 2;
	
	for(int w = 0; w<pic.cols(); w++) {
		int currStreak = 0;
		int numErr = 0;
		for(int h = 0; h<pic.rows(); h++) {
			if(pic[h][w] == 200) {
				currStreak++;
				numErr = 0;
				if(currStreak > hitFactor) score+=currStreak;
			}
			else {
				numErr++;
				if(numErr > forgiveFactor) currStreak = 0;
			}
		}
	}
	return score;
}