#ifndef _OFX_DYNAMIC_BACKGROUND_H_
#define _OFX_DYNAMIC_BACKGROUND_H_

#include <vector>
#include "ofMain.h"
#include "ofxOpenCv.h"

class ofxDynamicBackground{
private:
		
	ofxCvColorImage*		currentColorImage;
	ofxCvGrayscaleImage*	currentGrayImage;
	ofxCvGrayscaleImage*	averageChangeGrayImage;
	ofxCvGrayscaleImage*	backgroundGrayImage;
	std::vector< int >		pixellTimers;
	std::vector< bool > 	pixellChanged;
	int						maxLoopsChanged;
	int						threshold;
	int						numPixels;
	
public:
	void 					setup			( ofxCvColorImage*	inputColorImage , ofxCvGrayscaleImage*	outputBackground, int themaxLoopsChanged , int theThreshold );
	void 					update			( );
	void 					draw			( );
	void 					draw			( int thePosX , int thePosY );
	void 					draw			( int thePosX , int thePosY , int width , int height);
	void 					keyPressed		( int key );
	void 					mouseMoved		( int x , int y );
	void 					mouseDragged	( int x , int y , int button );
	void 					mousePressed	( int x , int y , int button );
	void 					mouseReleased	( );
	
	void					setThreshold	( int newThreshold );
	int						getThreshold	();
	void					setMaxLoopsChanged	( int newMaxLoopsChanged );
	int						getMaxLoopsChanged	();

		
};
#endif
