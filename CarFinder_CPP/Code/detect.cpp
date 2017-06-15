//
// detect.cpp : Detect cars in satellite images.
//
// Based on skeleton code by D. Crandall, Spring 2017
//
// PUT YOUR NAMES HERE
//
//

#include "SImage.h"
#include "SImageIO.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane will be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below is a helper functions that overlays rectangles
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
  for(int w=-width/2; w<=width/2; w++) {
    int top = _top+w, left = _left+w, right=_right+w, bottom=_bottom+w;

    // if any of the coordinates are out-of-bounds, truncate them 
    top = min( max( top, 0 ), input.rows()-1);
    bottom = min( max( bottom, 0 ), input.rows()-1);
    left = min( max( left, 0 ), input.cols()-1);
    right = min( max( right, 0 ), input.cols()-1);
      
    // draw top and bottom lines
    for(int j=left; j<=right; j++)
	  input[top][j] = input[bottom][j] = graylevel;
    // draw left and right lines
    for(int i=top; i<=bottom; i++)
	  input[i][left] = input[i][right] = graylevel;
  }
}

// DetectedBox class may be helpful!
//  Feel free to modify.
//
class DetectedBox {
public:
  int row, col, width, height;
  double confidence;
};

// Function that outputs the ascii detection output file
void  write_detection_txt(const string &filename, const vector<DetectedBox> &cars)
{
  ofstream ofs(filename.c_str());

  for(vector<DetectedBox>::const_iterator s=cars.begin(); s != cars.end(); ++s)
    ofs << s->row << " " << s->col << " " << s->width << " " << s->height << " " << s->confidence << endl;
}

// Function that outputs a visualization of detected boxes
void  write_detection_image(const string &filename, const vector<DetectedBox> &cars, const SDoublePlane &input)
{
  SDoublePlane output_planes[3];

  for(int p=0; p<3; p++)
    {
      output_planes[p] = input;
      for(vector<DetectedBox>::const_iterator s=cars.begin(); s != cars.end(); ++s)
	overlay_rectangle(output_planes[p], s->row, s->col, s->row+s->height-1, s->col+s->width-1, p==2?255:0, 2);
    }

  SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);
}


// Convolve an image with a  convolution kernel
//
SDoublePlane convolve_general(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output(input.rows(), input.cols());
  
  int v_border_to_handle = (filter.rows() - 1) / 2;
  int h_border_to_handle = (filter.cols() - 1) / 2;
  SDoublePlane padded_input = SDoublePlane(input.rows() + 2 * v_border_to_handle, input.cols() + 2 * h_border_to_handle);
  
  for (int i = 0; i < input.rows(); i++){
      for (int j = 0; j < input.cols(); j++){
          padded_input[i + v_border_to_handle][j + h_border_to_handle] = input[i][j];
      }
  }
  
  for (int i = 0; i < v_border_to_handle; i++){
      for (int j = 0; j < h_border_to_handle; j++){
          padded_input[i][j] = padded_input[2 * v_border_to_handle - i - 1][2 * h_border_to_handle - j - 1];
          padded_input[padded_input.rows() - i - 1][padded_input.cols() - j - 1] = padded_input[padded_input.rows() + i - 1 - 2 * v_border_to_handle][padded_input.cols() + j - 1 - 2 * h_border_to_handle];
          padded_input[i][padded_input.cols() - j - 1] = padded_input[2 * v_border_to_handle - i - 1][padded_input.cols() + j - 1 - 2 * h_border_to_handle];
          padded_input[padded_input.rows() - i - 1][j] = padded_input[padded_input.rows() + i - 1 - 2 * v_border_to_handle][2 * h_border_to_handle - j - 1];
      }
  }
  
  for (int i = 0; i < v_border_to_handle; i++){
      for (int j = h_border_to_handle; j < h_border_to_handle + input.cols() - 1; j++){
          padded_input[i][j] = padded_input[2 * v_border_to_handle - i - 1][j];
          padded_input[padded_input.rows() - i - 1][j] = padded_input[padded_input.rows() + i - 1 - 2 * v_border_to_handle][j];
      }
  }
  
  for (int j = 0; j < h_border_to_handle; j++){
      for (int i = v_border_to_handle; i < v_border_to_handle + input.rows() - 1; i++){
          padded_input[i][j] = padded_input[i][2 * h_border_to_handle - j - 1];
          padded_input[i][padded_input.cols() - j - 1] = padded_input[i][padded_input.cols() + j - 1 - 2 * h_border_to_handle];
      }
  }
  
  // Convolution code here
  for (int i = 0; i < output.rows(); i++){
      for (int j = 0; j < output.cols(); j++){
          for (int m = 0; m < filter.rows(); m++){
              for (int n = 0; n < filter.cols(); n++){
                  output[i][j] += padded_input[i + m][j + n] * filter[m][n];
              }
          }
      }
  }
  
  
  return output;
}

// Convolve an image with a separable convolution kernel
//
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane temp = convolve_general(input, row_filter);
  SDoublePlane output = convolve_general(temp, col_filter);
  return output;
}

// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool direction_is_v)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane row_filter = SDoublePlane(3, 1);
  SDoublePlane col_filter = SDoublePlane(1, 3);

  // Implement a sobel gradient estimation filter with 1-d filters
  if (direction_is_v){
      row_filter[0][0] = 1;
      row_filter[1][0] = 0;
      row_filter[2][0] = -1;
      col_filter[0][0] = 1;
      col_filter[0][1] = 2;
      col_filter[0][2] = 1;
  }
  else {
      row_filter[0][0] = 1;
      row_filter[1][0] = 2;
      row_filter[2][0] = 1;
      col_filter[0][0] = 1;
      col_filter[0][1] = 0;
      col_filter[0][2] = -1;
  }
  
  output = convolve_separable(input, row_filter, col_filter);
  return output;
}

SDoublePlane gradient_magnitude(const SDoublePlane &v_gradient, const SDoublePlane &h_gradient)
{
  SDoublePlane output(v_gradient.rows(), v_gradient.cols());
  
  for (int i = 0; i < v_gradient.rows(); i++){
      for (int j = 0; j < v_gradient.cols(); j++){
          output[i][j] = sqrt(pow(v_gradient[i][j], 2) + pow(h_gradient[i][j], 2));
      }
  }
  
  return output;
}

// Apply an edge detector to an image, returns the binary edge map
// 
SDoublePlane find_edges(const SDoublePlane &input, double thresh)
{
  SDoublePlane output(input.rows(), input.cols());

  // Implement an edge detector of your choice, e.g.
  // use your sobel gradient operator to compute the gradient magnitude and threshold
  for (int i = 0; i < input.rows(); i++){
      for (int j = 0; j < input.cols(); j++){
          if (input[i][j] >= thresh){
              output[i][j] = 200;
          }
          else{
              output[i][j] = 10;
          }
      }
  }
  
  return output;
}




// Convolve an image with a separable convolution kernel with multiplication
//
SDoublePlane convolve_separable(const SDoublePlane &input, const vector<double> &row_filter, const vector<double> &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane output2(input.rows(), input.cols());

  int rL = row_filter.size();
  int cL = col_filter.size();
  int w = input.cols();
  int h = input.rows();
  double mFactor = row_filter[0];
  
  for(int y = 0; y<h; y++) {
  	int total = 0;
  	vector<double> values;
  	int pointer = 0;
  	for(int xx = 0; xx<rL; xx++) {
  		total += input[y][xx];
  		values.push_back(input[y][xx]);
  	}
  	output[y][rL/2] = total/rL;
  	for(int x = 1; x<w-rL; x++) {
  		total = total - values[pointer] + input[y][x+rL-1];
  		values[pointer] = input[y][x+rL-1];
  		pointer++;
  		if(pointer >= values.size()) pointer = 0;
  		output[y][x+rL/2] = abs(mFactor*total/rL);
  	}
  }
  for(int x = 0; x<w; x++) {
  	int total = 0;
  	vector<double> values;
  	int pointer = 0;
  	for(int yy = 0; yy<cL; yy++) {
  		total += input[yy][x];
  		values.push_back(output[yy][x]);
  	}
  	output2[cL/2][x] = total/cL;
  	for(int y = 1; y<h-cL; y++) {
  		total = total - values[pointer] + output[y+cL-1][x];
  		values[pointer] = output[y+cL-1][x];
  		pointer++;
  		if(pointer >= values.size()) pointer = 0;
  		output2[y+cL/2][x] = abs(mFactor*total/rL);
  	}
  }
  return output2;
}


// Convolve an image with a separable convolution kernel (no multiplication)
//
SDoublePlane convolve_separable_shortcut(const SDoublePlane &input, const vector<double> &row_filter, const vector<double> &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane output2(input.rows(), input.cols());

  int rL = row_filter.size();
  int cL = col_filter.size();
  int w = input.cols();
  int h = input.rows();
  
  for(int y = 0; y<h; y++) {
  	int total = 0;
  	vector<double> values;
  	int pointer = 0;
  	for(int xx = 0; xx<rL; xx++) {
  		total += input[y][xx];
  		values.push_back(input[y][xx]);
  	}
  	output[y][rL/2] = total/rL;
  	for(int x = 1; x<w-rL; x++) {
  		total = total - values[pointer] + input[y][x+rL-1];
  		values[pointer] = input[y][x+rL-1];
  		pointer++;
  		if(pointer >= values.size()) pointer = 0;
  		output[y][x+rL/2] = abs(total/rL);
  	}
  }
  for(int x = 0; x<w; x++) {
  	int total = 0;
  	vector<double> values;
  	int pointer = 0;
  	for(int yy = 0; yy<cL; yy++) {
  		total += input[yy][x];
  		values.push_back(output[yy][x]);
  	}
  	output2[cL/2][x] = total/cL;
  	for(int y = 1; y<h-cL; y++) {
  		total = total - values[pointer] + output[y+cL-1][x];
  		values[pointer] = output[y+cL-1][x];
  		pointer++;
  		if(pointer >= values.size()) pointer = 0;
  		output2[y+cL/2][x] = abs(total/rL);
  	}
  }
  return output2;
}


// Convolve an image with a uniform convolution kernel (with multiplication)
//
SDoublePlane convolve_general_shortcut(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output = input;

  int fW = filter.cols();
  int fH = filter.rows();
  int w = input.cols();
  int h = input.rows();
  double mFactor = filter[0][0];
  
  for(int i = 0; i<h-fH; i++) {
    double total = 0;
  	int colPointer = 0;
  	vector<double> colTotals;
  	for(int ii = 0; ii<fW; ii++) {
  		int cTot = 0;
  		for(int jj = i; jj<fH; jj++) {
  			cTot += input[ii][jj]*mFactor;
  		}
  		colTotals.push_back(cTot);
  		total += cTot;
  	}
  	output[fH/2][fW/2] = total / (fW*fH);
  	for(int j = 1; j<w-fW; j++) {
  		int cTot = 0;
  		for(int jj = i; jj<fH+i; jj++) {
  			cTot += input[jj][j]*mFactor;
  		}
  		total = total - colTotals[colPointer] + cTot;
  		colTotals[colPointer] = cTot;
  		colPointer++;
  		if(colPointer >= colTotals.size()) colPointer = 0;
  		output[i+fW/2][j+fH/2] = total / (fW*fH);
  	}
  }
  
  return output;
}


// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient(const SDoublePlane &input, int upperThreshold, int lowerThreshold, vector< vector<int> >* p)
{
  SDoublePlane output(input.rows(), input.cols());
  
  int filter = 5;
  int diffuser = 0;
  int edgeBuf = 8;

  int rL = filter;
  int cL = filter;
  int w = input.cols();
  int h = input.rows();
  
  for(int y = 0; y<h; y++) {
  	vector<double> values;
  	int pointer = 0;
  	for(int xx = 0; xx<rL; xx++) {
  		values.push_back(input[y][xx]);
  	}
  	output[y][0] = 10;
  	output[y][1] = 10;
  	output[y][2] = 10;
  	for(int x = 1; x<w-rL; x++) {
  		int tX = x;
  		double val = abs(values[pointer%5] + values[(pointer+1)%5] - values[(pointer+3)%5] - values[(pointer+4)%5]);
  		if(val > lowerThreshold && val < upperThreshold && x > edgeBuf && x < w-edgeBuf && y > edgeBuf && y < h - edgeBuf) {
  			val = 200;
  			x+=diffuser;
  			vector<int> r;
  			r.push_back(tX+rL/2);
  			r.push_back(y);
  			p->push_back(r);  		
  		}
  		else val = 10;
  		output[y][tX+rL/2] = val;
  		values[pointer] = input[y][tX+rL];
  		pointer++;
  		if(pointer > values.size()) pointer = 0;
  	}
  }
  for(int x = 0; x<w; x++) {
  	vector<double> values;
  	int pointer = 0;
  	for(int yy = 0; yy<cL; yy++) {
  		values.push_back(input[yy][x]);
  	}
  	for(int y = 1; y<h-cL; y++) {
  		int tY = y;
  		double val = abs(values[pointer%5] + values[(pointer+1)%5] - values[(pointer+3)%5] - values[(pointer+4)%5]);
  		if(val > lowerThreshold && val < upperThreshold && output[tY+cL/2][x] != 200 && x > edgeBuf && x < w-edgeBuf && y > edgeBuf && y < h - edgeBuf) {
  			vector<int> r;
  			r.push_back(x);
  			r.push_back(tY+cL/2);
  			p->push_back(r);
  			output[tY+cL/2][x] = 200;
  			y+=diffuser;
  		}
  		values[pointer] = input[tY+cL][x];
  		pointer++;
  		if(pointer > values.size()) pointer = 0;
  	}
  }
  return output;
}



vector< vector<int> > adjustStartPoints(int biggestDist, vector< vector<int> > *fS, SDoublePlane copy, SDoublePlane *h) {
	vector< vector<int> > firstSweep = *fS;
	SDoublePlane high = *h;
	vector< vector<int> > adjustedPoints;
	for(int i = 0; i<firstSweep.size(); i++) {
  		vector<int> p;
  		p.push_back(firstSweep[i][0]);
  		p.push_back(firstSweep[i][1]);
  		vector<int> topRight = p;
  		vector<int> topLeft = p;
  		vector<int> bottomRight = p;
  		vector<int> bottomLeft = p;
  		int keepGoing = 1;
  		int numForgiven = 0;
  		int forgiveFactor = 5;
  		
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
  		keepGoing = 1;
  		numForgiven = 0;
  		while(keepGoing == 1) {
  			if(bottomLeft[0]-1 >= 0 && high[bottomLeft[1]][bottomLeft[0]+1] == 200) {
  				bottomLeft[0] = bottomLeft[0]-1;
  				copy[bottomLeft[1]][bottomLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomLeft[1]-1 >= 0 && high[bottomLeft[1]-1][bottomLeft[0]] == 200) {
  				bottomLeft[1] = bottomLeft[1]-1;
  				copy[bottomLeft[1]][bottomLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomLeft[0]-1 >= 0 && bottomLeft[1]-1 >= 0 && high[bottomLeft[1]-1][bottomLeft[0]+1] == 200) {
  				bottomLeft[0] = bottomLeft[0]-1;
  				bottomLeft[1] = bottomLeft[1]-1;
  				copy[bottomLeft[1]][bottomLeft[0]] = 0;
  				numForgiven = 0;
  			}
  			else if(bottomLeft[0]-1 >= 0) {
  				numForgiven++;
  				if(numForgiven > forgiveFactor) keepGoing = 0;
  				bottomLeft[0] = bottomLeft[0]-1;
  			}
  			else keepGoing = 0;
  		}
  		
  		int d1 = sqrt(pow(abs(bottomLeft[0] - topLeft[0]),2) + pow(abs(bottomLeft[1] - topLeft[1]),2));
  		int d2 = sqrt(pow(abs(bottomLeft[0] - bottomRight[0]),2) + pow(abs(bottomLeft[1] - bottomRight[1]),2));
  		int d3 = sqrt(pow(abs(topLeft[0] - topRight[0]),2) + pow(abs(topLeft[1] - topRight[1]),2));
  		int d4 = sqrt(pow(abs(topRight[0] - bottomRight[0]),2) + pow(abs(topRight[1] - bottomRight[1]),2));
  		
  		int position = 0;
  		//1 is bottomLeft and topLeft are good anchors, and car is vertical
  		//2 is bottomLeft and topLeft are good anchors, and car is horizontal
  		//3 is bottomLeft and bottomRight are good anchors, and car is vertical
  		//4 is bottomLeft and bottomRight are good anchors, and car is horizontal
  		//5 is bottomRight and topRight are good anchors, and car is vertical
  		//6 is bottomRight and topRight are good anchors, and car is horizontal
  		//7 is topLeft and topRight are good anchors, and car is vertical
  		//8 is topLeft and topRight are good anchors, and car is horizontal
  		//0 is idk!
  		int errorFactor = 10;
  		if(abs(d1-biggestDist) < errorFactor) position = 1;
  		else if(abs(d1-biggestDist/2) < errorFactor) position = 2;
  		else if(abs(d2-biggestDist/2) < errorFactor) position = 3;
  		else if(abs(d2-biggestDist) < errorFactor) position = 4;
  		else if(abs(d3-biggestDist) < errorFactor) position = 5;
  		else if(abs(d3-biggestDist/2) < errorFactor) position = 6;
  		else if(abs(d4-biggestDist/2) < errorFactor) position = 7;
  		else if(abs(d4-biggestDist) < errorFactor) position = 8;
  		
  		vector<int> c;
  		switch (position) {
  			case 1:
  				c.push_back(bottomLeft[0]);
  				c.push_back(bottomLeft[1]);
  				c.push_back(d1/2);
  				c.push_back(d1);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 2:
  				c.push_back(bottomLeft[0]);
  				c.push_back(bottomLeft[1]);
  				c.push_back(d1);
  				c.push_back(d1*2);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 3:
  				c.push_back(bottomLeft[0]);
  				c.push_back(bottomLeft[1]);
  				c.push_back(d2);
  				c.push_back(d2*2);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 4:
  				c.push_back(bottomLeft[0]);
  				c.push_back(bottomLeft[1]);
  				c.push_back(d2);
  				c.push_back(d2/2);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 5:
  				c.push_back(bottomRight[0]-d3/2);
  				c.push_back(bottomRight[1]);
  				c.push_back(d3/2);
  				c.push_back(d3);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 6:
  				c.push_back(bottomRight[0]-d3);
  				c.push_back(bottomRight[1]);
  				c.push_back(d3);
  				c.push_back(d3/2);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 7:
  				c.push_back(topLeft[0]);
  				c.push_back(topLeft[1]-d4*2);
  				c.push_back(d4);
  				c.push_back(d4*2);
  				adjustedPoints.push_back(c);
  			break;
  			
  			case 8:
  				c.push_back(topLeft[0]);
  				c.push_back(topLeft[1]-d4);
  				c.push_back(d4/2);
  				c.push_back(d4);
  				adjustedPoints.push_back(c);
  			break;
  			
  			default:
  			break;
  		} 	
  }
  return adjustedPoints;
}




