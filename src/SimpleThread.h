#pragma once

#include "ofMain.h"
#include "ofxRuiThread.h"
#include "ofxOpenCv.h"
#include "jvcHaarFinder.h"
#include "ofxImage.h"

class SimpleThread:public ofxRuiThread{
public:
	int count;
	jvcHaarFinder finder;
	bool isReady;
	ofxCvGrayscaleImage grayImage;
	ofxCvColorImage colorImage;
	ofxCvFloatImage			diffFloatImage;
	ofImage backgroundFileImage;
	ofxCvGrayscaleImage backgroundImage;
	int videoWidth;
	int videoHeight;
	int threshold;
	int blur;
	int numBlobs;
	int minWidth;
	int minHeight;
	float color;
	string haarPath;
	unsigned char * videoPixels;
	SimpleThread(){
		numBlobs = 0;
		count = -1;
		isReady = false;
		videoWidth = 640;
		videoHeight = 480;
		blur = 0;
		threshold = 0;
		color=0xFF4455;
		haarPath="";
		minWidth = 20;
		minHeight = 20;
		allocateImages();
		isReady = true;
	}
	void setupFinder(string newHaarPath, int newVideoWidth=640, int newVideoHeight=480)
	{
		haarPath = newHaarPath;
		finder.setup(haarPath);
		if(videoWidth != newVideoWidth || videoHeight != newVideoHeight)
		{
			videoWidth = newVideoWidth;
			videoHeight = newVideoHeight;
			deleteImages();
			allocateImages();
		}

	}
	void deleteImages()
	{
		colorImage.clear();
		grayImage.clear();
		backgroundImage.clear();
		diffFloatImage.clear();
	}
	void allocateImages()
	{
		colorImage.allocate(videoWidth, videoHeight);
		grayImage.allocate(videoWidth, videoHeight);

		diffFloatImage.allocate(videoWidth, videoHeight);
		backgroundImage.allocate(videoWidth, videoHeight);
	}
	void updateThread(){
		isReady = false;
		colorImage.setFromPixels(videoPixels, videoWidth, videoHeight);
		grayImage = colorImage;
		grayImage.absDiff( backgroundImage );
		diffFloatImage	= grayImage;
		grayImage.blur( blur );
		grayImage.threshold( threshold );
		finder.findHaarObjects(grayImage, 0, 0, grayImage.width, grayImage.height, minWidth, minHeight);
		//faces.clear();
		numBlobs = finder.blobs.size();
		count++;	
		
		isReady = true;
	}
};
