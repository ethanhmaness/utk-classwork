/*  Ethan Maness
    TA: Gregory "Steve Wozniak" Croisdale
    2/4/21
    Allows user to create, manipulate, and read int sized sets of bits.
*/

#include <vector>
#include <string>
#include <iostream>

using namespace std;

//global const declaration
const int SET_SIZE = sizeof(int) * 8;
const unsigned int SPACING = 4;

//class for handling the bitset
class BITSET {
    private:
        //the actual set of bits
        vector<int> bits;
    public:
        //constructor
        BITSET();

        //mutators
        void Set(unsigned int i);
        void Clear(unsigned int i);

        //accessors
        bool Test(unsigned int i) const;
        unsigned int GetNumSets() const {return bits.size();}
        int GetSet(unsigned int i) const;
};

string ToBinary(int val, unsigned int spacing);

int main(){

    //var declaration
    BITSET bitset;
    char option;
    string str;
    int val;
    unsigned int param;

    //input loop
    while(!cin.eof() && option != 'q'){

        //prompt
        cout << "Enter command: ";

        //get input
        getline(cin,str);

        //option = command char
        option = str.front();

        //str = parameter
        if(str.length() > 1) val = stoi(str.substr(2));

        //skip if negative value
        if(val < 0){
            cout << "Invalid parameter\n";
            option = ' ';
        } else param = val;

        //execute the command
        switch(option){

            //test bit
            case 't':
                cout << "Bit " << param << " is " << int(bitset.Test(param)) << "\n";
                break;

            //set bit
            case 's':
                bitset.Set(param);
                cout << "Set bit " << param << " to 1\n";
                break;
            
            //clear bit
            case 'c':
                bitset.Clear(param);
                cout << "Cleared bit " << param << " to 0\n";
                break;
            
            //get set
            case 'g':
                cout << "Set " << param << " is " << ToBinary(bitset.GetSet(param),SPACING) << "\n";
                break;
            
            //num sets
            case 'n':
                cout << "There are currently " << bitset.GetNumSets() << " sets\n";
                break;
            
            //quit
            case 'q':
                return 0;
                break;
            
            default:
                break;
        }

    }

    return 0;
}

//reads a full set of bits as a string with spacing
string ToBinary(int val, unsigned int spacing){

    //vars
    int n = val;
    string str = "";

    //start at most significant bit and add chars to the right
    for(int i = 0; i < SET_SIZE; i++){

        //"price is right"
        if(n / (1<<(SET_SIZE - 1 - i)) > 0){

            //reduce remaining value
            n -= 1<<(SET_SIZE - 1 - i);

            //append digit
            str += "1";
        } else str += "0";
        
        //append space if applicable
        if((i+1) % spacing == 0 && (i+1) != SET_SIZE) str += " ";
    }

    return str;

}

//construct BITSET object w/ a single empty int
BITSET::BITSET(){
    
    //start with one set
    bits.resize(1,0);

}

//test if a specific bit is 0 or 1
bool BITSET::Test(unsigned int i) const {

    //determine which int to use
    int set = GetSet(i / SET_SIZE);

    //determine position of bit in int
    unsigned int index = i % SET_SIZE;

    //create mask for appropriate position
    int mask = 1 << index;

    //apply mask
    set = set & mask;
    
    //true if any non-zero value, false if zero
    return bool(set);
}

//set a specific bit to 1
void BITSET::Set(unsigned int i){
    
    unsigned int setindex = i / SET_SIZE;

    //determine which int to use
    int set = GetSet(setindex);

    //determine position of bit in int
    unsigned int index = i % SET_SIZE;

    //create mask for appropriate position
    int mask = 1 << index;

    //apply mask
    set = set | mask;

    //update vector
    if(GetNumSets() <= setindex) bits.resize(setindex + 1, 0);
    bits.at(setindex) = set;

}

//clear a specific bit to 0
void BITSET::Clear(unsigned int i){
    
    unsigned int setindex = i / SET_SIZE;
    
    //determine which int to use
    int set = GetSet(setindex);
    
    //determine position of bit in int
    unsigned int index = i % SET_SIZE;

    //create mask for appropriate position
    int mask = ~(1 << index);

    //apply mask
    set = set & mask;

    //update vector
    if(GetNumSets() <= setindex) bits.resize(setindex + 1, 0);
    bits.at(setindex) = set;

    //if vector ends in zero set(s), remove them
    while(GetNumSets() > 1){
        if(bits.back() == 0) bits.pop_back();
        else break;
    }

}

//get the int value of a set
int BITSET::GetSet(unsigned int i) const {
    
    //if the set exists, return it. if not, return 0
    if(bits.size() > i) return bits.at(i);
    else return 0;

}