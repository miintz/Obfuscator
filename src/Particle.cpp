#include "Particle.h"

//------------------------------------------------------------------
Particle::Particle(){
	attractPoints = NULL;
	
	ofTrueTypeFont::setGlobalDpi(72);

	font.loadFont("molot.ttf", 32);	
	font.setLineHeight(18.0f);
	font.setLetterSpacing(1.037);
}


//------------------------------------------------------------------
void Particle::setAttractPoints( vector <ofPoint> * attract ){
	attractPoints = attract;
}

//------------------------------------------------------------------
void Particle::reset(){
	//the unique val allows us to set properties slightly differently for each particle
	uniqueVal = ofRandom(-10000, 10000);
	
	pos.x = ofRandomWidth();
	pos.y = ofRandomHeight();
	
	vel.x = ofRandom(-3.9, 3.9);
	vel.y = ofRandom(-3.9, 3.9);
	
	frc   = ofPoint(0,0,0);
	
	scale = ofRandom(0.5, 1.0);
	
	//if( mode == PARTICLE_MODE_NOISE ){
	//	drag  = ofRandom(0.97, 0.99);
//		vel.y = fabs(vel.y) * 3.0; //make the particles all be going down
	//}else{
		drag  = ofRandom(0.95, 0.998);	
//	}
}

//------------------------------------------------------------------
void Particle::update(){

	//1 - APPLY THE FORCES BASED ON WHICH MODE WE ARE IN 
		
		ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
		frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point 
		
		vel *= drag; //apply drag
		vel += frc * 0.6; //apply force
	
	
	
	//2 - UPDATE OUR POSITION
	
	pos += vel; 
	
	
	//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
	//we could also pass in bounds to check - or alternatively do this at the ofApp level
	if( pos.x > ofGetWidth() ){
		pos.x = ofGetWidth();
		vel.x *= -1.0;
	}else if( pos.x < 0 ){
		pos.x = 0;
		vel.x *= -1.0;
	}
	if( pos.y > ofGetHeight() ){
		pos.y = ofGetHeight();
		vel.y *= -1.0;
	}
	else if( pos.y < 0 ){
		pos.y = 0;
		vel.y *= -1.0;
	}	
		
}

//------------------------------------------------------------------
void Particle::draw(std::string text){
	
	//ofSetColor(255, 63, 180);			
	//ofCircle(pos.x, pos.y, scale * 4.0);
	font.drawString(text, pos.x, pos.y);

}

