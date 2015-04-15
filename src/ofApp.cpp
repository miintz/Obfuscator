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

/// Edge detection globals  
Mat srcEdge, srcEdge_gray;  
Mat dstEdge, detected_edges;  
   
int edgeThresh = 1;  
int lowThreshold;  
int const max_lowThreshold = 100;  
int ratio = 3;  
int kernel_size = 3;     
   
/// contour detection globals  
Mat srcContour; Mat srcContour_gray, drawing;  
int thresh = 100;  
int max_thresh = 255;  
RNG rng(12345);  
   
/// blend globals  
Mat output;  
double alpha = 0.5;   
double beta = ( 1.0 - alpha );  

//--------------------------------------------------------------
void ofApp::setup(){

	//soundPlayer
	facesound.loadSound("facesound2.wav");
	letters.loadSound("letters.wav");
	
	//shader
	//shader.load( "shaderVert.c", "shaderFrag.c" );
	//fbo.allocate( ofGetWidth(), ofGetHeight() );
    // SETUP VIDEO INPUT
    camWidth = CAM_WIDTH;
    camHeight = CAM_HEIGHT;
	vidGrabber.setVerbose(true);

	//init for ext. webcam for the prototype
	//vidGrabber.listDevices();
	//vidGrabber.setDeviceID(1);

	vidGrabber.initGrabber(camWidth,camHeight);
	videoMirror = new unsigned char[camWidth*camHeight*3];
	mirrorTexture.allocate(camWidth, camHeight, GL_RGB);
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

	cvimg.allocate(640, 480);
	cvfinal.allocate(640,480);
	display.allocate(640,400);
}

void ofApp::calcFaceSprites() {
    if(!rec.isTrained()) return;

    for (int i=0; i<rec.numPeople(); i++) {
		ofImage masked;
		
        unsigned char* pixels = rec.getPersonPixels(i);        
		unsigned char* rgba_pixels = new unsigned char[4*PCA_WIDTH*PCA_HEIGHT];
        for(int x=0; x<PCA_WIDTH; x++)
            for(int y=0; y<PCA_HEIGHT; y++) { //masking stuff
                rgba_pixels[(x+(y*PCA_WIDTH))*4] = pixels[(x+(y*PCA_WIDTH))*3];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+1] = pixels[(x+(y*PCA_WIDTH))*3+1];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+2] = pixels[(x+(y*PCA_WIDTH))*3+2];
                rgba_pixels[(x+(y*PCA_WIDTH))*4+3] = mask_pixels[x+y*PCA_WIDTH];
            }
			
		masked.setFromPixels(rgba_pixels, PCA_WIDTH, PCA_HEIGHT, OF_IMAGE_COLOR_ALPHA);
		//now we have pixel data



		//openCV is bgr but our pixel array is in rgb
		ofxCvColorImage convmasked;		
		convmasked.allocate(150,150);
		convmasked.setFromPixels(masked.getPixels(), PCA_WIDTH, PCA_HEIGHT);
		convmasked.convertRgbToHsv();


		ofxCvColorImage recmasked;
		recmasked.allocate(150,150);
		
		Mat recmaskedmat;		
		Mat convmaskedmat = convmasked.getCvImage();

		cv::cvtColor(convmaskedmat, recmaskedmat, CV_RGBA2BGRA); 

		rgb2bgr_inter = recmaskedmat;
		recmasked = &rgb2bgr_inter;

		//now we should be able to get the improved pixels..
		unsigned char * newBgrPixels = convmasked.getPixels();

        //faces.push_back(masked);

		ofxCvColorImage cvmasked;		
		cvmasked.setFromPixels(pixels, 150, 150);
		
		//now we can mess these images up a little, lets create 3 versions
		//convert frame to Mat object used for edge detection  
		srcEdge = cv::cvarrToMat(cvmasked.getCvImage());  
        
		//convert frame to Mat object used for contour detection  
		srcContour = cv::cvarrToMat(cvmasked.getCvImage());  
		//contour detection  
		
		//convert image to gray and blur it  
		cvtColor( srcContour, srcContour_gray, CV_BGR2GRAY );  
		blur( srcContour_gray, srcContour_gray, cv::Size(3,3) );  
		
		ofApp::thresh_callback( 0, 0 );  
		
		//edge detection  
		//create a matrix of the same type and size as src (for dst)  
		dstEdge.create( srcEdge.size(), srcEdge.type() );  
   
		//Convert the image to grayscale  
		cvtColor( srcEdge, srcEdge_gray, CV_BGR2GRAY );  
   
		//show image
		CannyThreshold(0, 0);      
    
		//add these mat objects to other lists to create a little bit more chaos
		colors_inter = drawing;
		ofxCvColorImage cvcolors;
		cvcolors.allocate(150,150);		

		cvcolors = &colors_inter;

		cvfaces_colors.push_back(cvcolors);

		edges_inter = dstEdge;
		ofxCvColorImage cvedges;
		cvedges.allocate(150,150);		

		cvedges = &edges_inter;
		
		cvfaces_edges.push_back(cvedges);

		// we have two Mat objects to blend - dstEdge and drawing, this shouldnt crash now i think...
		addWeighted( dstEdge, alpha, drawing, beta, 0.0, output);  		
		
		//huzzah now we can mask this basterd				
		blended_inter = output;
		ofxCvColorImage cvblended;
		cvblended.allocate(150,150);

		cvblended = &blended_inter;

		cvfaces_blended.push_back(cvblended);

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
	}

	if (vidGrabber.isFrameNew()) {
    unsigned char * pixels = vidGrabber.getPixels();
    for (int i = 0; i < camHeight; i++) {
        for (int j = 0; j < camWidth*3; j+=3) {
            // pixel number
            int pix1 = (i*camWidth*3) + j;
            int pix2 = (i*camWidth*3) + (j+1);
            int pix3 = (i*camWidth*3) + (j+2);
            // mirror pixel number
            int mir1 = (i*camWidth*3)+1 * (camWidth*3 - j-3);
            int mir2 = (i*camWidth*3)+1 * (camWidth*3 - j-2);
            int mir3 = (i*camWidth*3)+1 * (camWidth*3 - j-1);
            // swap pixels
            videoMirror[pix1] = pixels[mir1];
            videoMirror[pix2] = pixels[mir2];
            videoMirror[pix3] = pixels[mir3];	
        }
    }
    mirrorTexture.loadData(videoMirror, camWidth, camHeight, GL_RGB);	
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

/** @function thresh_callback */  
 void ofApp::thresh_callback(int, void* )  
 {  
	Mat canny_output;  
	vector<vector<cv::Point> > contours;  
	vector<Vec4i> hierarchy;  
   
	/// Detect edges using canny  
	Canny( srcContour_gray, canny_output, thresh, thresh*2, 3 );  
	/// Find contours  
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );  
   
	/// Draw contours  
	drawing = Mat::zeros( canny_output.size(), CV_8UC3 );  
	for( int i = 0; i< contours.size(); i++ )  
	{  
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );  
		drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );  
	}  
 }  
   
 void ofApp::CannyThreshold(int, void*)  
 {  
	/// Reduce noise with a kernel 3x3  
	blur( srcEdge_gray, detected_edges, cv::Size(3,3) );  
   
	/// Canny detector - number has replaced lowThreshold  
	Canny( detected_edges, detected_edges, 50, lowThreshold*ratio, kernel_size );  
   
	/// Using Canny's output as a mask, we display our result  
	dstEdge = Scalar::all(0);  
   
	srcEdge.copyTo( dstEdge, detected_edges);  
   
 }  

void ofApp::draw(){
    // draw current video frame to screen (need to change img before drawing or after?)

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

		//lets go fucking mental, lets go fucking mental!
		int distortcoin = ofApp::coin(4);
		int facecoin = ofApp::coin(3); 

		if(distortcoin == 1)
			cvimg.erode();
		else if(distortcoin == 2)
			cvimg.dilate();
		else if(distortcoin == 3)
			cvimg.blurGaussian(3); //odd numbers here
		else if(distortcoin == 4)
			cvimg.contrastStretch();
		
		//lalaa lalala, lalaa lalala
		if(facecoin == 1)		
			cvfaces_blended[person].draw(cur.x*SCALE, cur.y*SCALE, cur.width*SCALE, cur.height*SCALE);			
		else if(facecoin == 2)
			cvfaces_colors[person].draw(cur.x*SCALE, cur.y*SCALE, cur.width*SCALE, cur.height*SCALE);		
		else				
			cvfaces_edges[person].draw(cur.x*SCALE, cur.y*SCALE, cur.width*SCALE, cur.height*SCALE);
			

		cvimg.draw(0, 0, camWidth*SCALE, camHeight*SCALE);	

		mirrorTexture.draw(camWidth, 0, camWidth, camHeight);

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
		letters.play();
		
		//draw things 
		for(unsigned int i = 0; i < p.size(); i++){
			//flip coin first
			int coin = ofApp::coin(2);
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

int ofApp::coin(int mod)
{
	int flip;
	flip = rand() % mod + 1;// assign random numbers
				
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
