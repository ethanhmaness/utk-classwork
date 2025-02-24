#include <iostream>
#include <sstream>
using namespace std;

/* enum.cpp
 * Ethan Maness
 * November 3, 2021
 *
 * This program takes a string length and number of ones (no greater than the string length) and generates every combination of 0s and 1s of the given length with the given number of ones, in order from least to greatest binary value.
 */

//Prints all strings of size equal to that of s, which are identical to s up to the index, then contain n_ones 1s afterwards, with the other characters as 0s
void do_enumeration(string &s, int index, int n_ones){
	//if index past end of string, just print
	if(index >= s.size()){
		cout << s << endl;
		return;
	}
	
	//copy string to mutable string
	string ns = s;
	
	//if there are more remaining chars than ones
	if(s.size() - index > n_ones){
		//set the char at index to 0
		ns.at(index) = '0';
		//run recursively at the next index
		do_enumeration(ns, index + 1, n_ones);
	}

	//if there are ones remaining
	if(n_ones > 0){
		//set the char at index to 1
		ns.at(index) = '1';
		//run recursively at the next index, with one less 1
		do_enumeration(ns, index + 1, n_ones - 1);
	}
}

int main(int argc, char** argv){
	//if not enough args
	if(argc < 3){
		cerr << "Usage: enum <length> <ones>" << endl;
		return 1;
	}
	
	stringstream ss;
	string s;
	int length;
	int ones;
	
	//read in length
	ss << string(argv[1]);
	//error check
	if(!(ss >> length)){
		cerr << "Invalid length \"" << length << "\"" << endl;
		return 1;
	}
	ss.clear();

	//read in ones
	ss << string(argv[2]);
	//error check (ones cannot be greater than length)
	if(!(ss >> ones) || ones > length){
		cerr << "Invalid ones \"" << ones << "\"" << endl;
		return 1;
	}
	ss.clear();

	//make s a string of length '-' chars
	s.resize(length, '-');

	//run the recursive method
	do_enumeration(s, 0, ones);
}
