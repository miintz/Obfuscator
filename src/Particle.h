#pragma once
#include "ofMain.h"
#include <string>

class Particle
{

	public:
		Particle();
		
		void setAttractPoints( vector <ofPoint> * attract );

		ofTrueTypeFont font;

		void reset();
		void update();
		void draw(std::string text);		
		
		ofPoint pos;
		ofPoint vel;
		ofPoint frc;
		
		float drag; 
		float uniqueVal;
		float scale;
		
		vector <ofPoint> * attractPoints; 
};