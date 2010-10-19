/*
 *  ofxImage
 *
 *  Provides extended functionality to the core ofImage class.
 *  
 *  Created by Pat Long (plong0) on 08/06/10.
 *  Copyright 2010 Tangible Interaction Inc. All rights reserved.
 *
 */
#ifndef _OFX_IMAGE
#define _OFX_IMAGE

#include "ofMain.h"
#include "ofxCvMain.h"

class ofxImage : public ofImage{
protected:
	string fileName;
	
	void flipPixels(ofPixels &pix, bool horizontal, bool vertical);
	void rotatePixels(ofPixels &pix, float angle);
	bool saveImageFromPixels(string fileName, ofPixels &pix);
	
	
public:
	ofxImage();
	
	void 				clone(ofxImage &mom);
	
	void				clearPixels(int x, int y, int w, int h, int r=0, int g=0, int b=0, int a=0);
	unsigned char * 	getPixels();			// up to you to get this right
	unsigned char * 	getPixels(unsigned char* cropPixels, int x, int y, int w, int h);			// up to you to get this right
	
	string getFileName();
	bool loadImage(string fileName="");
	bool saveImage(string fileName="");
	void setFileName(string fileName);
	
	void mirror(bool horizontal, bool vertical);
	void rotate(float angle);
	void superFastBlur(int radius);
	void superFastBlurAlpha(int radius);
	ofxCvBlob blob;
	string	haarType;
};

#endif
