#include <cstdio>
#include <iostream>
using namespace std;

/* pgminfo.cpp
 * Ethan Maness
 * Sept 6, 2021
 *
 * Reads in a .pgm image file and outputs information regarding the size and pixels of the file.
 */

int main(){
	//define vars
	int tmp;
	string P2;
	int width;
	int height;
	string _255;
	int pixelNum = 0;
	int sum = 0;
	double avg;

	//check formatting
	if(!(cin >> P2) || P2 != "P2"){
		cerr << "Bad PGM file -- first word is not P2" << endl;
		return 1;
	}
	//check for valid cols
	if(!(cin >> width) || width < 0){
		cerr << "Bad PGM file -- No column specification" << endl;	
		return 1;
	}
	//check for valid rows
	if(!(cin >> height) || height < 0){
		cerr << "Bad PGM file -- No row specification" << endl;	
		return 1;
	}
	//check formatting
	if(!(cin >> _255) || _255 != "255"){
		cerr << "Bad PGM file -- No 255 following the rows and columns" << endl;
		return 1;
	}

	//read in a pixel at a time
	while(cin >> tmp){
		//error if pixel out of range
		if(pixelNum >= width * height){
			cerr << "Bad PGM file -- Extra stuff after the pixels" << endl;
			return 1;
		}
		//error if invalid pixel value
		if(tmp > 255 || tmp < 0){
			cerr << "Bad PGM file -- pixel " << pixelNum << " is not a number between 0 and 255" << endl;
			return 1;
		}
		//add to sum, increment pixel count
		sum += tmp;
		pixelNum++;
	}

	//compute average
	avg = sum / (double)(pixelNum);

	//print information
	printf("# Rows:   %9d\n", height);
	printf("# Columns:%9d\n", width);
	printf("# Pixels: %9d\n", width*height);
	printf("Avg Pixel:%9.3f\n", avg);

	return 0;
}
