//lab2.cpp
//This lab determines whether any given interstate is a primary or auxiliary highway
//as well as the direction of travel and, if applicable, which primary highway it serves.
//Ethan H. Maness
//09/18/20

#include <iostream>
#include <string>
#include <limits>
using namespace std;

int main() {

	//init interstate num integer
	int num;

	//main loop
	do{

	//request value for interstate number
	cout << "Enter an interstate number: ";
	
	//keep reading until valid input
	while(!(cin >> num)) {

		cin.clear();

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		cout << "Invalid input, try again: ";
	}
	
	//exit if 0
	if(num == 0) return 0;

	//end if invalid integer (multiple of 100) is given
	if(num < 1 || num > 999 || num%100 == 0){

		cout << num << " is not a valid interstate number.\n";
		return 0;

	}

	//begin response line
	cout << "I-" << num << " is ";
		
	//determine primary or auxiliary, including which primary is served if auxiliary
	if(num <= 99) cout << "a primary interstate, going ";
	else cout << "an auxiliary highway, serving I-" << (num%100) << ", going ";

	//determine direction
	if(num % 2 == 0) cout << "east/west.\n";
	else cout << "north/south.\n";

	//just in case it hasn't already exited
	}while (num != 0);

	return 0;

}
