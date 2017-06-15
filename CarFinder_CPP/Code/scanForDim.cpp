#include "shrinkAndCompare.cpp"

using namespace std;

//scans edge map to estimate dimensions of cars in it
//given a rough estimate of that dimension so it can throw out found dimensions too far away from it
int scanForDim(SDoublePlane *h, int dimension, int shouldCheck) {
	vector< vector<int> > lineSegs;
	SDoublePlane pic = *h;
	int currStreak = 0, forgiveFactor = 2, streakFactor = 25, errStreak = 0, lineStart = -1, xStart = 0, oneWay = 1, otherWay = -1;
	int threshold = pic.cols()/12;
	if(pic.rows() > pic.cols()) threshold = pic.rows()/12;
	
	for(int xx = 0; xx<pic.cols(); xx++) {
		int x = xx;
		for(int y = 0; y<pic.rows(); y++) {
			if(pic[y][x] == 200) {
				currStreak++;
				errStreak = 0;
				if(currStreak > streakFactor && lineStart == -1) {
					lineStart = y-streakFactor;
					xStart = x;
				}
			}
			else if(x+oneWay >= 0 && x+oneWay < pic.cols() && pic[y][x+oneWay] == 200) {
				x += oneWay;
				currStreak++;
				errStreak = 0;
				if(currStreak > streakFactor && lineStart == -1) lineStart = y-streakFactor;
				oneWay = oneWay * -1;
				otherWay = otherWay * -1;
			}
			else if(x+otherWay >= 0 && x+otherWay < pic.cols() && pic[y][x+otherWay] == 200) {
				x += otherWay;
				currStreak++;
				errStreak = 0;
				if(currStreak > streakFactor && lineStart == -1) lineStart = y-streakFactor;
				oneWay = oneWay * -1;
				otherWay = otherWay * -1;
			}
			else {
				errStreak++;
				if(errStreak > forgiveFactor) {
					errStreak = 0;
					currStreak = 0;
					int dist = (y-forgiveFactor)-lineStart;
					if(lineStart != -1 && (shouldCheck == 0 || abs(dist - dimension) < dimension/3) && dist < threshold) {
						vector<int> line;
						line.push_back(xStart);
						line.push_back(lineStart);
						line.push_back(x);
						line.push_back(y-forgiveFactor);
						lineSegs.push_back(line);
					}
				}
			}
		}
		lineStart = -1;
		currStreak = 0;
		errStreak = 0;
	}
	if(lineSegs.size() == 0) return 0;
	if(lineSegs.size() <= 5) {
		int total = 0;
		int max = 0;
		for(int i = 0; i<lineSegs.size(); i++) {
			total += lineSegs[i][3] - lineSegs[i][1];
			if(lineSegs[i][3] - lineSegs[i][1] > max) max = lineSegs[i][3] - lineSegs[i][1];
		}
		int avg = total / lineSegs.size();
		return avg;
	}
	int total = 0;
	int max = 0, min = 10000, secMax = 0;
	for(int i = 0; i<lineSegs.size(); i++) {
		int dist = lineSegs[i][3] - lineSegs[i][1];
		if(dist > max) {
			total += max;
			max = dist;
			secMax = max;
		}
		else if(dist < min) {
			if(min != 10000) total += min;
			min = dist;
		}
		else total += dist;
	}
	int avg = total/lineSegs.size()-2;
	return avg;
	
}