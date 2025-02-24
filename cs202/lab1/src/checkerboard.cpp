#include <stdio.h>
#include <iostream>
using namespace std;

/* checkerboard.cpp
 * Ethan Maness
 * 8-30-21
 * Given five stdin inputs (row count, column count, starting char, cycle size, square width), this program will print a generalized checkerboard of characters, provided that the inputs do not describe a char cycle exceeding the ASCII char size limit.
 */

//Read from cin into integer refence i, return true if successful and positive, false otherwise
bool readPosInt(int &i){
    if(!(cin >> i)){
        cerr << "usage: checkerboard  - stdin should contain R, C, SC, CS and W" << endl;
        return false;
    }
    return i > 0;
}

//Read from cin into char reference c, return true if successful, false otherwise
bool readChar(char &c){
    if(!(cin >> c)){
        cerr << "usage: checkerboard  - stdin should contain R, C, SC, CS and W" << endl;
        return false;
    }
    return true;
}

int main(){

	//Main vars declaration
	int rows;
	int cols;
	char sc;
	int cycle_len;
	int width;

	//Read in values for main vars from cin, end program w/ appropriate message if failure or improper values
	if(!readPosInt(rows)) return 0;
	if(!readPosInt(cols)) return 0;
	if(!readChar(sc)) return 0;
	if(!readPosInt(cycle_len)) return 0;
	if(!readPosInt(width)) return 0;

	if(sc + cycle_len > 127) return 0;

	//enable the following line if needing to test values
	//cout << rows << " " << cols << " " << sc << " " << cycle_len << " " << width << endl;

	//Print each "row"
	for(int r = 0; r < rows; r++){
		//Causes each row to be printed multiple times according to width
		for(int i = 0; i < width; i++){
			//Print each "column"
			for(int c = 0; c < cols; c++){
				//Causes each column char to be printed multiple times according to width
				for(int j = 0; j < width; j++){
					//formula to determine character
					cout << (char)((int)(sc + (r + c)%cycle_len));
				}
			}
			//go to next line after all columns have been printed for given line
			cout << endl;
		}
	}

	return 0;
}
