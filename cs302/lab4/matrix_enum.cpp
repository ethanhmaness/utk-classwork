#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

/* 
  matrix_enum.cpp
  Ethan Maness
  March 6, 2022
  
  Generates all permutation matrices of a given size with extra elements in every possible position, and prints them.

*/

using namespace std;

class Matrix {
  public:
    int W;
    int E;
    int P;                      /* This is 'x' or 'h' */
    vector <int> Perm;          /* Permutation of 0 .. (W-1), for the 'X' elements. */
    vector <int> Non_X;         /* This is the row/col id of each of the non-X elements. */
    vector <int> E_ID;          /* This is the row/col id of the E elements */
    void Print();               /* Print the matrix defined by W, Perm and E_ID */
    void Permute(int index);    /* This is the recursive permuting method. */
    void Choose(int index, int ne);     /* This is the recursive n-choose-k method. */
};

//Prints the current matrix or a hexadecimal representation of it, depending on the P value
void Matrix::Print(){

  //Matrix element mode
  if(P == 'x'){
    
    //for every position
    for(int i = 0; i < W; i++){
      for(int j = 0; j < W; j++){
	      
	      //if Perm lists the current column for the current row, print X
	      if(Perm[i] == j) cout << "X";
	      //if E_ID contains this position id, print E
	      else if(find(E_ID.begin(), E_ID.end(), i * W + j) != E_ID.end()) cout << "E";
	      //otherwise nothing is here
	      else cout << ".";
        
	    }
      cout << endl;
    }
    cout << endl;
    
  //Hexademical rows mode
  } else {
    
    unsigned int h;
    
    for(int i = 0; i < W; i++){
    
      //reset bits
      h = 0;
      
      for(int j = 0; j < W; j++){
      
        //if this position has an X or E, set the corresponding bit to 1
        if(Perm[i] == j || find(E_ID.begin(), E_ID.end(), i * W + j) != E_ID.end()) h |= 1 << j;
      
      }
      
      //print hex int
      cout << hex << h << endl;
      
		}
    
    cout << endl;
    
  }
  
}

/* Prints every possible matrix given the W and E parameters by permuting the X's
   recursively, and for each permutating using the recursive Choose() function to
   generate every possible selection of spots for E's given the permutation's
	 selections for X positions */
void Matrix::Permute(int index){
  
  int i;
  int tmp;

  /* Base case -- we're done - perform n-choose-k enumeration */

  if (index == (int) Perm.size()) {
  
    //set Non_X to every position id not occupied by an X
    Non_X.clear();
    for(int r = 0; r < W; r++){
      for(int c = 0; c < W; c++){
        if(Perm[r] != c) Non_X.push_back(r * W + c);
      }
    }
    
    Choose(0, E);
    
    return;
    
  }

  /* Otherwise, for each element of the vector, swap it with the element
     in index, call GenPermutations() recursively on the remainder of the
     vector, and then swap it back. */
     
  for (i = index; i < (int) Perm.size(); i++) {

    tmp = Perm[i];         /* Swap people[index] with people[i] */
    Perm[i] = Perm[index];
    Perm[index] = tmp;

    Permute(index+1);

    tmp = Perm[i];          /* Swap back */
    Perm[i] = Perm[index];
    Perm[index] = tmp;
  }
  
}

/* Prints every possible matrix for the given number of E's and the current Perm
   vector by recursively calling itself to perform an n-choose-k enumeration of
   Non_X to generate all selections of E positions */
void Matrix::Choose(int index, int ne){

  /* Base case -- if there are no more E's to add, 
     print out the matrix and return */

  if (ne == 0) {
    Print();
    return;
  }

  /* This is a second base case -- if there are fewer E's left to add
     than there are positions left in the matrix, then it's impossible to finish,
     so simply return. */

  if (ne > (int) Non_X.size() - index) return;

  /* Now, put the position in Non_X[index] into E_ID, and call Choose()
     recursively.  Afterwards, take the position out of E_ID. */
  
  E_ID.push_back(Non_X[index]);
  Choose(index+1, ne-1);
  E_ID.pop_back();

  /* Finally, call Choose() recursively without putting
     Non_X[index] in E_ID. */

  Choose(index+1, ne);
}

int main(int argc, char** argv){

/*
    Initializing
*/

  //Check input
  if(argc != 4){
    cout << "Usage: matrix_enum.cpp W E x|h" << endl;
    return 1;
  }

  //Init vars
  istringstream iss("");
  
  int w;
  int e;
  char p;
  
  Matrix m;
  
  //Read in args
  iss.str(string(argv[1]));
  iss >> w;
  iss.clear();
  
  iss.str(string(argv[2]));
  iss >> e;
  iss.clear();
  
  iss.str(string(argv[3]));
  iss >> p;
  iss.clear();
  
  //Set fields to arg values
  m = Matrix();
  m.W = w;
  m.E = e;
  m.P = p;

  //Create the first matrix (X's along diagonal, E's in first available spaces)
  for(int i = 0; i < m.W; i++){
  
    //at row i, insert an X at index i
    m.Perm.push_back(i);
    
  }
  
/*
    Permuting and Printing
*/

  m.Permute(0);
  
  return 0;

}
