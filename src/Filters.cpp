#include "Filters.h"
#include "cv.h"


#define filterWidth 5 
#define filterHeight 5 //geen ; hier, is geen expressie

#define imageWidth 150 
#define imageHeight 150

//declare image buffers 
ofColor image[imageWidth][imageHeight]; 
ofColor result[imageWidth][imageHeight];

double ExcessiveSharpenFilter[filterWidth][filterHeight] = 
{    
     1,  1,  1,
     1, -7,  1,
     1,  1,  1
};

double ExcessiveSharpenFilterFactor = 1.0;
double ExcessiveSharpenFilterBias = 0.0;

double SimpleFEFilter[filterWidth][filterHeight] =
{
    -1, -1, -1, -1, -1,
    -1,  2,  2,  2, -1,
    -1,  2,  8,  2, -1,
    -1,  2,  2,  2, -1,
    -1, -1, -1, -1, -1,
};

double SimpleFEFilterFactor = 1.0 / 8.0;
double SimpleFEFilterBias = 0.0;

Filters::Filters()
{
}

ofImage Filters::ApplyFilter(ofImage currentFrame, int mode)
{
	//nu moet ik de kleuren pakken
	
	int w = 150;
	int h = 150;

	for(int x = 0; x < w; x++)
	for(int y = 0; y < h; y++)
		image[x][y] = currentFrame.getColor(x,y);

	int factor = SimpleFEFilterFactor;
	int bias = SimpleFEFilterBias;

	//int factor = ExcessiveSharpenFilterFactor;
	//int bias = ExcessiveSharpenFilterBias;

	for(int x = 0; x < w; x++) 
    for(int y = 0; y < h; y++) 
    { 
        double red = 0.0, green = 0.0, blue = 0.0; 
        
		if(image[x][y].r != 128 && image[x][y].g != 128 && image[x][y].b != 128)
		{
			//multiply every value of the filter with corresponding image pixel unless its the gray border color
			for(int filterX = 0; filterX < filterWidth; filterX++) 
			for(int filterY = 0; filterY < filterHeight; filterY++) 
			{ 
				int imageX = (x - filterWidth / 2 + filterX + w) % w; 
				int imageY = (y - filterHeight / 2 + filterY + h) % h; 

				red += image[imageX][imageY].r * SimpleFEFilter[filterX][filterY]; 
				green += image[imageX][imageY].g * SimpleFEFilter[filterX][filterY]; 
				blue += image[imageX][imageY].b * SimpleFEFilter[filterX][filterY]; 
		
			} 
		
			//truncate values smaller than zero and larger than 255 
			result[x][y].r = std::min(std::max(int(factor * red + bias), 0), 255); 
			result[x][y].g = std::min(std::max(int(factor * green + bias), 0), 255); 
			result[x][y].b = std::min(std::max(int(factor * blue + bias), 0), 255);
		}
		else
		{ //set it to gray, else the alpha will break
			result[x][y].r = 128;
			result[x][y].g = 128;
			result[x][y].b = 128;
		}		
    }    
	
	//now set colors TODO merge with loop above, we dont really need the result array
	for(int x = 0; x < w; x++)
	for(int y = 0; y < h; y++)
		currentFrame.setColor(x, y, result[x][y]);
	
	return currentFrame;    
}