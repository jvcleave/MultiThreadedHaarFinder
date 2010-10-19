/*
 *  jvcHaarFinder.cpp
 *  MultiThreadedHaarFinder
 *
 *  Created by Jason Van Cleave on 10/5/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "jvcHaarFinder.h"

int jvcHaarFinder::findHaarObjects(ofxCvGrayscaleImage& input,
									 int x, int y, int w, int h,
									 int minWidth, int minHeight) {
	
	int nHaarResults = 0;
	
	if (cascade) {
		if (!blobs.empty())
			blobs.clear();
		
		// we make a copy of the input image here
		// because we need to equalize it.
		
		if (img.width == input.width && img.height == input.height) {
			img = input;
		} else {
			img.clear();
			//JVC This is primarily the only reason for the existance of this class - to set the texture to false to enable multi-threading
			img.setUseTexture(false);
			img.allocate(input.width, input.height);
			
			img = input;
		}
		
		img.setROI(x, y, w, h);
		cvEqualizeHist(img.getCvImage(), img.getCvImage());
		CvMemStorage* storage = cvCreateMemStorage();
		
		/*
		 Alternative modes:
		 
		 CV_HAAR_DO_CANNY_PRUNING
		 Regions without edges are ignored.
		 
		 CV_HAAR_SCALE_IMAGE
		 Scale the image rather than the detector
		 (sometimes yields speed increases).
		 
		 CV_HAAR_FIND_BIGGEST_OBJECT
		 Only return the largest result.
		 
		 CV_HAAR_DO_ROUGH_SEARCH
		 When BIGGEST_OBJECT is enabled, stop at
		 the first scale for which multiple results
		 are found.
		 */
		
		CvSeq* haarResults = cvHaarDetectObjects(
												 img.getCvImage(), cascade, storage, scaleHaar, neighbors, CV_HAAR_DO_CANNY_PRUNING,
												 cvSize(minWidth, minHeight));
		
		nHaarResults = haarResults->total;
		
		for (int i = 0; i < nHaarResults; i++ ) {
			ofxCvBlob blob;
			
			CvRect* r = (CvRect*) cvGetSeqElem(haarResults, i);
			
			float area = r->width * r->height;
			float length = (r->width * 2) + (r->height * 2);
			float centerx	= (r->x) + (r->width / 2.0);
			float centery	= (r->y) + (r->height / 2.0);
			
			blob.area = fabs(area);
			blob.hole = area < 0 ? true : false;
			blob.length	= length;
			blob.boundingRect.x = r->x + x;
			blob.boundingRect.y = r->y + y;
			blob.boundingRect.width = r->width;
			blob.boundingRect.height = r->height;
			blob.centroid.x = centerx;
			blob.centroid.y = centery;
			blob.pts.push_back(ofPoint(r->x, r->y));
			blob.pts.push_back(ofPoint(r->x + r->width, r->y));
			blob.pts.push_back(ofPoint(r->x + r->width, r->y + r->height));
			blob.pts.push_back(ofPoint(r->x, r->y + r->height));
			blobs.push_back(blob);
			
		}
		
		cvReleaseMemStorage(&storage);
	}
	
	return nHaarResults;
}