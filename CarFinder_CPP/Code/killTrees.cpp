#include "explore.cpp"

using namespace std;

//this algorithm attempts to remove trees and buildings from the edge map
//calls explore to estimate the dimensions of a blob in the edge map
//if its way too big to be a car, it will be blacked out
void killTrees(SDoublePlane* p, vector< vector<int> > originalPoints, int dimension) {
	int height = p->rows();
	int width = p->cols();
	int largestDim = height;
	if(width > largestDim) largestDim = width;
	for(int i = 0; i<originalPoints.size(); i++) {
  		vector< vector<int> > records;
  		explore(p, 0, 5000, originalPoints[i][0], originalPoints[i][1], &records, 0);
  		vector<int> topLeft = records[0], bottomLeft = records[0], bottomRight = records[0], topRight = records[0];
  		int furthestLeft = 10000, furthestRight = 0, furthestUp = 10000, furthestDown = 0;
  		for(int ii = 0; ii<records.size(); ii++) {
  			if(records[ii][0]<furthestLeft) furthestLeft = records[ii][0];
  			if(records[ii][1]<furthestUp) furthestUp = records[ii][1];
  			if(records[ii][0]>furthestRight) furthestRight = records[ii][0];
  			if(records[ii][1]>furthestDown) furthestDown = records[ii][1];
  			if(topLeft[0] - records[ii][0] > 2*abs(records[ii][1]-topLeft[1]) || topLeft[1] - records[ii][1] > 2*abs(records[ii][0]-topLeft[0])) topLeft = records[ii];
  			else if(records[ii][0] - topRight[0] > 2*abs(records[ii][1]-topRight[1]) || topRight[1] - records[ii][1] > 2*abs(records[ii][0]-topRight[0])) topRight = records[ii];
  			else if(records[ii][0] - bottomRight[0] > 2*abs(records[ii][1]-bottomRight[1]) || records[ii][1] - bottomRight[1] > 2*abs(records[ii][0]-bottomRight[0])) bottomRight = records[ii];
  			else if(bottomLeft[0] - records[ii][0] > 2*abs(records[ii][1]-bottomLeft[1]) || records[ii][1] - bottomLeft[1] > 2*abs(records[ii][0]-bottomLeft[0])) bottomLeft = records[ii];
  		}
  		int ratio = 2;
  		int hDim = furthestRight - furthestLeft;
  		int vDim = furthestDown - furthestUp;
  		if(vDim > hDim && hDim != 0) ratio = vDim/hDim;
  		else if(hDim > vDim && vDim != 0) ratio = hDim/vDim;
  		if(hDim > dimension || vDim > dimension || (ratio > 5 && (hDim > dimension || vDim > dimension))) {
  			for(int iii = 0; iii<records.size(); iii++) {
  				(*p)[records[iii][1]][records[iii][0]] = 10;
  				if(records[iii][1] - 1 >= 0) (*p)[records[iii][1]-1][records[iii][0]] = 10;
  				if(records[iii][0] - 1 >= 0) (*p)[records[iii][1]][records[iii][0]-1] = 10;
  				if(records[iii][1] + 1 < height) (*p)[records[iii][1]+1][records[iii][0]] = 10;
  				if(records[iii][0] + 1 < width) (*p)[records[iii][1]][records[iii][0]+1] = 10;
  			}
  		}
  	}
}

