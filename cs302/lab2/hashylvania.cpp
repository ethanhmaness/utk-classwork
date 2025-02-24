#include<string>
#include<iostream>
#include<map>
#include<vector>
#include<algorithm>
#include<sstream>

/*
	hashylvania.cpp
	Feb 13, 2022
	Ethan Maness
*/

using namespace std;

//Hash function
unsigned long long jsp_hash(const string &s)
{
  size_t i;
  unsigned long long h, t;

  h = 0x6bd300f29f1505ULL;

  for (i = 0; i < s.size(); i++) {
    t = h & 0x3ffffffffffffffULL;
    h = (t << 5) ^ h ^ s[i];
  }
  return h;
}

//Each Person has a name and id, represented by this object
class Person {
  public:
    string name;
    string id;
};

int main(){

  string str;
  string hexh;
  stringstream ss;
  Person* person;
  Person* first;
  Person* last;
  vector<Person*> population;
  unsigned long long index;
  unsigned long long h;
  unsigned long long hash;
  unsigned long long modulus;
  
  //pair of hash and associated person
  pair<unsigned long long, Person*> hashAssociation;
  //points to a map containing hashes and the associated person
  map<unsigned long long, Person*>* hashMapPtr;
  //Pairs of the form (number, map<hash, person>) such that hash modulo the modulus equals the number
  map<unsigned long long, map<unsigned long long, Person*>* > modMap;
  
  map<unsigned long long, map<unsigned long long, Person*>* >::iterator modMapIterator;
  map<unsigned long long, map<unsigned long long, Person*>* >::iterator modMapIterator2;
  map<unsigned long long, Person*>::iterator hashMapIterator;

  //Constantly read in strings until none remain
  while(cin >> str){
    
    //"PERSON": add Person object to population vector
    if(str == "PERSON"){
      
      person = new Person();
      cin >> person->name;
      cin >> person->id;
      population.push_back(person);
    
    }
    
    //"ORDER": Generate the random ordering of people, print first and last
    if(str == "ORDER"){
    
      first = NULL;
      last = NULL;
    
      //clear the map so it is ready for re-use
      modMap.clear();
    
      cin >> str;
      cin >> modulus;
      
      //calculate the hash and hash%modulus for all people and populate the modMap
      for(size_t i = 0; i < population.size(); i++){
        
        person = population.at(i);
        
        hash = jsp_hash(person->name + person->id + str);
        
        modMap.insert( pair<unsigned long long, map<unsigned long long, Person*>* >( hash % modulus, new map<unsigned long long, Person*>() ) );
        modMap.at(hash % modulus)->insert( pair<unsigned long long, Person*>(hash, person) );
        
      }
      
      //h = hash of random string
      h = jsp_hash(str);
      
      while(modMap.size() != 0){
      
	      //pick the group of people whose hash % modulus is closest to h % modulus (biased towards greater if tied)
        
        modMapIterator = modMap.lower_bound(h % modulus);
        modMapIterator2 = modMapIterator; //greater than or equal to h % modulus
        modMapIterator--; //smaller than h % modulus
        
        //if no larger than h % modulus, pick the next largest
        if(modMapIterator2 == modMap.end()) index = modMapIterator->first;
        //if the smallest, pick it automatically
        else if(modMapIterator2 == modMap.begin()) index = modMapIterator2->first;
        else{
          //pick the closer one
          if(h % modulus - modMapIterator->first < modMapIterator2->first - (h % modulus)) index = modMapIterator->first;
          else index = modMapIterator2->first;
        }
        
        hashMapPtr = modMap.at(index);
        
        if(hashMapPtr->size() == 0) continue;
        
        //pick person with smallest hash >= h, or the largest if none such exists
        if(hashMapPtr->lower_bound(h) != hashMapPtr->end()) hashAssociation = *(hashMapPtr->lower_bound(h));
        else hashAssociation = *((hashMapPtr->rbegin())++);
        
        person = hashAssociation.second;
        
        //if this is the first person picked, store them
        if(first == NULL) first = person;
        //store into last no matter what
        last = person;
        
        //remove them from map
        hashMapPtr->erase(hashAssociation.first);
        
        //if map empty, remove it from main map
        if(hashMapPtr->size() == 0) modMap.erase(index);
        
        ss.str(string());
        ss << hex << h;
        
        h = jsp_hash(person->id + ss.str());
      
      }
      
      cout << "Last: " << last->name << " " << last->id << endl;
      cout << "First: " << first->name << " " << first->id << endl;
     
     
      //free up memory from modMap
      
			for(modMapIterator = modMap.begin(); modMapIterator != modMap.end(); modMapIterator++){
        
        hashMapPtr = modMapIterator->second;
        for(hashMapIterator = hashMapPtr->begin(); hashMapIterator != hashMapPtr->end(); hashMapIterator++){
        
          delete hashMapIterator->second;
        
        }
        
        delete hashMapPtr;
        
      }
			
      
    }
    
  }
  
  //free up memory from population vector
  for(size_t i = 0; i < population.size(); i++){
  
    //delete population.at(i);
  
  }
  
  delete person;
  
  return 0;

}
