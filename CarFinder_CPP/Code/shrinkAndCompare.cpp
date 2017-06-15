#include "rotateImage.cpp"

using namespace std;

//shrinks image for efficient rotating
SDoublePlane shrinkPic(SDoublePlane* p, int diffuser) {
	SDoublePlane pic = *p;
	SDoublePlane nPic(pic.rows()/diffuser, pic.cols()/diffuser);
	for(int y = 0; y<pic.rows()/diffuser; y++) {
		int yy = y * diffuser;
		for(int x = 0; x<pic.cols()/diffuser; x++) {
			nPic[y][x] = pic[yy][x*diffuser];
		}
	}
	return nPic;
}

//rotates image to maximum vertical line score
vector<int> rotateToTop(SDoublePlane* p) {
	SDoublePlane med = *p;
	vector<int> stats;
	
	vector<int> degVals;
  	degVals.push_back(0);
  	SDoublePlane r;
  	for(int i = 1; i<=45; i+=1) {
  		r =rotateImage(&med, tan((float)(i)*3.14159265/180));
  		degVals.push_back(calcVertScore(&r));
  	}
  	int maxScore = 0;
  	int maxIndex = 0;
  	for(int i = 0; i<degVals.size(); i++) {
  		if(degVals[i] > maxScore) {
  			maxIndex = i;
  			maxScore = degVals[i];
  		}
  	}
  	stats.push_back(maxIndex);
  	stats.push_back(maxScore);
  	return stats;
}

//compares scores of rotated images at each flip possibility
void shrinkAndCompare(SDoublePlane* p, int* rotateCode, float* rAngle) {
  SDoublePlane med = *p;
  int bigger = med.rows();
  if(med.cols() > med.rows()) bigger = med.cols();
  int diffuser = 1;
  if(bigger > 300) diffuser = bigger/300;
  
  SDoublePlane shrunk = shrinkPic(&med, diffuser);
  SDoublePlane shrunk90(shrunk.cols(),shrunk.rows());
  for(int y = 0; y<shrunk.rows(); y++) {
  	for(int x = 0; x<shrunk.cols(); x++) {
  		shrunk90[x][y] = shrunk[y][x];
  	}
  }	
  SDoublePlane shrunk270(shrunk.cols(),shrunk.rows());
  for(int y = 0; y<shrunk.rows(); y++) {
  	for(int x = 0; x<shrunk.cols(); x++) {
  		shrunk270[shrunk.cols() -1 - x][y] = shrunk[y][x];
  	}
  }
  SDoublePlane shrunk180(shrunk.rows(),shrunk.cols());
  for(int y = 0; y<shrunk.rows(); y++) {
  	for(int x = 0; x<shrunk.cols(); x++) {
  		shrunk180[y][shrunk.cols() -1 - x] = shrunk[y][x];
  	}
  }	
  
  vector<int> s1 = rotateToTop(&shrunk);
  vector<int> s2 = rotateToTop(&shrunk90);
  vector<int> s3 = rotateToTop(&shrunk270);
  vector<int> s4 = rotateToTop(&shrunk180);
  
  int max = s1[1];
  int code = 0;
  int angle = s1[0];
  if(s2[1] > max) {
  	code = 1;
  	angle = s2[0];
  	max = s2[1];
  }
  if(s3[1] > max) {
  	code = 2;
  	angle = s3[0];
  	max = s3[1];
  }
  if(s4[1] > max) {
  	code = 3;
  	angle = s4[0];
  	max = s4[1];
  }
  
  *rotateCode = code;
  *rAngle = (float)(angle);
} 