#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
     ofSetVerticalSync(true);
     ofSetFrameRate(30);
     ofBackground(160);
     receiver.setup(PORT);
     
     camW = 1920;
     camH = 1080;
     vid.listDevices();
     vid.setDeviceID(0);
     vid.initGrabber(camW, camH);
     cout << "vidW" << vid.getWidth() << endl;
     cout << "vidH" << vid.getHeight() << endl;
     testImg.load("sample.gif");
     testImg.resize(200,200);
     font.loadFont("Arial Bold.ttf", 10);
     fontBig.loadFont("Arial Bold.ttf", 50);
     outFbo.allocate(camW, camH, GL_RGB);

}

//--------------------------------------------------------------
void ofApp::update(){
     w = ofGetWidth();
     h = ofGetHeight();
     vid.update();
     
     //OSC
     while(receiver.hasWaitingMessages()){
          ofxOscMessage m;
          receiver.getNextMessage(m);
          if(m.getAddress() == "/point:0"){
               point[0].set(m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:1"){
               point[1].set(m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:2"){
               point[2].set(m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:3"){
               point[3].set(m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
     }
     
     for(int i=0; i<4; i++){
          imageEdge[i] = point[i];
     }

}

//--------------------------------------------------------------
void ofApp::draw(){
     ofSetColor(255);
     outFbo.begin();
     ofClear(0);
     vid.draw(camW, camH, -camW, -camH);
     outFbo.end();
     
     outFbo.draw(w*0.5, 0, w*0.5, h*0.5);
     
     ofSetColor(255);
     stringstream oscMes;
     for(int i=0; i<4; i++){
          oscMes << "Point: " << i << point[i] << "\n";
     }
     stringstream buf;
     buf << "listening for osc messages on port: " << PORT
     << "\n" << oscMes.str();
     
     ofDrawBitmapString(buf.str(), w*0.01, h*0.02);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
