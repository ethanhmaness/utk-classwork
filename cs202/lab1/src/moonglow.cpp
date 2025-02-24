#include <stdio.h>
#include <iostream>
using namespace std;

/* moonglow.cpp
 * Ethan Maness
 * 8-30-21
 *
 * This reads in from stdin, identifies a name, and resolves an exam score from a "moonglow formatted" assortment of words, and then prints out the name and score.
 */

int main(){

	//define vars
	string s;
	string name;
	double val;
	double score = 0;
	double avgTot;
	double avgNum;

	//main processing loop
	while(1){
		//if number, simply add to score
		if(cin >> val) score += val;
		//if eof, end loop
		else if (cin.eof()) break;
		//otherwise, check for keyword
		else {
			//clear error state from non-int read
			cin.clear();
			//get string
			cin >> s;
			//if NAME, read next value into name (unless eof, in which case break)
			if(s == "NAME"){
				if(!(cin >> name)) if(cin.eof()) break;
			//if AVERAGE...
			} else if (s == "AVERAGE"){
				//reset avg vars
				avgTot = 0;
				avgNum = 0;
				//while reading cin gives a number
				while(cin >> val){
					//add the number to a sum
					avgTot += val;
					//increment the number of numbers
					avgNum++;
				}
				//if there were any numbers, average and add to score
				if(avgNum != 0) score += (avgTot / avgNum);
				//if eof, break loop
				if(cin.eof()) break;
			}
		}
	}

	//output values
	cout << name << " " << score << endl;
	return 0;
}
