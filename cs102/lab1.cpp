// lab1.cpp
// This lab prompts the uer for a name and greets them.
// Ethan H. Maness
// 9/4/20

#include <iostream>
#include <string>

using namespace std;

int main(){

	//init all vars
	string name;
	int number;
	string streetname;
	string type;
	int lastmonth;
	int thismonth;

	//prompt user for name
	cout << "What is your name? ";
	cin >> name;

	//greet user
	cout << "Hi there, " << name << "." << " \n";

	//Get address
	cout << "Enter the property street number, name, and type: ";
	cin >> number >> streetname >> type;

	//Get property values
	cout << "Last month's property value: $";
	cin >> lastmonth;
	cout << "This month's property value: $";
	cin >> thismonth;

	//Print results
	cout << "\n";
	cout << "This property is located at " << number << " " << streetname << " " << type << ".\n";
	cout << "Its value has changed by $" << (thismonth - lastmonth) << " since last month.\n";
	cout << "The estimated monthly mortgage is $" << (thismonth * 0.050) / 12 << ".\n";
	cout << "Last month's estimated monthly mortgage was $" << (lastmonth * 0.050) / 12 << ".\n";
	cout << "The average monthly mortgage is $" << (thismonth + lastmonth) * 0.050 / 24 << ".\n";

	return 0;

}
