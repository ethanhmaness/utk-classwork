#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void CharReplace(string&,char,char);

int NumWords(const string&);

int NumNonWSCharacters(const string&);

char PrintMenu();

int main(){

	//declare vars
	string txt;
	char option;

	//get line of text
	cout << "Enter a line of text: ";
	getline(cin, txt);

	//output original line
	cout << "\nYou entered: " << txt;

	//main loop
	while(option != 'q' && !cin.eof()){

		cout << "\n\n";

		//get user option choice
		option = PrintMenu();

		cout << "\n";

		switch(option){
		
			//print number of words (separated by spaces)
			case 'w': cout << "Number of words: " << NumWords(txt);
			break;

			//print number of non-space characters
			case 'c': cout << "Number of non-whitespace characters: " << NumNonWSCharacters(txt);
			break;

			//change string by replacing one character with another
			case 'r':

				char find;
				char repl;

				cout << "Enter a character to find: ";
				cin >> find;

				cout << "Enter a character to replace: ";
				cin >> repl;

				CharReplace(txt, find, repl);

				cout << "\nNew string: " << txt;

			break;

		}

	}

	return 0;

}

//sets str to a string identical to the previous, except every instance of char o is replaced with char f
void CharReplace(string& str, char o, char f){

	for(unsigned int i = 0; i < str.size(); i++){

		if(str.at(i) == o) str.at(i) = f;

	}

}

int NumWords(const string& str){
	
	bool followsNonspace = false;
	int count = 0;

	//read chars
	for(unsigned int i = 0; i < str.size(); i++){

		//if space, increment if not following another space, and remember for next loop
		if(str.at(i) == ' '){

			if(followsNonspace) count++;
			followsNonspace = false;
		
		//if not space, remember for next loop
		} else followsNonspace = true;

	}

	//count last word even if no space after
	if(str.at(str.size() - 1) != ' ') count++;

	return count;

}

int NumNonWSCharacters(const string& str){

	int count = 0;

	//count number of non-spaces in stringstream
	for(unsigned int i = 0; i < str.size(); i++){

		if(str.at(i) != ' ') count++;

	}

	return count++;

}

char PrintMenu(){

	char c;

	cout << "Options\nw - Number of words\nc - Number of non-whitespace characters\nr - Replace a character\nq - Quit\n\nChoose an option: ";

	cin >> c;

	return c;

}
