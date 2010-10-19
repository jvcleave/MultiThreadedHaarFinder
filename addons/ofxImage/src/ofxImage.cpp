/*
 *  ofxImage.cpp
 *
 *  Created by Pat Long (plong0) on 08/06/10.
 *  Copyright 2010 Tangible Interaction Inc. All rights reserved.
 *
 */
#include "ofxImage.h"

//------------------------------------
ofxImage::ofxImage():ofImage(){
	fileName = "";
	haarType = "";
}

//------------------------------------
void ofxImage::clone(ofxImage &mom){
	fileName = mom.getFileName();
	ofImage::clone(mom);
}

//------------------------------------
void ofxImage::clearPixels(int x, int y, int w, int h, int r, int g, int b, int a){
	unsigned char* pixels = this->getPixels();
	int Bpp = this->bpp/8;
	for(int i=x; i < x+w && i < this->width; i++){
		for(int j=y; j < y+h && j < this->height; j++){
			pixels[(j*this->width+i)*Bpp+0] = r;
			if(Bpp > 1){
				pixels[(j*this->width+i)*Bpp+1] = g;
				if(Bpp > 2){
					pixels[(j*this->width+i)*Bpp+2] = b;
					if(Bpp > 3){
						pixels[(j*this->width+i)*Bpp+3] = a;
					}
				}
			}
		}
	}
	this->setFromPixels(pixels, this->width, this->height, this->type);
}

//------------------------------------
unsigned char * ofxImage::getPixels(){
	return ofImage::getPixels();
}

//------------------------------------
unsigned char * ofxImage::getPixels(unsigned char* cropPixels, int x, int y, int w, int h){
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	if(cropPixels == NULL)
		cropPixels = new unsigned char[w*h*this->bpp/8];
	unsigned char* pixels = this->getPixels();
	int Bpp = this->bpp / 8;
	for (int i = x; i < (x+w) && i < this->width; i++){
		for (int j = y; j < (y+h) && j < this->height; j++){
			for(int b = 0; b < Bpp; b++)
				cropPixels[((j-y)*w+(i-x))*Bpp+b] = pixels[(j*this->width+i)*Bpp+b];
		}
	}
	return cropPixels;
}

//------------------------------------
string ofxImage::getFileName(){
	return fileName;
}

//------------------------------------
bool ofxImage::loadImage(string fileName){
	string localFileName = getFileName();
	if(fileName == "" && localFileName != "")
		fileName = localFileName;
	bool result = ofImage::loadImage(fileName);
	if(result && fileName != getFileName())
		setFileName(fileName);
	return result;
}

//------------------------------------
bool ofxImage::saveImage(string fileName){
	string localFileName = getFileName();
	if(fileName == "" && localFileName != "")
		fileName = localFileName;
	
	bool result = saveImageFromPixels(fileName, myPixels);

	if(result && fileName != getFileName())
		setFileName(fileName);
	
	return result;
}

//----------------------------------------------------------------
// copied directly from core ofImage::saveImageFromPixels, with added bool return value
bool ofxImage::saveImageFromPixels(string fileName, ofPixels &pix){
	bool result = false;
	if (pix.bAllocated == false){
		ofLog(OF_LOG_ERROR,"error saving image - pixels aren't allocated");
		return result;
	}
	
#ifdef TARGET_LITTLE_ENDIAN
	if (pix.bytesPerPixel != 1) swapRgb(pix);
#endif
	
	FIBITMAP * bmp	= getBmpFromPixels(pix);
	
#ifdef TARGET_LITTLE_ENDIAN
	if (pix.bytesPerPixel != 1) swapRgb(pix);
#endif
	
	fileName = ofToDataPath(fileName);
	if (pix.bAllocated == true){
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileType(fileName.c_str(), 0);
		if(fif == FIF_UNKNOWN) {
			// or guess via filename
			fif = FreeImage_GetFIFFromFilename(fileName.c_str());
		}
		if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
			result = FreeImage_Save(fif, bmp, fileName.c_str(), 0);
		}
	}
	
	if (bmp != NULL){
		FreeImage_Unload(bmp);
	}
	return result;
}


//------------------------------------
void ofxImage::setFileName(string fileName){
	this->fileName = fileName;
}

//------------------------------------
void ofxImage::mirror(bool horizontal, bool vertical){
	flipPixels(myPixels, horizontal, vertical);
	
	update();
}

//------------------------------------
void ofxImage::rotate(float angle){
	rotatePixels(myPixels, angle);
	
	tex.clear();
	if (bUseTexture == true){
		tex.allocate(myPixels.width, myPixels.height, myPixels.glDataType);
	}
	
	update();
}

//----------------------------------------------------
void ofxImage::flipPixels(ofPixels &pix, bool horizontal, bool vertical){
	if(!horizontal && !vertical)
		return;
	
	FIBITMAP * bmp               = getBmpFromPixels(pix);
	bool horSuccess = false, vertSuccess = false;
	
	if(horizontal)
		horSuccess = FreeImage_FlipHorizontal(bmp);
	if(vertical)
		vertSuccess = FreeImage_FlipVertical(bmp);
	
	if(horSuccess || vertSuccess)
		putBmpIntoPixels(bmp, pix);
	
	if (bmp != NULL)            FreeImage_Unload(bmp);
}

//----------------------------------------------------
void ofxImage::rotatePixels(ofPixels &pix, float angle){
	if(angle == 0.0)
		return;
	
	FIBITMAP * bmp               = getBmpFromPixels(pix);
	FIBITMAP * convertedBmp         = NULL;
	
	convertedBmp = FreeImage_RotateClassic(bmp, angle);
	putBmpIntoPixels(convertedBmp, pix);
	
	if (bmp != NULL)            FreeImage_Unload(bmp);
	if (convertedBmp != NULL)      FreeImage_Unload(convertedBmp);
}


void ofxImage::superFastBlur(int radius) {
	
	unsigned char *pix = myPixels.pixels;
	int w = myPixels.width;
	int h = myPixels.height;
	if (type == OF_IMAGE_COLOR_ALPHA) {
		superFastBlurAlpha(radius);
		return;
	}
	if (radius<1) return;
	int wm=w-1;
	int hm=h-1;
	int wh=w*h;
	int div=radius+radius+1;
	unsigned char *r=new unsigned char[wh];
	unsigned char *g=new unsigned char[wh];
	unsigned char *b=new unsigned char[wh];
	int rsum,gsum,bsum,x,y,i,p,p1,p2,yp,yi,yw;
	int *vMIN = new int[MAX(w,h)];
	int *vMAX = new int[MAX(w,h)];
	
	unsigned char *dv=new unsigned char[256*div];
	for (i=0;i<256*div;i++) dv[i]=(i/div);
	
	yw=yi=0;
	
	for (y=0;y<h;y++){
		rsum=gsum=bsum=0;
		for(i=-radius;i<=radius;i++){
			p = (yi + MIN(wm, MAX(i,0))) * 3;
			rsum += pix[p];
			gsum += pix[p+1];
			bsum += pix[p+2];
		}
		for (x=0;x<w;x++){
			
			r[yi]=dv[rsum];
			g[yi]=dv[gsum];
			b[yi]=dv[bsum];
			
			if(y==0){
				vMIN[x]=MIN(x+radius+1,wm);
				vMAX[x]=MAX(x-radius,0);
			}
			p1 = (yw+vMIN[x])*3;
			p2 = (yw+vMAX[x])*3;
			
			rsum += pix[p1]      - pix[p2];
			gsum += pix[p1+1]   - pix[p2+1];
			bsum += pix[p1+2]   - pix[p2+2];
			
			yi++;
		}
		yw+=w;
	}
	
	for (x=0;x<w;x++){
		rsum=gsum=bsum=0;
		yp=-radius*w;
		for(i=-radius;i<=radius;i++){
			yi=MAX(0,yp)+x;
			rsum+=r[yi];
			gsum+=g[yi];
			bsum+=b[yi];
			yp+=w;
		}
		yi=x;
		for (y=0;y<h;y++){
			pix[yi*3]      = dv[rsum];
			pix[yi*3 + 1]   = dv[gsum];
			pix[yi*3 + 2]   = dv[bsum];
			if(x==0){
				vMIN[y]=MIN(y+radius+1,hm)*w;
				vMAX[y]=MAX(y-radius,0)*w;
			}
			p1=x+vMIN[y];
			p2=x+vMAX[y];
			
			rsum+=r[p1]-r[p2];
			gsum+=g[p1]-g[p2];
			bsum+=b[p1]-b[p2];
			
			yi+=w;
		}
	}
	
	delete r;
	delete g;
	delete b;
	
	delete vMIN;
	delete vMAX;
	delete dv;
}

void ofxImage::superFastBlurAlpha(int radius) {
	unsigned char *pix = myPixels.pixels;
	int w = myPixels.width;
	int h = myPixels.height;
	
	if (radius<1) return;
	int wm=w-1;
	int hm=h-1;
	int wh=w*h;
	int div=radius+radius+1;
	unsigned char *r=new unsigned char[wh];
	unsigned char *g=new unsigned char[wh];
	unsigned char *b=new unsigned char[wh];
	unsigned char *a=new unsigned char[wh];
	int rsum,gsum,bsum,asum, x,y,i,p,p1,p2,yp,yi,yw;
	int *vMIN = new int[MAX(w,h)];
	int *vMAX = new int[MAX(w,h)];
	
	unsigned char *dv=new unsigned char[256*div];
	for (i=0;i<256*div;i++) dv[i]=(i/div);
	
	yw=yi=0;
	
	for (y=0;y<h;y++){
		rsum=gsum=bsum=asum=0;
		for(i=-radius;i<=radius;i++){
			p = (yi + MIN(wm, MAX(i,0))) * 4;
			rsum += pix[p];
			gsum += pix[p+1];
			bsum += pix[p+2];
			asum += pix[p+3];
		}
		for (x=0;x<w;x++){
			
			r[yi]=dv[rsum];
			g[yi]=dv[gsum];
			b[yi]=dv[bsum];
			a[yi]=dv[asum];
			if(y==0){
				vMIN[x]=MIN(x+radius+1,wm);
				vMAX[x]=MAX(x-radius,0);
			}
			p1 = (yw+vMIN[x])*4;
			p2 = (yw+vMAX[x])*4;
			
			rsum += pix[p1]      - pix[p2];
			gsum += pix[p1+1]   - pix[p2+1];
			bsum += pix[p1+2]   - pix[p2+2];
			asum += pix[p1+3]   - pix[p2+3];
			yi++;
		}
		yw+=w;
	}
	
	for (x=0;x<w;x++){
		rsum=gsum=bsum=asum=0;
		yp=-radius*w;
		for(i=-radius;i<=radius;i++){
			yi=MAX(0,yp)+x;
			rsum+=r[yi];
			gsum+=g[yi];
			bsum+=b[yi];
			asum+=a[yi];
			yp+=w;
		}
		yi=x;
		for (y=0;y<h;y++){
			pix[yi*4]      = dv[rsum];
			pix[yi*4 + 1]   = dv[gsum];
			pix[yi*4 + 2]   = dv[bsum];
			pix[yi*4 + 3]   = dv[asum];
			if(x==0){
				vMIN[y]=MIN(y+radius+1,hm)*w;
				vMAX[y]=MAX(y-radius,0)*w;
			}
			p1=x+vMIN[y];
			p2=x+vMAX[y];
			
			rsum+=r[p1]-r[p2];
			gsum+=g[p1]-g[p2];
			bsum+=b[p1]-b[p2];
			asum+=b[p1]-a[p2];
			yi+=w;
		}
	}
	
	delete r;
	delete g;
	delete b;
	delete a;
	delete vMIN;
	delete vMAX;
	delete dv;
}


