/*
 *  Demo.cpp
 *  MultiThreadedHaarFinder
 *
 *  Created by jason van cleave on 10/19/10.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include "Demo.h"

string info;
int videoWidth;
int videoHeight;
bool useCamera = false;
int frameCounter =0;
bool doLearnBackground = false;
bool hasStarted = false;
bool forceGuiDraw = false;
float acceptableFrameRate = 10.0f;
int startWidth =0;
int widthCounter =0;
int heightCounter = 0;
int startHeight = 0;
int minBlobWidth = 20;
int minBlobHeight = 20;
bool doColorize = true;
bool doDrawBackground = false;
//--------------------------------------------------------------
void Demo::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(false);
	videoWidth = 640;
	videoHeight = 480;
	blur = 0;
	threshold = 53;
	//
	//startWidth = videoWidth;
	widthCounter= startWidth;
	//
	backgroundImage.allocate(videoWidth, videoHeight);
	if(useCamera)
	{
		videoGrabber.listDevices();
		videoGrabber.setVerbose(true);
		videoGrabber.initGrabber(videoWidth, videoHeight);
		videoGrabber.grabFrame();
	}else
	{
		initVideo();
		//for the demo lets grab the first frame of black
		doLearnBackground = true;
	}
	gui.addSlider("threshold", threshold, 0.0, 200);
	gui.addSlider("blur", blur, 0.0, 20);
	//
	gui.addSlider("blob minWidth", minBlobWidth, 2, videoWidth);
	gui.addSlider("blob minHeight", minBlobHeight, 2, videoHeight);
	gui.addToggle("doColorize", doColorize);
	createMultipleThreads(FACE, 4);
	//createMultipleThreads(FACE_ALT, 4);
	
	//a few Haar options - hopefully you get the point
	//	createThread(FACE);
	//	createThread(FACE_ALT);
	//	createThread(FACE_PROFILE);
	//	createThread(EYE);
	//	createThread(UPPER_BODY);
	//	createThread(LOWER_BODY);
	//	createThread(FULL_BODY);
	//
	
	//createMultipleThreads(FACE, 4);
	//createMultipleThreads(FACE_ALT, 4);
	//createMultipleThreads(FACE_PROFILE, 4);
	//createThread(UPPER_BODY);
	//createThread(LOWER_BODY);
	//createMultipleThreads(FULL_BODY, 2);
	
	for(int i=0; i<threads.size(); i++){
		threads[i]->videoPixels = getVideoPixels();
		threads[i]->blur = blur;
		threads[i]->threshold = threshold;
		threads[i]->minWidth = minBlobWidth;
		threads[i]->minHeight = minBlobHeight;
		threads[i]->initAndSleep();
		
	}
}
void Demo::createThread(string haarType)
{
	SimpleThread * thread = new SimpleThread();
	thread->setupFinder(haarType, videoWidth, videoHeight);
	threads.push_back(thread);
}
void Demo::createMultipleThreads(string haarType, int numInstances)
{
	for (int i=0; i<numInstances; i++) {
		createThread(haarType);
	}
}
void Demo::createBackground()
{
	backgroundFileImage.setFromPixels(getVideoPixels(), videoWidth, videoHeight, OF_IMAGE_COLOR, true);		
	backgroundFileImage.setImageType(OF_IMAGE_GRAYSCALE);
	backgroundImage.setFromPixels(backgroundFileImage.getPixels(), videoWidth, videoHeight);
	cout << "createBackground" << endl;
	
}
//--------------------------------------------------------------
void Demo::update(){
	if(useCamera)
	{
		videoGrabber.grabFrame();
	}
	if(doLearnBackground)
	{
		createBackground();
		for(int i=0; i<threads.size(); i++){
			
			threads[i]->backgroundImage = backgroundImage;
		}
		hasStarted = true;
		doLearnBackground = false;
	}
	
	if (hasStarted) 
	{
		if (ofGetFrameRate() >= acceptableFrameRate) {
			for(int i=0; i<threads.size(); i++)
			{
				if(threads[i]->isReady)
				{
					threads[i]->updateOnce();
				}
				
			}
		}else {
			//blobImages.clear();
		}
		
		
		
		for(int i=0; i<threads.size(); i++)
		{
			if(threads[i]->isReady)
			{
				
				threads[i]->blur = blur;
				threads[i]->threshold = threshold;
				threads[i]->minWidth = minBlobWidth;
				threads[i]->minHeight = minBlobHeight;
				for(int j=0; j<threads[i]->numBlobs; j++)
				{
					ofxCvBlob blob = threads[i]->finder.blobs[j];
					
					int w = blob.boundingRect.width;
					int h = blob.boundingRect.height;
					
					
					
					ofxCvColorImage rgbImage;
					
					
					rgbImage.allocate(videoWidth, videoHeight);
					rgbImage = threads[i]->colorImage;
					rgbImage.setROI(blob.boundingRect);
					
					ofxImage blobImage;
					blobImage.blob = blob;
					blobImage.haarType = threads[i]->haarPath;
					blobImage.setFromPixels(rgbImage.getRoiPixels(), w, h, OF_IMAGE_COLOR);
					
					blobImage.update();
					blobImages.push_back(blobImage);
					
					rgbImage.resetROI();
					rgbImage.clear();
					
				}
			}
		}
		info = "threshold:" + ofToString(threshold) + "\n";
		info.append("threshold:" + ofToString(blur) + "\n");
		info.append("main thread:" + ofToString(ofGetFrameNum()) + "\n");
		for(int i=0; i<threads.size(); i++)
		{
			info.append( "thread:" + ofToString(i,0) + ":" + threads[i]->haarPath + " " + ofToString(threads[i]->count) +  "\n");
		}
		info.append("FPS: " +ofToString(ofGetFrameRate())+"\n");
		info.append("acceptableFrameRate: " +ofToString(int(acceptableFrameRate))+"\n");
		
	}
	//cout << info << endl;
	if(!forceGuiDraw)
	{
		gui.hide();
	}else
	{
		gui.show();
	}
}

//--------------------------------------------------------------
void Demo::draw(){
	
	if(useCamera)
	{
		videoGrabber.draw(0, 0, videoWidth, videoHeight);
	}else {
		videoPlayer.draw(0, 0, videoWidth, videoHeight);
	}
	ofPushStyle();
		ofSetColor(0x00FF00);
		backgroundImage.draw(videoWidth, 0, videoWidth, videoHeight);
		ofDrawBitmapString(info, videoWidth, 20);
	ofPopStyle();
	
	
	int numToDraw=40;
	if (blobImages.size()<numToDraw) {
		numToDraw = blobImages.size();
	}else {
		blobImages.clear();
		numToDraw = 0;
	}
	float color = 0xFFFFFF;
	string haarTypeToString = " ";
	for (int i=0; i<numToDraw; i++)
	{
		ofxImage blobImage = blobImages[i];
		if(blobImage.haarType == FACE_ALT) 
		{
			color = GRAY_COLOR;
			haarTypeToString="FACE_ALT";
		}
		if(blobImage.haarType == FACE) 
		{
			color = RED_COLOR;
			haarTypeToString="FACE";
		}
		if(blobImage.haarType == FACE_PROFILE)
		{
			color = BLUE_COLOR;
			haarTypeToString="FACE_PROFILE";
			
		}
		if(blobImage.haarType == FULL_BODY)
		{
			color = GREEN_COLOR;
			haarTypeToString="FULL_BODY";
		}
		
		if(blobImage.haarType == EYE) 
		{
			color = CYAN_COLOR;
			haarTypeToString="EYE";
		}
		if(blobImage.haarType == UPPER_BODY) 
		{
			color = MAGENTA_COLOR;
			haarTypeToString="UPPER_BODY";
		}
		if(blobImage.haarType == LOWER_BODY) 
		{
			color = YELLOW_COLOR;
			haarTypeToString="LOWER_BODY";
		}
		
		if((widthCounter+blobImages[i].width) < ofGetWidth())
		{
			widthCounter+=blobImages[i].width;
			if(heightCounter<blobImages[i].height)
			{
				heightCounter = blobImages[i].height;
			}
		}else {
			widthCounter = startWidth;
			startHeight = heightCounter;
		}
		ofPushStyle();
			if(doColorize) ofSetColor(color);
			blobImage.draw(widthCounter, videoHeight/2);
		ofPopStyle();
	}
	
	//draw blobs
	ofPushStyle();
	for(int i=0; i<threads.size(); i++)
	{
		if (threads[i]->isReady) 
		{
			if(threads[i]->finder.blobs.size()>0)
			{
				ofPushMatrix();
				for(int j=0; j<threads[i]->numBlobs; j++)
				{
					ofxCvBlob blob = threads[i]->finder.blobs[j];
					blob.draw();
				}
				ofPopMatrix();
			}
		}
	}
	ofPopStyle();
	
	//draw thread images
	if (hasStarted) 
	{
		
		int numThreads = threads.size();
		for(int i=0; i<numThreads; i++)
		{
			if (threads[i]->isReady)
			{
				threads[i]->grayImage.draw(i*(videoWidth/numThreads), videoHeight, (videoWidth/numThreads), (videoHeight/numThreads));
				ofPushStyle();
					ofSetColor(color);
					ofPushMatrix();
				ofDrawBitmapString(ofToString(i, 0), (i*(videoWidth/numThreads))+((videoWidth/numThreads)/2), videoHeight);
					ofPopMatrix();
				ofPopStyle();
			}
			
		}
		
	}	
	
	gui.draw();
	ofPushStyle();
		ofPushMatrix();
			ofSetColor(color);
			ofDrawBitmapString(haarTypeToString, (videoWidth*2)-200, 20);
		ofPopMatrix();
	ofPopStyle();
	
}

unsigned char * Demo::getVideoPixels()
{
	if(useCamera)
	{
		//
		return videoGrabber.getPixels();
	}else {
		//videoPlayer.setFrame(frameCounter);
		videoPlayer.nextFrame();
		return videoPlayer.getPixels();
	}
	
}
void Demo::initVideo()
{
	videoPlayer.loadMovie( VIDEO_SOURCE );
	videoPlayer.setLoopState( OF_LOOP_NORMAL );
	videoPlayer.play();
}
//--------------------------------------------------------------
void Demo::keyPressed(int key){
	if( key == ' ' ) 
	{
        doLearnBackground = true;
    }
	if(key == 'g')
	{
		forceGuiDraw = !forceGuiDraw;
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

//--------------------------------------------------------------
void Demo::keyReleased(int key){
	
}

//--------------------------------------------------------------
void Demo::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void Demo::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void Demo::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void Demo::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void Demo::windowResized(int w, int h){
	
}

