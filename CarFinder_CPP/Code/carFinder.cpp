#include "convertCars.cpp"

using namespace std;

int main(int argc, char *argv[]) {
  //read input into image
  string input_filename(argv[1]);
  SDoublePlane input_image = SImageIO::read_png_file(input_filename.c_str());
  
  //load blur filter
  SDoublePlane mean_filter(3,3);
  for(int i=0; i<3; i++) {
  	for(int j=0; j<3; j++) mean_filter[i][j] = 1/9.0;
  }
  
  //guess at size of cars according to size of image
  //will be fine tuned later
  int yDim = input_image.cols()/10;
  int xDim = yDim/2;
  if(input_image.rows() > input_image.cols()) {
  	yDim = input_image.rows()/10;
  	xDim = yDim/2;
  }

  vector< vector<int> > firstSweep;
  vector< vector<int> > secondSweep;
  vector< vector<int> > thirdSweep;

  //blurs input image
  //runs edge detection with three contrast levels
  SDoublePlane blurred = convolve_general_shortcut(input_image, mean_filter);
  SDoublePlane high = sobel_gradient(blurred, 2000, 32, &firstSweep);
  SDoublePlane med = sobel_gradient(blurred, 32, 20, &secondSweep);
  SDoublePlane low = sobel_gradient(blurred, 20, 10, &thirdSweep);
  
  //blacks out areas of the low and medium edge maps where now high contrast values were found
  //assumption is that cars are shinier than most other objects in scene
  clearBlankAreas(&high, &med);
  clearBlankAreas(&high, &low);
  
  //estimates rough positions and number of cars from high contrast edge map
  vector< vector<int> > originalPoints = findStartPoints(&high);
  originalPoints = removeExtra(originalPoints, high.cols(), high.rows(), xDim, yDim, 4);
  killTrees(&high, originalPoints, yDim);
  
  //estimates rough positions and number of cars from medium contrast edge map
  vector< vector<int> > originalPoints2 = findStartPoints(&med);
  originalPoints2 = removeExtra(originalPoints2, med.cols(), med.rows(), xDim, yDim, 4);
  killTrees(&med, originalPoints2, yDim);
  
  //estimates rough positions and number of cars from low contrast edge map
  vector< vector<int> > originalPoints3 = findStartPoints(&low);
  originalPoints3 = removeExtra(originalPoints3, low.cols(), low.rows(), xDim, yDim, 4);
  killTrees(&low, originalPoints3, yDim);
  
  //standardizes values in edge maps
  for(int y = 0; y<high.rows(); y++) {
  	for(int x = 0; x<high.cols(); x++) {
  		if(high[y][x] == 90) high[y][x] = 200;
  		if(med[y][x] == 90) med[y][x] = 200;
  		if(low[y][x] == 90) low[y][x] = 200;
  	}
  }
  
  //rotates edge maps so cars point straight up
  float angle = 0; int code = 0;
  shrinkAndCompare(&med, &code, &angle);
  SDoublePlane rotMed = rotateImageWithCode(&med, angle, code);
  SDoublePlane rotHigh = rotateImageWithCode(&high, angle, code);
  SDoublePlane rotLow = rotateImageWithCode(&low, angle, code);
  
  
  //find average dimensions of cars
  int vertDist = scanForDim(&rotMed, 0, 0);
  for(int i = 0; i<5; i++) vertDist = scanForDim(&rotMed, vertDist, 1);
  vertDist = scanForDim(&rotMed, vertDist, 1);
  yDim = vertDist;
  xDim = (vertDist)/2;
  
  //repeats above processes but with better estimates on car orientation, number, and size
  vector< vector<int> > rPoints1 = findStartPoints(&rotHigh);
  rPoints1 = removeExtra(rPoints1, rotHigh.cols(), rotHigh.rows(), xDim, yDim, 4);
  killTrees(&rotHigh, rPoints1, yDim*1.5);
  
  vector< vector<int> > rPoints2 = findStartPoints(&rotMed);
  rPoints2 = removeExtra(rPoints2, rotMed.cols(), rotMed.rows(), xDim, yDim, 4);
  killTrees(&rotMed, rPoints2, yDim*1.5);
  
  vector< vector<int> > rPoints3 = findStartPoints(&rotLow);
  rPoints3 = removeExtra(rPoints3, rotLow.cols(), rotLow.rows(), xDim, yDim, 4);
  killTrees(&rotLow, rPoints3, yDim*1.5);
  
  for(int y = 0; y<rotHigh.rows(); y++) {
  	for(int x = 0; x<rotHigh.cols(); x++) {
  		if(rotHigh[y][x] == 90) rotHigh[y][x] = 200;
  		if(rotMed[y][x] == 90) rotMed[y][x] = 200;
  		if(rotLow[y][x] == 90) rotLow[y][x] = 200;
  	}
  }
  
  vector< vector<int> > fPoints = findStartPoints(&rotHigh);
  fPoints = removeExtra(fPoints, rotHigh.cols(), rotHigh.rows(), xDim, yDim, 1);
  
  int maxFoundSoFar = 0, sizeWhileFound = yDim, timesMissed = 0;
  
  //below processes will send info to final algorithm to get final locations of cars
  //this algorithm uses template matching
  //it will loop until at least 75% of estimated number of cars are found
  //the size of the template is increased/decreased with each iteration
  vector<DetectedBox> cars3 = finalGo(&rotMed, &input_image, &rotHigh, &fPoints, 0);
  int count = 2;
  while (cars3.size() < 3*fPoints.size()/4 && count < 30) {
  	if(yDim-count > 14)cars3 = finalGo(&rotMed, &input_image, &rotHigh, &fPoints, -1*count);
  	if(cars3.size() > maxFoundSoFar) {
  		maxFoundSoFar = cars3.size();
  		sizeWhileFound = -1*count;
  	}
  	if(yDim+count < 100 && cars3.size() < 3*fPoints.size()/4) cars3 = finalGo(&rotMed, &input_image, &rotHigh, &fPoints, count);
  	if(cars3.size() > maxFoundSoFar) {
  		maxFoundSoFar = cars3.size();
  		sizeWhileFound = count;
  	}
  	timesMissed++;
  	count += 2;
  }
  //same process is repeated if not enough cars have been found, but with low contrast edge map
  count = 2;
  while (cars3.size() < 3*fPoints.size()/4 && count < 30) {
  	if(yDim-count > 14)cars3 = finalGo(&rotLow, &input_image, &rotHigh, &fPoints, -1*count);
  	if(cars3.size() > maxFoundSoFar) {
  		maxFoundSoFar = cars3.size();
  		sizeWhileFound = -1*count;
  	}
  	if(yDim+count < 100 && cars3.size() < 3*fPoints.size()/4) cars3 = finalGo(&rotLow, &input_image, &rotHigh, &fPoints, count);
  	if(cars3.size() > maxFoundSoFar) {
  		maxFoundSoFar = cars3.size();
  		sizeWhileFound = 1*count;
  	}
  	timesMissed++;
  	count += 2;
  }
  //if still not enough cars have been found, final result will revert back to best try
  if(cars3.size() < fPoints.size()/2 || timesMissed > 4) {
  	cars3 = finalGo(&rotLow, &input_image, &rotHigh, &fPoints, sizeWhileFound);
  	for(int i = 0; i<fPoints.size(); i++) {
  		int good = 1;
  		for(int ii = 0; ii<cars3.size(); ii++) {
  			if(abs(cars3[ii].row - fPoints[i][1]) < yDim+sizeWhileFound && abs(cars3[ii].col - fPoints[i][0]) < xDim+sizeWhileFound) {
  				good = 0;
  				break;
  			}
  		}
  		if(good == 1) {
  			DetectedBox s;
      		s.row = fPoints[i][1];
      		s.col = fPoints[i][0];
      		s.width = xDim;
      		s.height = yDim;
      		if(cars3.size() > 0) {
      			s.width = cars3[i].width;
      			s.height = cars3[i].height;
      		}
      		s.confidence = 10;
      		cars3.push_back(s);
  		}
  	}
  }
  
  //write output image to bin folder
  vector<DetectedBox> fCars = reverseRotate(cars3, &med, &rotMed, angle, code, 1);
  write_detection_image("bin/foundCars.png", cars3, rotMed);
  
  
}
