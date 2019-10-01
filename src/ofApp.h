#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxTrueTypeFontUL2.h"
#include "ofxOsc.h"

#define PORT 9002
#define RESETSCOUNDS 120

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
      int w, h;
     int camW, camH;
     ofVideoGrabber vid;
     ofFbo outFbo;
     ofImage testImg;
     ofxTrueTypeFontUL2 font;
     ofxTrueTypeFontUL2 fontBig;
     
     ofVec2f point[4];
     ofVec2f imageEdge[4];
     
     ofxOscReceiver receiver;
};
