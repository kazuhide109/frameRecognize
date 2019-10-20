#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
     ofSetVerticalSync(true);
     ofSetFrameRate(60);
     ofBackground(160);
     receiver.setup(PORT);
     
     camW = 1920;
     camH = 1080;
     vid.listDevices();
     vid.setDeviceID(1);
     vid.initGrabber(camW, camH);
     cout << "vidW" << vid.getWidth() << endl;
     cout << "vidH" << vid.getHeight() << endl;
     testImg.load("sample.gif");
     testImg.resize(200,200);
     font.loadFont("Arial Bold.ttf", 10);
     fontBig.loadFont("Arial Bold.ttf", 50);
     vidFbo.allocate(camW, camH, GL_RGB);
     outFbo.allocate(camW, camH, GL_RGB);
     pointsFbo.allocate(camW, camH, GL_RGBA);

}

//--------------------------------------------------------------
void ofApp::update(){
     ofSetWindowTitle(ofToString(ofGetFrameRate()));
     w = ofGetWidth();
     h = ofGetHeight();
     vid.update();
     vidFbo.begin();
     ofClear(0);
     vid.draw(camW, camH, -camW, -camH);
     vidFbo.end();
     
     //OSC
     oscClient();
     
     //インデックス処理
     if(bolbNum > 0){
          for(int i=0; i<4; i++){
               imageEdge[i] = point[i];
          }
          //トラッキング
          float blobX=0;
          float blobY=0;
          for(int i=0; i<bolbNum; i++){
               blobX += point[i].x;
               blobY += point[i].y;
          }
          ave.set(blobX/4, blobY/4);
          int pp[4][4] =
          {{0, 2, 3, 1}, {1, 3, 0, 2}, {3, 1, 2, 0}, {2, 0, 1, 3}};
          
          if(!isSetPointers){
               float bufDif[4];
               for(int n=0; n<4; n++){
                    float def;
                    float miniDist = 10000;
                    for(int m=0; m<4; m++){
                         def = (imageEdge[m] - prePos[n]).length();
                         if(def < miniDist){
                              miniDist = def;
                              bufDif[n] = def;
                              p[n] = imageEdge[m];
                              e[n] = n;
                         }
                    }
               }
               for(int n=0; n<4; n++){
                    prePos[n] = p[n];
               }
               if(p[0] == p[1]) isSetPointers = true;
               else if(p[0]==p[2])isSetPointers = true;
               else if(p[0] == p[3])isSetPointers = true;
               else if(p[1] == p[2])isSetPointers = true;
               else if(p[1] == p[3])isSetPointers = true;
               else if(p[2] == p[3])isSetPointers = true;
               else isSetPointers = false;
               
          }else{
               int total = 0;
               //左上
               for(int n=0; n<bolbNum; n++){
                    if(ave.x > imageEdge[n].x && ave.y > imageEdge[n].y){
                         p[pp[framePose][0]] = imageEdge[n];
                         e[pp[framePose][0]] = n;
                         prePos[pp[framePose][0]] = p[pp[framePose][0]];
//                         posAveX  << "\n" << 0 << ": " << finderRed.blobs[n].centroid .x;
                    }
               }
               //右下
               for(int n=0; n<bolbNum; n++){
                    if(ave.x < imageEdge[n].x && ave.y < imageEdge[n].y){
                         p[pp[framePose][1]] = imageEdge[n];
                         e[pp[framePose][1]] = n;
                         prePos[pp[framePose][1]] = p[pp[framePose][1]];
//                         posAveX  << "\n" << 2 << ": " << finderRed.blobs[n].centroid .x;
                    }
               }
               //左下
               for(int n=0; n<bolbNum; n++){
                    if(ave.y < imageEdge[n].y && ave.x > imageEdge[n].x){
                         p[pp[framePose][2]] = imageEdge[n];
                         e[pp[framePose][2]] = n;
                         prePos[pp[framePose][2]] = p[pp[framePose][2]];
//                         posAveX  << "\n" << 3 << ": " << finderRed.blobs[n].centroid .x;
                    }
               }
               //右上
               for(int n=0; n<bolbNum; n++){
                    if(ave.y > imageEdge[n].y && ave.x < imageEdge[n].x){
                         e[pp[framePose][3]] = n;
                         p[pp[framePose][3]] = imageEdge[n];
                         prePos[pp[framePose][3]] = p[pp[framePose][3]];
//                         posAveX  << "\n" << 1 << ": " << finderRed.blobs[n].centroid .x;
                    }
               }
               if(p[0] == p[1]) isSetPointers = true;
               else if(p[0]==p[2])isSetPointers = true;
               else if(p[0] == p[3])isSetPointers = true;
               else if(p[1] == p[2])isSetPointers = true;
               else if(p[1] == p[3])isSetPointers = true;
               else if(p[2] == p[3])isSetPointers = true;
               else isSetPointers = false;
          }
          
     }else if(bolbNum < 1){
          isSetPointers = true;
     }
     
     //フレームの向き
     float swichLine = 59.037 / 90;
     if(accelZ < 0){
          accelYfix = ofMap(accelY, -1.0, 1.0, -90, 90);
          if(accelY < swichLine && accelY > -swichLine) framePose = 0;
          else if(swichLine < accelY && accelY < 1.0) framePose = 1;
          else if(-1.0 < accelY && accelY < -swichLine) framePose = 2;
     }else{
          accelYfix = 180 - ofMap(accelY, -1.0, 1.0, -90, 90);
          if(accelY < swichLine && accelY > -swichLine) framePose = 3;
          else if(swichLine < accelY && accelY < 1.0) framePose = 1;
          else if(-1.0 < accelY && accelY < -swichLine) framePose = 2;
     }
     
     //静止状態持続後、ループへ戻る処理
     float dif = abs(ave.length() - debugAve.length());
     if(dif < 20 && recMode != 0){ //&& RecMode != 0
          if((ofGetElapsedTimef() - standStillTime) > RESETSCOUNDS){
               recMode = 0;
               bRec  = true;
               bPlayRec = false;
               savePixels.clear();
               saveTexcodePos.clear();
               recFrame = 0;
          }
     }else{
          standStillTime = ofGetElapsedTimef();
     }
     debugAve = ave;
     
     //スイッチの判定 0が返ってくるとfalseに変更
     if(switchState == false){
          if(stateCount == 0){
               if(recMode == 1){
                    recMode = 2;
                    bRec = true;
               }
               else{
                    recMode = 1;
                    bPlayRec = false;
                    playCounter = 0;
                    savePixels.clear();
                    saveTexcodePos.clear();
               }
          }
          stateCount++;
     }else{
          stateCount = 0;
     }
     
     //録画中の処理
     if(bRec){
          if(recMode == 0) nRecFrame = 60;
          ofPixels pix;
          pix.allocate(vidFbo.getWidth(), vidFbo.getHeight(), 3);
          savePixels.push_back(pix);
          vidFbo.readToPixels(savePixels[recFrame]);
          vector<ofVec2f> saveTexVec;
          saveTexcodePos.push_back(saveTexVec);
          saveTexcodePos[recFrame].resize(4);
          if(bolbNum > 0){
               for(int i=0; i<4; i++){
                    saveTexcodePos[recFrame][i] = p[i];
                    
               }
          }
          recFrame++;
          
          if(bolbNum < 4){
               bRec = true;
               recFrame = 0;
               savePixels.clear();
               saveTexcodePos.clear();
               recMode = 0;
          }
          if(recMode == 0){
               if(recFrame >= nRecFrame){
                    bRec = false;
                    recFrame = 0;
                    bPlayRec = true;
                    fixFrame = ofGetFrameNum();
               }
          }else if(recMode == 2){
               if(stateCount == 0){
                    bRec = false;
                    nRecFrame = recFrame;
                    recFrame = 0;
                    bPlayRec = true;
                    fixFrame = ofGetFrameNum();
               }
          }
     }
     
     if(bPlayRec){
          currentFrame = (ofGetFrameNum() - fixFrame)%nRecFrame;
          if(recMode == 0){
               if(currentFrame == 0) playCounter++;
               if(playCounter > maxLoopCnt){
                    playCounter = 0;
                    bRec  = true;
                    bPlayRec = false;
                    savePixels.clear();
                    saveTexcodePos.clear();
                    for(int i=0; i<nRecFrame; i++){
                         savePixels[i].setColor(camW, camH, 3);
                    }
               }
          }
     }
     
     mesh.clear();
     if(bolbNum > 0){
          mesh.addVertex(p[0]);
          mesh.addVertex(p[1]);
          mesh.addVertex(p[2]);
          mesh.addVertex(p[3]);
     }else{
          for(int i=0; i<4; i++){
               mesh.addVertex(imageEdge[i]);
          }
     }
     mesh.addIndex(0);
     mesh.addIndex(1);
     mesh.addIndex(2);
     mesh.addIndex(2);
     mesh.addIndex(0);
     mesh.addIndex(3);
     
     //ポイント位置のFBO
     pointsFbo.begin();
     ofClear(0);
     ofSetColor(200, 0, 0, 200);
     for(int i=0; i<4; i++){
          ofDrawCircle(p[i], 20);
          fontBig.drawString(ofToString(e[i]), p[i].x + w*0.02, p[i].y);
     }
      ofSetColor(255, 0, 255, 200);
     ofDrawCircle(ave, 10);//枠の中心を描画
     pointsFbo.end();

}

//--------------------------------------------------------------
void ofApp::draw(){
     //guide
     ofSetColor(255);
     ofSetLineWidth(1);
     for(int i=1; i<10; i++){
          ofDrawLine(i*w*0.1, 0, i*w*0.1, h);
          ofDrawLine(0, i*h*0.1, w, i*h*0.1);
     }
     
     ofSetColor(255);
     outFbo.begin();
     ofClear(0);
     vidFbo.draw(0, 0);
     
     if(bPlayRec){
          for(int i=0; i<4; i++){
               mesh.addTexCoord(saveTexcodePos[currentFrame][i]);
          }
          ofSetColor(255);
          ofTexture recBind;
          recBind.allocate(savePixels[currentFrame]);
          if(bolbNum>3){ //ライトが隠れた時の非表示
               recBind.bind();
               ofPopMatrix();
               mesh.draw();
               ofPushMatrix();
               recBind.unbind();
          }
     }else{
          mesh.addTexCoord(ofVec2f(0, 0));
          mesh.addTexCoord(ofVec2f(testImg.getWidth(), 0));
          mesh.addTexCoord(ofVec2f(testImg.getWidth(), testImg.getHeight()));
          mesh.addTexCoord(ofVec2f(0, testImg.getHeight()));
          //テストイメージ
          //          testImg.bind();
          //          ofPopMatrix();
          //          mesh.draw();
          //          ofPushMatrix();
          //          testImg.unbind();
          
     }
     
     outFbo.end();
     outFbo.draw(w*0.5, 0, w*0.5, h*0.5);
     pointsFbo.draw(w*0.5, 0, w*0.5, h*0.5);
     
     //monitor
     vidFbo.draw(w*0.5, h*0.5, w*0.25, h*0.25);
     ofSetColor(20);
     ofDrawRectangle(w*0.75, h*0.5, w*0.25, h*0.25);
     ofSetColor(255);
     pointsFbo.draw(w*0.75, h*0.5, w*0.25, h*0.25);
     
     //Data
     ofSetColor(255);
     stringstream oscMes;
     for(int i=0; i<4; i++){
          oscMes << "Point: " << i << point[i] << "\n";
     }
     oscMes << "BolbNum: " << bolbNum;
     stringstream espMes;
     espMes << "\n" << "[ESP]"
     << "accelX: " << accelX << "\n"
     << "accelY: " << accelY << "\n"
     << "accelZ: " << accelZ << "\n"
     << "gyroX: " << gyroX << "\n"
     << "gyroY: " << gyroY << "\n"
     << "gyroZ: " << gyroZ << "\n"
     << "switch: " << switchState << "\n";
     stringstream buf;
     buf << "listening for osc messages on port: " << PORT
      << "\n" << "[sendFramePoints]"
     << "\n" << oscMes.str()
     << "\n" << espMes.str();
     
     ofDrawBitmapString(buf.str(), w*0.01, h*0.02);

}

void ofApp::oscClient(){
     while(receiver.hasWaitingMessages()){
          ofxOscMessage m;
          receiver.getNextMessage(m);
          if(m.getAddress() == "/point:0"){
               point[0].set(camW - m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:1"){
               point[1].set(camW - m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:2"){
               point[2].set(camW - m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/point:3"){
               point[3].set(camW - m.getArgAsFloat(0), m.getArgAsFloat(1));
          }
          if(m.getAddress() == "/size:"){
               bolbNum = m.getArgAsInt(0);
          }
          
          if(m.getAddress() == "/accelX"){
               accelX = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/accelY"){
               accelY = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/accelZ"){
               accelZ = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/gyroX"){
               gyroX = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/gyroY"){
               gyroY = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/gyroZ"){
               gyroZ = m.getArgAsFloat(0);
          }
          if(m.getAddress() == "/switch"){
               switchState = m.getArgAsInt(0);
          }
     }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
     switch (key) {
     case ' ':
          bRec  = true;
          bPlayRec = false;
          playCounter = 0;
          savePixels.clear();
          saveTexcodePos.clear();
          recMode = 0;
          break;
     case 'p':
          isSetPointers = !isSetPointers;
          break;
          
     default:
          break;
     }

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
