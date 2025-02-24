#include <cstdio>
#include <iostream>
#include <sstream>
using namespace std;

/* bigwhite.cpp
 * Ethan Maness
 * Sept 6, 2021
 *
 * Takes a width and height from command-line arguments, and outputs a valid .pgm image file of entirely white pixels according to the input arguments.
 */

int main(int argc, char** argv){
	//declare vars
	int rows;
	int cols;
	istringstream ss1;
	istringstream ss2;
	
	//check args
	if(argc != 3){
		cerr << "usage: bigwhite rows cols" << endl;
		return 1;
	}

	//pass args into stringstreams
	ss1.str(argv[1]);
	ss2.str(argv[2]);

	//read args into ints, or detect error otherwise
	if(!(ss1 >> rows) || !(ss2 >> cols) || rows <= 0 || cols <= 0){
		cerr << "usage: bigwhite rows cols" << endl;
		return 1;
	}

	//output header
	cout << "P2" << endl;
	cout << cols << " " << rows << endl;
	cout << "255" << endl;
	
	//print white rectangle
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			cout << "255 ";
		}
		cout << endl;
	}

	return 0;
}
