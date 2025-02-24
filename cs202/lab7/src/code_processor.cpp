#include "code_processor.hpp"
#include <iostream>
#include <fstream>
using namespace std;

//returns djb hash value for a key
unsigned int djb_hash(const string &s)
{
	size_t i;
	unsigned int h;
	
	h = 5381;

	for (i = 0; i < s.size(); i++) {
		h = (h << 5) + h + s[i];
	}
	return h;
}

//Adds a new Prize to the Prizes map
bool Code_Processor::New_Prize(const string &id, const string &description, int points, int quantity){
	
	//check valid values (no non-positive ints, no duplicate prize ids)
	if(points <= 0 || quantity <= 0 || Prizes.find(id) != Prizes.end()) return false;
	
	//create new Prize with given values
	Prize* p = new Prize();
	p->id = id;
	p->description = description;
	p->points = points;
	p->quantity = quantity;

	//insert into set with key id
	Prizes.insert( pair<string, Prize*>(id, p));

	return true;
}

//Adds a new User to the Names map
bool Code_Processor::New_User(const string &username, const string &realname, int starting_points){
	
	//check valid values (no negative points, no duplicate usernames)
	if(starting_points < 0 || Names.find(username) != Names.end()) return false;

	//create new User with given values
	User* u = new User();
	u->username = username;
	u->realname = realname;
	u->points = starting_points;
	u->phone_numbers.clear();

	//insert into set with key username
	Names.insert( pair<string, User*>(username, u));

	return true;
}

//removes a User, and removes all phone numbers associated with the user
bool Code_Processor::Delete_User(const string &username){
	
	//find User associated with username
	map<string, User*>::const_iterator umit;
	set<string>::const_iterator sit;
	umit = Names.find(username);
	User* u;

	//check for user exists
	if(umit == Names.end()) return false;
	
	//remove user from Names map
	u = umit->second;
	Names.erase(u->username);

	//remove all User's phones from Phones map
	for(sit = u->phone_numbers.begin(); sit != u->phone_numbers.end(); sit++){
		Phones.erase(*sit);
	}

	//delete User
	delete u;

	return true;
}

//Adds a phone number to Phones which is associated with a given user
bool Code_Processor::Add_Phone(const string &username, const string &phone){
    
	//find user associated with username
	map<string, User*>::const_iterator mit;
	mit = Names.find(username);

	//check valid values (no duplicate phone, user must exist)
	if(mit == Names.end() || Phones.find(phone) != Phones.end()) return false;

	//insert User pointer into set with key phone
	Phones.insert( pair<string, User*>(phone, mit->second));
	
	//insert phone into User's phone_numbers set
	mit->second->phone_numbers.insert(phone);

	return true;
}

//removes phone number from Phones and from a User's phone_numbers set
bool Code_Processor::Remove_Phone(const string &username, const string &phone){

	//find associated pairs in Names and Phones
	map<string, User*>::const_iterator umit;
	map<string, User*>::const_iterator pmit;
	umit = Names.find(username);
	pmit = Phones.find(phone);

	//check valid values (user must exist, phone must be in system and associated with the given user)
	if(umit == Names.end() || pmit == Phones.end() || pmit->second != umit->second) return false;

	//remove phone pair from Phones, remove phone number from user
	Phones.erase(phone);
	umit->second->phone_numbers.erase(phone);

	return true;
}

//prints a User's phone numbers
string Code_Processor::Show_Phones(const string &username) const {
	
	//find user
	map<string, User*>::const_iterator mit;
	mit = Names.find(username);
	
	User* u;
	string s;
	set<string>::const_iterator sit;

	//check if valid user
	if(mit == Names.end()) return "BAD USER";
	
	//check if user has phone numbers
	u = mit->second;
	if(u->phone_numbers.size() == 0) return "";

	//for each phone number string in phone_numbers, add a line containing it to the string
	for(sit = u->phone_numbers.begin(); sit != u->phone_numbers.end(); sit++){
		s += *sit + "\n";
	}

	return s;
}

//validates and redeems points for a given code, and marks the code as used
int Code_Processor::Enter_Code(const string &username, const string &code){

	//find associated User
	map<string, User*>::const_iterator mit;
	mit = Names.find(username);

	//check valid values (code must not be used, User must exist)
	if(Codes.find(code) != Codes.end() || mit == Names.end()) return -1;

	unsigned int djb;
	int value;

	//find djb hash value of given code
	djb = djb_hash(code);
	if(djb % 17 == 0) value = 10;
	else if(djb % 13 == 0) value = 3;
	else return 0;

	//add points to User
	mit->second->points += value;

	//mark code as used
	Codes.insert(code);

	return value;

}

//finds a User associated with a given phone number and enters the code on their behalf
int Code_Processor::Text_Code(const string &phone, const string &code){
	
	//find phone in Phones
	map<string, User*>::const_iterator mit;
	mit = Phones.find(phone);

	//check that phone number has a user
	if(mit == Phones.end()) return -1;

	//call Enter_Code with username and return the same value
	return Enter_Code(mit->second->username, code); 
	
}

//marks a valid code as used (without actually needing to be used)
bool Code_Processor::Mark_Code_Used(const string &code){
	
	//check that code is unused
	if(Codes.find(code) != Codes.end()) return false;
	
	unsigned int djb;
	
	//find djb hash value of given code
	djb = djb_hash(code);
	if(djb % 17 != 0 && djb % 13 != 0) return false;

	//mark code as used
	Codes.insert(code);
	return true;
}

//Retreives the points total of a User associated with the given username, or -1 if not found
int Code_Processor::Balance(const string &username) const {
	
	map<string, User*>::const_iterator mit;

	//find pair with username as key
	mit = Names.find(username);
	//if no pair, return -1
	if(mit == Names.end()) return -1;
	//otherwise, return associated user's points
	return mit->second->points;
}

//Removes points from a user's balance and reduces prize quantity by 1
bool Code_Processor::Redeem_Prize(const string &username, const string &prize){
	
	//find User and Prize associated with given keys
	map<string, User*>::const_iterator umit;
	map<string, Prize*>::const_iterator pmit;
	umit = Names.find(username);
	pmit = Prizes.find(prize);

	//check valid values (user and prize must exist, user must have sufficient funds)
	if(umit == Names.end() || pmit == Prizes.end() || umit->second->points < pmit->second->points) return false;

	//subtract points from User balance, reduce remaining Prize quantity by 1
	umit->second->points -= pmit->second->points;
	pmit->second->quantity--;

	if(pmit->second->quantity == 0){ 
		Prize* p = pmit->second;	
		Prizes.erase(prize);
		delete p;
	}

	return true;
}

//deletes all user and prize memory
Code_Processor::~Code_Processor(){
	map<string, User*>::const_iterator umit;
	map<string, Prize*>::const_iterator pmit;

	//delete all Users in Names
	for(umit = Names.begin(); umit != Names.end(); umit++){
		delete umit->second;
	}

	//delete all Prizes in Prizes
	for(pmit = Prizes.begin(); pmit != Prizes.end(); pmit++){
		delete pmit->second;
	}
}

//Writes a list of commands to a file which allow for the recreation of a given server state, including all Users, Phones and Codes
bool Code_Processor::Write(const string &filename) const{
	
	ofstream fs;

	//open file, or return false if fail
	fs.open(filename.c_str());
	if(!(fs.is_open()))	return false;

	//init vars
	map<string, Prize*>::const_iterator pmit;
	map<string, User*>::const_iterator umit;
	set<string>::const_iterator sit;
	Prize* p;
	User* u;

	//write commands to add all prizes
	for(pmit = Prizes.begin(); pmit != Prizes.end(); pmit++){
		p = pmit->second;
		fs << "PRIZE " << p->id << " " << p->points << " " << p->quantity << " " << p->description << endl;
	}

	//write commands to add all users
	for(umit = Names.begin(); umit != Names.end(); umit++){
		u = umit->second;
		fs << "ADD_USER " << u->username << " " << u->points << " " << u->realname << endl;
	}

	//write commands to add all phone numbers
	for(umit = Phones.begin(); umit != Phones.end(); umit++){
		fs << "ADD_PHONE " << umit->second->username << " " << umit->first << endl;
	}

	//write commands to mark all codes as used
	for(sit = Codes.begin(); sit != Codes.end(); sit++){
		fs << "MARK_USED " << *sit << endl;
	}

	//close file
	fs.close();

	return true;
}
