#include <cstdio>
#include <iostream>
using namespace std;

/* neg.cpp
 * Ethan Maness
 * Sept 6, 2021
 *
 * Reads in a .pgm image file and outputs a corresponding file with the "opposite" brightness value for each pixel, effectively inverting the image about the center of the grayscale.
 */

int main(){
	//declare vars
	int tmp;
	string P2;
	int width;
	int height;
	string _255;
	int pixelNum = 0;

	//check formatting
	if(!(cin >> P2) || P2 != "P2"){
		cerr << "Bad PGM file -- first word is not P2" << endl;
	    return 1;
	}
	//check valid width
	if(!(cin >> width) || width < 0){
		cerr << "Bad PGM file -- No column specification" << endl;
		return 1;
	}
	//check valid height
	if(!(cin >> height) || height < 0){
	    cerr << "Bad PGM file -- No row specification" << endl;
	    return 1;
	}
	//check formatting
	if(!(cin >> _255) || _255 != "255"){
		cerr << "Bad PGM file -- No 255 following the rows and columns" << endl;
	    return 1;
	}
	
	//output header
	cout << "P2" << endl;
	cout << width << " " << height << endl;
	cout << "255" << endl;

	//handle pixel body of file
	while(cin >> tmp){
		//check for pixel out of range
		if(pixelNum >= width * height){
			cerr << "Bad PGM file -- Extra stuff after the pixels" << endl;
			return 1;
		}
		//check for invalid pixel value
		if(tmp > 255 || tmp < 0){
			cerr << "Bad PGM file -- pixel " << pixelNum << " is not a number between 0 and 255" << endl;
			return 1;
		}
		//output inverted pixel
		cout << (255 - tmp) << endl;
		//increment pixel count
		pixelNum++;
	}

	return 0;

}   
