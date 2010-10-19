#ifndef _DEMO_APP
#define _DEMO_APP


#include "ofMain.h"
#include "SimpleThread.h"
#include "ofxImage.h"
#include "ofxSimpleGuiToo.h"

class Demo : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	ofVideoGrabber 		videoGrabber;
	ofVideoPlayer videoPlayer;
	vector<SimpleThread *> threads;
	vector<ofxImage> blobImages;
	unsigned char * getVideoPixels();
	void initVideo();
	void createBackground();
	void createThread(string haarPath);
	void createMultipleThreads(string haarType, int numInstances);
	ofImage backgroundFileImage;
	
	ofxCvGrayscaleImage backgroundImage;
	//
	ofxSimpleGuiToo	gui;
	int threshold;
	int blur;
	ofxCvContourFinder 	contourFinder;
	
	
#define FACE "haarcascade_frontalface_default.xml"
#define FACE_ALT "haarcascade_frontalface_alt.xml"
#define FACE_PROFILE "haarcascade_profileface.xml"
#define EYE "haarcascade_eye.xml"
#define FULL_BODY "haarcascade_fullbody.xml"
	
#define UPPER_BODY "haarcascade_upperbody.xml"
#define LOWER_BODY "haarcascade_lowerbody.xml"
	
#define RED_COLOR 0xFF0000
#define GREEN_COLOR 0x00FF00
#define BLUE_COLOR 0x0000FF
#define CYAN_COLOR 0x00FFFF
	
#define MAGENTA_COLOR 0xFF00FF
#define YELLOW_COLOR 0xFFFF00
#define GRAY_COLOR 0x999999
#define VIDEO_SOURCE "StudioCrowdRAM-JPEG 100 NTSC.mov"
};

#endif
