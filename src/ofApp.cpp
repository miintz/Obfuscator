#include "ofApp.h"
#include "ofxOpenCv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;

float textWidth;
float textHeight;
  
float maxDistance;

std::string text = "test";

//--------------------------------------------------------------
void ofApp::setup(){

	//shader
	//shader.load( "shaderVert.c", "shaderFrag.c" );
	//fbo.allocate( ofGetWidth(), ofGetHeight() );
    // SETUP VIDEO INPUT
    camWidth = CAM_WIDTH;
    camHeight = CAM_HEIGHT;
	vidGrabber.setVerbose(true);

	//vidGrabber.listDevices();
	vidGrabber.initGrabber(camWidth,camHeight);
	
    // SETUP FACE DETECTION
	img.loadImage("test.jpg");
	finder.setup("haarcascade_frontalface_default.xml");
	finder.findHaarObjects(img);

	mask.loadImage("mask.png");
    mask.resize(PCA_WIDTH, PCA_HEIGHT);
    mask_pixels = mask.getPixels();

    // SETUP FACE RECOGNITION
    rec.learn();
    gray.allocate(PCA_WIDTH, PCA_HEIGHT);
    color.allocate(PCA_WIDTH, PCA_HEIGHT);

    // PRECALCULATE TRANSLUCENT FACE SPRITES
    calcFaceSprites();

    showEigens=false;
    showFaces=false;
    showExtracted=false;
    showTest = false;
    showLeastSq=false;
    showClock=false;
    bgSubtract=false;

    ofBackground(0,0,0);
    ofEnableAlphaBlending();
    ofHideCursor();	

	ofTrueTypeFont::setGlobalDpi(72);

	font.loadFont("molot.ttf", 32);	
	font.setLineHeight(18.0f);
	font.setLetterSpacing(1.037);
	 
	textWidth = font.stringWidth(text);
    textHeight = font.stringHeight(text);
	
	//particles 

	ofSetVerticalSync(true);

	int num = 15;
	p.assign(num, Particle());
	
	resetParticles();

	//canny edge

	cannygray.allocate(640, 480);  
	canny.allocate(640, 480); 
	display.allocate(640,400);
}

void ofApp::calcFaceSprites() {
    if(!rec.isTrained()) return;

    for (int i=0; i<rec.numPeople(); i++) {
		ofImage masked;
		
        unsigned char* pixels = rec.getPersonPixels(i);        
		unsigned char* rgba_pixels = new unsigned char[4*PCA_WIDTH*PCA_HEIGHT];
        for(int x=0; x<PCA_WIDTH; x++)
            for(int y=0; y<PCA_HEIGHT; y++) { //hoeken?
                rgba_pixels[(x+(y*PCA_WIDTH))*4] = pixels[(x+(y*PCA_WIDTH))*3];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+1] = pixels[(x+(y*PCA_WIDTH))*3+1];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+2] = pixels[(x+(y*PCA_WIDTH))*3+2];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+3] = mask_pixels[x+y*PCA_WIDTH];
            }
        

		masked.setFromPixels(rgba_pixels, PCA_WIDTH, PCA_HEIGHT, OF_IMAGE_COLOR_ALPHA);
		
        faces.push_back(masked);

        delete rgba_pixels;
    };
}

void ofApp::update(){

	vidGrabber.update();
	
	if (vidGrabber.isFrameNew())
	{
		unsigned char * pixels = vidGrabber.getPixels();		
		
		cvimg.setFromPixels(pixels, camWidth, camHeight);
		img.setFromPixels(pixels, camWidth, camHeight, OF_IMAGE_COLOR, true);

		test_image.setFromPixels(pixels, camWidth, camHeight, OF_IMAGE_COLOR);
        test_image.resize(camWidth/TEST_DIV, camHeight/TEST_DIV);
        test_image.update();
        finder.findHaarObjects(test_image);
        
		//finder.findHaarObjects(img);
		    
		//canny edge stuff
		int minVal = 30;
		int maxVal = 80;
		
		cannygray.setFromPixels(pixels, camWidth, camHeight);
		cvCanny(cannygray.getCvImage(), canny.getCvImage(), minVal,  maxVal, 3);
		canny.flagImageChanged();		
	}

	//update particles
	for(unsigned int i = 0; i < p.size(); i++){
		//p[i].setMode(currentMode);
		p[i].update();
	}
	
	//lets add a bit of movement to the attract points
	for(unsigned int i = 0; i < attractPointsWithMovement.size(); i++){
		attractPointsWithMovement[i].x = attractPoints[i].x + ofSignedNoise(i * 10, ofGetElapsedTimef() * 0.7) * 12.0;
		attractPointsWithMovement[i].y = attractPoints[i].y + ofSignedNoise(i * -10, ofGetElapsedTimef() * 0.7) * 12.0;
	}	

}

void ofApp::draw(){
    // draw current video frame to screen (need to change img before drawing or after?)
	//img.draw(0, 0, camWidth*SCALE, camHeight*SCALE);

	cvimg.dilate();
	
	//do some linear blending stuff to make it look a little nicer maybe?
	double alpha = 0.4;
	double beta = ( 1.0 - alpha );
	
	Mat upper = cvimg.getCvImage();
	Mat lower = canny.getCvImage();
	Mat root;

	//this gives off an access error or whatever like a big idiot, no clue wtf is happening here
	//addWeighted(upper, alpha, lower, beta, 0.0, root);

	cvimg.draw(0, 0, camWidth*SCALE, camHeight*SCALE);	
	//canny.draw(0, 0, camWidth*SCALE, camHeight*SCALE);
	
	
	//hm ok
    int person = -1;

    std::ostringstream fr;
    std::ostringstream o;
	
	ofRectangle cur;

	for(int i = 0; i < finder.blobs.size(); i++) {
		
		cur = finder.blobs[i].boundingRect;

        cur.x*=TEST_DIV;
        cur.y*=TEST_DIV;
        cur.width*=TEST_DIV;
        cur.height*=TEST_DIV;

        int tx=cur.x;
        int ty=cur.y;
        int tw=cur.width;
        int th=cur.height;

        unsigned char *img_px = img.getPixels();
		unsigned char *temp = new unsigned char[tw*th*3];

		for (int x=0; x<tw; x++)
            for (int y=0; y<th; y++) {
                temp[(x+y*tw)*3] = img_px[((x+tx)+(y+ty)*camWidth)*3];
                temp[(x+y*tw)*3+1] = img_px[((x+tx)+(y+ty)*camWidth)*3+1];
                temp[(x+y*tw)*3+2] = img_px[((x+tx)+(y+ty)*camWidth)*3+2];
            }

        face.setFromPixels(temp, cur.width, cur.height, OF_IMAGE_COLOR);
        delete temp;

        face.resize(PCA_WIDTH, PCA_HEIGHT);
        //face.update();
        color = face.getPixels();
        gray = color;

        unsigned char *pixels = gray.getPixels();

        for(int x=0; x<PCA_WIDTH; x++)
            for(int y=0; y<PCA_HEIGHT; y++)
                if(mask.getPixels()[x+y*PCA_HEIGHT]<=0)
                    pixels[x+y*PCA_HEIGHT]=128;

        gray = pixels;

        person=rec.recognize(gray);

        ofSetColor(255, 255, 255, 192);

		//cvimg.erode();	
		
        // super-impose matched face over detected face
		faces[person].draw(cur.x*SCALE, cur.y*SCALE, cur.width*SCALE, cur.height*SCALE);	

        // reset color
        ofSetColor(255, 255, 255, 255);
	}
	
    float time = ofGetElapsedTimef();

	//only draw strings every now and then...
	float mod = RandomFloat(0.0,1.0);	
	if(fmod(time, 2.0) < mod) //fmod == modulo
	{	
		//get me some random stringature then
		std::string res = RandString.gen_random(1);
				
		//draw things 
		for(unsigned int i = 0; i < p.size(); i++){
			//flip coin first
			int coin = ofApp::coin();
			if(coin == 1)
			{
				std::string drawletter = RandString.gen_random(1);
				p[i].draw(drawletter, cur);
			}
		}		
	}

	
}

void ofApp::resetParticles(){

	//these are the attraction points used in the forth demo 
	attractPoints.clear();
	for(int i = 0; i < 4; i++){
		attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
	}
	
	attractPointsWithMovement = attractPoints;
	
	for(unsigned int i = 0; i < p.size(); i++){
		//p[i].setMode(currentMode);		
		p[i].setAttractPoints(&attractPointsWithMovement);;
		p[i].reset();
	}	
}

int ofApp::coin()
{
	int flip;
	flip = rand() % 2 + 1;// assign random numbers
				
	return (flip);
}

float ofApp::RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

/*
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
//this is a test
}
*/
