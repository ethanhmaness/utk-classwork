#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;

/* vflip.cpp
 * Ethan Maness
 * Sept 6, 2021
 *
 * Reads in a .pgm image file and outputs a corresponding file which is flipped vertically by reversing the order of the rows in the file.
 */

int main(){
	//delcare vars
	int tmp;
	string P2;
	int width;
	int height;
	string _255;
	int pixelNum = 0;
	vector< vector<int> > rows;

	//check formatting
	if(!(cin >> P2) || P2 != "P2"){
		cerr << "Bad PGM file -- first word is not P2" << endl;
		return 1;
	}
	//check valid width
	if(!(cin >> width) || width <= 0){
		cerr << "Bad PGM file -- No column specification" << endl;
		return 1;
	}
	//check valid height
	if(!(cin >> height) || height <= 0){
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

	//handle body of file
	for(int i = 0; i < height; i++){
		//new row
		vector<int> row;
		//pixel-by-pixel handling
		for(int c = 0; c < width; c++){
			//check out-of-range pixel
			if(pixelNum >= width * height){
				cerr << "Bad PGM file -- Extra stuff after the pixels" << endl;
				return 1;
			}
			//check vaid pixel
			if(!(cin >> tmp) || tmp > 255 || tmp < 0){
				cerr << "Bad PGM file -- pixel " << pixelNum << " is not a number between 0 and 255" << endl;
				return 1;
			}
			//add pixel to row
			row.push_back(tmp);
			//increment pixel count
			pixelNum++;
		}
		//add row to matrix
		rows.push_back(row);
	}
	//check for excess values
	if(cin >> tmp){
		cerr << "Bad PGM file -- Extra stuff after the pixels" << endl;
		return 1;
	}
	//output file, flipping row order
	for(int r = height-1; r >= 0; r--){
		for(int c = 0; c < width; c++){
			cout << rows.at(r).at(c) << endl;
		}
	}
	return 0;
}   
