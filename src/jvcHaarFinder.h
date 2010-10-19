#include "ofxCvHaarFinder.h"

class jvcHaarFinder:public ofxCvHaarFinder {
	public:
	//same as ofxCvHaarFinder but disables textures for multithreading
	int findHaarObjects(ofxCvGrayscaleImage&, int x, int y, int w, int h,
						int minWidth = HAAR_DEFAULT_MIN_SIZE, int minHeight = HAAR_DEFAULT_MIN_SIZE);

};