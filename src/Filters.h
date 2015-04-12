#pragma once

#include <string>
#include <iostream>
#include "ofMain.h"

class Filters 
{
	public:
		Filters();

		ofImage ApplyFilter(ofImage currentFrame, int mode); //moet dit iets returnen? een aangepast plaatje neem ik aan		
};