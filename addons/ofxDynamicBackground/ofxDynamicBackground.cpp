#include "ofxDynamicBackground.h"

//--------------------------------------------------------------
void ofxDynamicBackground::setup( ofxCvColorImage*	inputColorImage , ofxCvGrayscaleImage*	outputBackground, int themaxLoopsChanged , int theThreshold ){
	if( inputColorImage->getWidth() != outputBackground->getWidth() )
		printf( "Dynamic Bacgroud Error: Input and output images have diferent sizes \n" );

	if( inputColorImage->getHeight() != outputBackground->getHeight() )
		printf( "Dynamic Bacgroud Error: Input and output images have diferent sizes \n" );
	
	currentColorImage = inputColorImage;
	backgroundGrayImage = outputBackground;
	
	numPixels = outputBackground->getWidth() * outputBackground->getHeight();
	for( int p = 0 ; p < numPixels ; p ++ ){
		pixellTimers.push_back( 0 );
		pixellChanged.push_back( false );
	}
	currentGrayImage = new ofxCvGrayscaleImage();
	currentGrayImage->allocate( outputBackground->getWidth() , outputBackground->getHeight() );
	
	*currentGrayImage = *currentColorImage;
	*backgroundGrayImage = *currentColorImage;
	
	maxLoopsChanged = themaxLoopsChanged;
	threshold = theThreshold;
	//avarageGrayImage->allocate( outputBackground->getWidth() , outputBackground->getHeight() );
}	
//--------------------------------------------------------------
void ofxDynamicBackground::update(){
	*currentGrayImage = *currentColorImage;
	
	unsigned char* currentPixels = currentGrayImage->getPixels();
	unsigned char* backgroundPixels = backgroundGrayImage->getPixels();
	//unsigned char* avaragePixels = 
	
	for( int p = 0 ; p < numPixels ; p ++ ){
		//flag the pixelss that have changed from the background
		if( abs( currentPixels[ p ] - backgroundPixels[ p ] ) > threshold ){
			if( pixellChanged[ p ] )
				pixellTimers[ p ]++;
			else
				pixellChanged[ p ] = true;
		}
		
		else{
			if( pixellChanged[ p ] ){
				pixellChanged[ p ] = false;
				pixellTimers[ p ] = 0;
			}
		}
		
		if( pixellTimers[ p ] > maxLoopsChanged ){
			backgroundPixels[ p ] = currentPixels[ p ];
			pixellChanged[ p ] = false;
			pixellTimers[ p ] = 0;
		}
	}
	backgroundGrayImage->setFromPixels( backgroundPixels , currentGrayImage->getWidth() , currentGrayImage->getHeight() );
}
//--------------------------------------------------------------
void ofxDynamicBackground::draw(  ){
	currentGrayImage->draw( 0 , 0 );
	backgroundGrayImage->draw( 0 , currentGrayImage->getHeight() + 20 );
	
	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "Input Image" );
	ofDrawBitmapString(reportStr, 10, 20);
	sprintf(reportStr, "Dynamic Background by juliolucio\n\nBG Threshold = %i \nBG Pixell Max Time  = %i" , threshold , maxLoopsChanged );
	ofDrawBitmapString(reportStr, 10, 190);
	sprintf(reportStr, "Output Background Image" );
	ofDrawBitmapString(reportStr, 10, currentGrayImage->getHeight() + 40);
	
}
//--------------------------------------------------------------
void ofxDynamicBackground::draw( int thePosX , int thePosY ){

	//drawing corner points
	glPushMatrix();
	glTranslatef( thePosX , thePosY , 0 );
	draw();
	glPopMatrix();
}
//--------------------------------------------------------------
void ofxDynamicBackground::draw( int thePosX , int thePosY , int width , int height ){
	//drawing corner points
	glPushMatrix();
	glTranslatef( thePosX , thePosY , 0 );
	glScalef( 1, 1, 1 );
	draw();
	glPopMatrix();
	printf( "Dynamic Bacgroud Warning: Not Implemented  method : void draw( int thePosX , int thePosY , int width , int height );\n" );
}
//--------------------------------------------------------------
void ofxDynamicBackground::keyPressed( int key ){ 
	if( key == '+' ){
		threshold += 1;
		if( threshold > 255 )
			threshold = 255;
		return;
	}
	
	else if( key == '-' ){
		threshold -= 1;
		if( threshold <= 0 )
			threshold = 0;
		return;
	}
	
	else if( key == 'q' ){
		maxLoopsChanged += 100;
		if( maxLoopsChanged > 10000 )
			maxLoopsChanged = 10000;
		return;
	}
	
	
	else if( key == 'a' ){
		maxLoopsChanged -= 100;
		if( maxLoopsChanged <= 0 )
			maxLoopsChanged = 0;
		return;
	}
	
	else {
		return;
	}
}
//--------------------------------------------------------------
void ofxDynamicBackground::mouseMoved(int x, int y ){
}	


//--------------------------------------------------------------
void ofxDynamicBackground::mousePressed(int x, int y, int button){

}
//--------------------------------------------------------------
void ofxDynamicBackground::mouseDragged(int x, int y, int button ){

}
//--------------------------------------------------------------
void ofxDynamicBackground::mouseReleased(){
}

//--------------------------------------------------------------
void ofxDynamicBackground::setThreshold( int theNewThreshold ){
	threshold = theNewThreshold;
}
//--------------------------------------------------------------
int ofxDynamicBackground::getThreshold(){
	return threshold;
}
//--------------------------------------------------------------
void ofxDynamicBackground::setMaxLoopsChanged( int theMaxLoopsChanged ){
	maxLoopsChanged = theMaxLoopsChanged;
}
//--------------------------------------------------------------
int ofxDynamicBackground::getMaxLoopsChanged(){
	return maxLoopsChanged;
}