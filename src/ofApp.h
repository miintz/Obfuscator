#pragma once

#include "Particle.h"
#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxCvFaceRec.h"
#include "RandomString.h"

#define SCALE 2
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
		
		int Displaying_Random_Text(cv::Mat image, char* window_name, cv::RNG rng);
		int coin();		
		float RandomFloat(float a, float b);

		ofTrueTypeFont font;

		ofxCvColorImage cvimg;
		ofxCvColorImage display;
		ofxCvGrayscaleImage cannygray;  
		ofxCvGrayscaleImage canny;  

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
        ofxCvColorImage color;
        ofxCvGrayscaleImage gray;

		vector <ofImage> faces;
		//ofShader shader; //Shader
		//ofFbo fbo; //Buffer for intermediate drawing
		//ofImage image;

		RandomString RandString;
		
		vector <Particle> p;		
		vector <ofPoint> attractPoints;
		vector <ofPoint> attractPointsWithMovement;

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
