#include "Particle.h"

//------------------------------------------------------------------
Particle::Particle(){
	attractPoints = NULL;
	
	ofTrueTypeFont::setGlobalDpi(72);

	linoFont.loadFont("linowrite.ttf", 72, true, true, true);	
	typeFont.loadFont("typewriter.ttf", 72, true, true, true);	
	atFont.loadFont("atwriter.ttf", 72, true, true, true);	
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
	
	//ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
	ofPoint attractPt(faceCurPos.x, faceCurPos.y);

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
void Particle::draw(std::string text, ofRectangle cur){
	
	//we need to update the attractor
	faceCurPos = cur;

	//random tint
	ofSetColor(ofRandom(25,255));

	//flip coin
	int coin = Particle::coin(3);
	if(coin == 1)
		atFont.drawString(text, pos.x, pos.y);	
	else if(coin == 2)
		typeFont.drawString(text, pos.x, pos.y);	
	else
		linoFont.drawString(text, pos.x, pos.y);	

}

int Particle::coin(int mod)
{
	int flip;
	flip = rand() % mod + 1;// assign random numbers
				
	return (flip);
}