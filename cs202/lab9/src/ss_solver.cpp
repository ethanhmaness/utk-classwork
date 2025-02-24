#include "shifter.hpp"
#include <iostream>
#include <sstream>

/* ss_solver.cpp
 * Ethan Maness
 * November 9, 2021
 *
 * This program takes in a ShapeShifter grid as command-line arguments, and shapes through standard input, as separate words on the same line for each.  It prints out the turns to take in order to solve the ShapeShifter puzzle, if it is solvable, using recursion to test every possible set of turns until it finds a correct one.
 */

using namespace std;

//reads the grid in from standard input, with each argument as a row
//then reads in one shape per line of standard input, with each word as a row
bool Shifter::Read_Grid_And_Shapes(int argc, const char** argv){
	//fail if no grid given
	if(argc == 1) return false;
	
	//read in grid rows
	for(int i = 1; i < argc; i++){
		//push row into grid
		Grid.push_back(string(argv[i]));
		//fail if row length is inconsistent
		if(Grid[i-1].size() != Grid[0].size()) return false;
	}
	
	string l, s;
	vector<string> shape;
	stringstream ss;

	//read in shapes
	while(getline(cin, l)){
		//break at end of file
		if(cin.eof()) break;

		shape.clear();
		ss << l;
		//read in each row from the line
		while(ss >> s){
			//skip empty string
			if(s == "") continue;
			//push s into shape vector
			shape.push_back(s);
			//fail if row length is inconsistent
			if(shape.at(0).size() != s.size()) return false;
		}
		ss.clear();

		//push completed shape into Shapes
		Shapes.push_back(shape);
	}

	return true;
}

//applies a given shape to the grid, with the top left corner of the shape at r, c
void Shifter::Apply_Shape(int index, int r, int c){
	
	vector<string> shape = Shapes.at(index);

	//do nothing if position is invalid
	if(shape.size() + r > Grid.size() || shape.at(0).size() + c > Grid.at(0).size()) return;

	//for each row from the start to end of the shape application region
	for(size_t i = r; i < r + shape.size(); i++){
		//for each column of the region
		for(size_t j = c; j < c + shape.at(0).size(); j++){
			//if shape has '1' at pos
			if(shape.at(i-r).at(j-c) == '1'){
				//flip the character
				if(Grid.at(i).at(j) == '0') Grid.at(i).at(j) = '1';
				else Grid.at(i).at(j) = '0';
			}
		}
	}

	return;
}

//determines if the current board can be solved with the shapes in Shapes at index and beyond
//if index is outside of the range of Shapes, simply returns if the board is already solved
bool Shifter::Find_Solution(int index){

	//if no more shapes left
	if(index >= Shapes.size()){
		//check if grid has any zeroes
		for(size_t i = 0; i < Grid.size(); i++){
			for(size_t j = 0; j < Grid.at(0).size(); j++){
				//if a zero is found, this is not the solution
				if(Grid.at(i).at(j) != '1'){
					return false;
				}
			}
		}
		//otherwise, the solution is found
		return true;
	}

	//if valid shape index
	vector<string> shape = Shapes.at(index);
	//for each valid placement of the shape
	for(size_t i = 0; i <= Grid.size() - shape.size(); i++){
		for(size_t j = 0; j <= Grid.at(0).size() - shape.at(0).size(); j++){
					
			//apply the shape
			Apply_Shape(index, i, j);
			//if this placement cannot solve the grid, undo
			if(!Find_Solution(index + 1)){
				Apply_Shape(index, i, j);
			//if it does solve the grid, store the placement pos
			//note, upon completion these will be in reverse order
			} else {
				Solution_Rows.push_back(i);
				Solution_Cols.push_back(j);
				return true;	
			}
		}
	}

	//if this is reached, the puzzle is unsolvable
	return false;

}

//prints the solution as a turn on each line, describing the shape and placement position as:
//<shape rows> <top row> <left col>
void Shifter::Print_Solution() const {
	
	//for each shape
	for(size_t s = 0; s < Shapes.size(); s++){
		//print each row of the shape
		for(size_t r = 0; r < Shapes.at(s).size(); r++){
			cout << Shapes.at(s).at(r) << " ";
		}
		//print the placement position
		cout << Solution_Rows.at(Shapes.size() - 1 - s) << " " << Solution_Cols.at(Shapes.size() - 1 - s) << endl;
	}
}

int main(int argc, const char** argv){
	
	Shifter game;

	//instantiates the games with the grid and shapes
	game.Read_Grid_And_Shapes(argc, argv);
	
	//finds the solution, if it exists, and prints it
	if(game.Find_Solution(0)) game.Print_Solution();

	return 0;
}
