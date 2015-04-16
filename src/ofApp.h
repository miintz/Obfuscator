#pragma once

#include "Particle.h"
#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxCvFaceRec.h"
#include "RandomString.h"

#define SCALE 1
#define TEST_DIV 2
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

#include <string>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();	
		void keyReleased  (int key);
		
		void resetParticles();
        void calcFaceSprites();		
		
		void CannyThreshold(int, void*);  
		void thresh_callback(int, void* );  
		
		int Displaying_Random_Text(cv::Mat image, char* window_name, cv::RNG rng);
		
		int coin(int mod);			

		float RandomFloat(float a, float b);

		ofTrueTypeFont font;
	
		ofxCvColorImage cvimg;
		ofxCvColorImage display;

		ofImage img;
		ofImage test_image;
		ofImage bgImage;
		ofImage mask;

		unsigned char *mask_pixels;

		ofxCvHaarFinder finder;
        ofxCvFaceRec rec;

        ofVideoGrabber vidGrabber;
		ofTexture videoTexture;

		int camWidth;
		int camHeight;
		int person;

		ofImage face;
		
		IplImage blended_interHalf;
		IplImage blended_interThreeQ;
		IplImage blended_interFull;		
		IplImage colors_inter;
		IplImage edges_inter;
		IplImage rgb2bgr_inter;		
		
        ofxCvColorImage color;
		ofxCvColorImage cvfinal;		
        ofxCvGrayscaleImage gray;

		vector <ofImage> faces;
		vector <ofxCvColorImage> cvfaces_colors;
		vector <ofxCvColorImage> cvfaces_edges;
		vector <ofxCvColorImage> cvfaces_blendedHalf;
		vector <ofxCvColorImage> cvfaces_blendedThreeQ;
		vector <ofxCvColorImage> cvfaces_blendedFull;

		//ofShader shader; //Shader
		//ofFbo fbo; //Buffer for intermediate drawing
		//ofImage image;

		RandomString RandString;

		cv::Mat outputHalf;  
		cv::Mat outputThreeQ;  
		cv::Mat outputFull;  

		vector <Particle> p;		
		vector <ofPoint> attractPoints;
		vector <ofPoint> attractPointsWithMovement;		

		ofSoundPlayer facesound;
		ofSoundPlayer letters;

		ofTexture mirrorTexture;
		unsigned char * videoMirror;

    private:
        // vars to toggle onscreen display
        bool showEigens;
        bool showFaces;
        bool showExtracted;
        bool showTest;
        bool showLeastSq;
        bool bgSubtract;
        bool showClock;
};
