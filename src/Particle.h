#pragma once
#include "ofMain.h"
#include <string>

class Particle
{
	public:
		Particle();
		
		void setAttractPoints( vector <ofPoint> * attract );

		ofTrueTypeFont linoFont;
		ofTrueTypeFont typeFont;
		ofTrueTypeFont atFont;

		void reset();
		void update();
		void draw(std::string text, ofRectangle cur);		
		int coin(int mod);
		ofPoint pos;
		ofPoint vel;
		ofPoint frc;
		
		float drag; 
		float uniqueVal;
		float scale;
		
		vector <ofPoint> * attractPoints; 
	
	private:
		ofRectangle faceCurPos;
};