#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxTrueTypeFontUL2.h"
#include "ofxOsc.h"

#define PORT 9002
#define RESETSCOUNDS 30

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
     
     void oscClient();
		
      int w, h;
     int camW, camH;
     ofVideoGrabber vid;
     ofFbo vidFbo;
     ofFbo pointsFbo;
     ofFbo outFbo;
     ofImage testImg;
     ofxTrueTypeFontUL2 font;
     ofxTrueTypeFontUL2 fontBig;
     
     ofVec2f point[4];
     int bolbNum;
     ofVec2f imageEdge[4];
     
     ofVec2f ave;
     ofVec2f debugAve;
     bool isSetPointers;
     ofVec2f prePos[4];
     ofVec2f p[4];
     int e[4];
     int framePose;
     
     int recMode;
     float standStillTime;
     bool bRec;
     bool bPlayRec;
     int recFrame = 0;
     vector<ofPixels> savePixels;
     vector<vector<ofVec2f>> saveTexcodePos;
     int nRecFrame;
     int fixFrame;
     int currentFrame;
     int maxLoopCnt = 4;
     
     int stateCount;
     int playCounter;
     ofMesh mesh;
     
     float accelX;
     float accelY;
     float accelZ;
     float gyroX;
     float gyroY;
     float gyroZ;
     bool switchState = 1;
     float accelYfix;
     
     ofxOscReceiver receiver;
};
