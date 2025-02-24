#include <vector>
#include <string>
#include "hash_202.hpp"
#include <sstream>
#include <cstdio>
using namespace std;

/* hash_202.cpp
 * Ethan Maness
 * September 21, 2021
 *
 * This program implements the Hash_202 class from the hash_202.hpp header file. This allows for the simulation of a hash table using Last7 and/or XOR hashing and Linear or Double collision resolution strategies, as well as the printing and efficiency-testing (via probe-counting) of the functions.
 */

//converts hex string to unsigned int (will not work on improperly formatted strings)
size_t IntFromHex(string hexStr){
	size_t x;
	stringstream ss;

	//read in as hex format
	ss << hex << hexStr;
	ss >> x;
	return x;
}

//returns the integer hash value from a given hex string using XOR hashing
size_t XOR(string hexStr){
	vector<size_t> nums;
	size_t result = 0;
	size_t i;

	//just in case
	nums.clear();

	//take 7-char slices and convert to an int, then push into nums vector
	for(i = 0; i < hexStr.size(); i += 7){
		nums.push_back(IntFromHex(hexStr.substr(i, min(7, static_cast<int>(hexStr.size() - i)))));
	}
	
	//repeatedly use binary xor on the elements on nums
	for(i = 0; i < nums.size(); i++){
		result ^= nums.at(i);
	}

	//return final binary as unsigned int
	return result;
}

//returns integer hash value from a given hex string using Last7 hashing
size_t Last7(string hexStr){
	string hex7;
	//if string is less than 7, use entire string
	if(hexStr.size() < 7) hex7 = hexStr;
	//otherwise select only last 7 hex digits
	else hex7 = hexStr.substr(hexStr.size() - 7);
	
	//convert to int from hex
	return IntFromHex(hex7);
}

//sets up hash table to be ready for use, accounting for possible error cases
string Hash_202::Set_Up(size_t table_size, const string &fxn, const string &collision){

	//check if already set up
	if(Keys.size() > 0 && Vals.size() > 0 && Fxn != 0 && Coll != 0) return "Hash table already set up";
	//table must have positive integer size (negatives already excluded w unsigned int size_t)
	if (table_size == 0) return "Bad table size";
	//must use one of the two available functions for collisions and hashing
	if (!(fxn == "Last7" || fxn == "XOR")) return "Bad hash function";
	if (!(collision == "Linear" || collision == "Double")) return "Bad collision resolution strategy";

	//Resize to correct vector of empty strings
	Keys.resize(table_size);
	Vals.resize(table_size);

	//set Fxn and Coll flags
	if(fxn == "Last7") Fxn = 'L';
	else if(fxn == "XOR") Fxn = 'X';
	if(collision == "Linear") Coll = 'L';
	else if (collision == "Double") Coll = 'D';
	
	//if no errors return empty string
	return "";
}

//adds a key-value pair to the hash table using the previously set function flags
string Hash_202::Add(const string &key, const string &val){

	size_t k;
	size_t k2;
	bool flag = false;

	//check for table set up
	if(Fxn == 0 || Coll == 0 || Keys.size() == 0 || Vals.size() == 0) return "Hash table not set up";
	//cannot perform function without a hex value
	if(key == "") return "Empty key";
	//checks if any chars are non-hex
	if(key.find_first_not_of("0123456789abcdef") != string::npos) return "Bad key (not all hex digits)";
	//check for an actual value to store
	if(val == "") return "Empty val";
	
	//check if table has open spots
	for(size_t i = 0; i < Keys.size(); i++){
		//minimize number of string operations for efficiency
		if(flag == true) continue;
		if(Keys.at(i) == "") flag = true;
	}
	if(!flag) return "Hash table full";

	//check function and perform appropriate one
	if(Fxn == 'X'){
		k = XOR(key);
		//set secondary hash value if Double hashing is used
		if(Coll == 'D') k2 = Last7(key);
	} else if(Fxn == 'L'){
		k = Last7(key);
		//set secondary hash value if Double hashing is used
		if(Coll == 'D') k2 = XOR(key);
	//just in case
	} else return "Cannot insert key";

	//set index to hash value (will be modulo'd later)
	size_t i = k;
	//!! cannot use double hash value of zero !!
	if(k2 % Keys.size() == 0) k2 = 1;

	//only run a number of times equal to the size of the table (to avoid infinite loops when unable to insert)
	for(size_t j = 0; j < Keys.size(); j++){
		//force index within correct bounds
		i %= Keys.size();
		//if empty, insert key-val pair
		if(Keys.at(i) == ""){
			Keys.at(i) = key;
			Vals.at(i) = val;
			return "";
		}
		//if match, the key already has been inserted
		if(Keys.at(i) == key) return "Key already in the table";
		
		//if linear, add 1
		if(Coll == 'L') i++;
		//if double, add k2 (will be modulo'd on next run)
		else if(Coll == 'D') i += k2;
		//just in case
		else return "Cannot insert key";

		//if returns to starting index, detect loop and send error
		if(i == k % Keys.size()) return "Cannot insert key";
	}

	//if runs through entire table, send error
	return "Cannot insert key";

}

//finds and returns a value given its key
string Hash_202::Find(const string &key){
	//reset Nprobes
	Nprobes = 0;
	//check for hex chars only
	if(key.find_first_not_of("0123456789abcdef") != string::npos) return "";
	//check that table exists
	if(Keys.size() == 0) return "";
	
	size_t k;
	size_t k2;

	//check function and use appropriate one, assign secondary hash value if necessary (when Double hashing)
	if(Fxn == 'X'){
		k = XOR(key);
		if(Coll == 'D') k2 = Last7(key);
	} else if(Fxn == 'L'){
		k = Last7(key);
		if(Coll == 'D') k2 = XOR(key);
	} else return "";

	//assign hash value as starting index
	size_t i = k;
	//do not use 0 as a double hash h2 value
	if(k2 % Keys.size() == 0) k2 = 1;

	//do not run more than the number of spaces in the table as this indicates infinite looping
	for(size_t j = 0; j < Keys.size(); j++){
		//force index within table bounds
		i %= Keys.size();
		
		//if no key return empty string
		if(Keys.at(i) == "") return "";
		//if key found, return value
		if(Keys.at(i) == key) return Vals.at(i);

		//if linear add 1
		if(Coll == 'L') i++;
		//if double add k2 (will be modulo'd next run)
		else if(Coll == 'D') i += k2;
		else return "";
		
		//if repeating, give up
		if(i == k % Keys.size()) return "";

		//add 1 to probe count
		Nprobes++;
	}

	//if exceeds allowable number of checks, give up
	return "";
}

//prints contents of hash table (does not print empty rows)
void Hash_202::Print() const {

	//for each row in table
	for(size_t i = 0; i < Keys.size(); i++){
		//if entry exists
		if(Keys.at(i) != ""){
			//print w/ appropriate formatting
			printf("%5lu %s %s\n", i, Keys.at(i).c_str(), Vals.at(i).c_str());
		}
	}

}

//calculate total number of probes for every element combined
size_t Hash_202::Total_Probes(){
	//check that table exists
	if(Keys.size() == 0) return 0;

	//start at 0
	size_t total = 0;

	//for every row in table
	for(size_t i = 0; i < Keys.size(); i++){
		//reset nprobes
		Nprobes = 0;
		//if row has entry
		if(Keys.at(i) != ""){
			//run find function to calc num of probes
			Find(Keys.at(i));
			//add to total
			total += Nprobes;
		}
	}
	//return sum total of all find() runs
	return total;
}
