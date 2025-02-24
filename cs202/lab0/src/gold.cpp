/* Ethan Maness
 * Lab 0
 * 8/25/21
 * Reads in a file of periods, dashes, and capital alphabetic characters representing locations and the corresponding amount of gold buried at each location, and outputs the total gold found.
 */

#include <iostream>
#include <fstream>

using namespace std;

//calculates amount of gold corresponding to a char
int calcGold(char c){
    //check if valid gold indicator char
    if(c >= 'A' && c <= 'Z'){
        //shift int char cast val to start with 'A' at 1
        return (int)(c - 'A') + 1;
    } else {
        //all other chars mean zero gold
        return 0;
    }
}

int main() {
	//vars
	char result;
	int gold;

	//read in char at a time
	while(cin >> result){
		//add char's gold to sum total
		gold += calcGold(result);
	}

	cout << gold << endl;

	return 0;
}
