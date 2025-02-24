#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include "bitmatrix.hpp"
using namespace std;

/* bitmatrix.cpp
 * Ethan Maness
 * October 4, 2021
 *
 * This program implements functions from Bitmatrix.hpp to allow for the creation, editing, writing, and reading of bitmatrices as well as temporarily storing them in a hash table and performing row sums, matrix products, and matrix inversions.
 */

//constructs empty rows x cols matrix
Bitmatrix::Bitmatrix(int rows, int cols)
{
	//check valid size
	if(rows <= 0) throw string("Bad rows");
	if(cols <= 0) throw string("Bad cols");
	//sets M to a vector of cols-length strings consisting of only '0's
	string s = "";
	for(int i = 0; i < cols; i++) s += "0";
	M.resize(rows, s);
}

//reads a matrix from a text file
Bitmatrix::Bitmatrix(const string &fn)
{
	ifstream fs;
	string s;
	string tmp;

	//open file and check if opened
	fs.open(fn.c_str());
	if(!(fs.is_open()))	throw string("Can't open file");
	//read in lines from file
	while(getline(fs, s)){
		//break when end of file reached
        if(fs.eof()) break;
		//skip if empty
		if(s == "") continue;
		//remove spaces and breaks
		tmp = "";
		for(size_t c = 0; c < s.size(); c++){
			if(s.at(c) != '\n' && s.at(c) != ' ') tmp += s.at(c);
		}
		s = tmp;
		//if any line has anything but 0 or 1, throw error
		for(size_t i = 0; i < s.size(); i++){
			if(s.at(i) != '0' && s.at(i) != '1'){
				fs.close();
				throw string("Bad file format");
			}
		}
		//add line to matrix
		M.push_back(s);
	}

	fs.close();
}

//creates a matrix identical to the current matrix
Bitmatrix *Bitmatrix::Copy() const
{
	//make new matrix
	Bitmatrix* bm = new Bitmatrix(1,1);
	//copy matrix vector
	bm->M = M;
	//return pointer
	return bm;
}

//writes current matrix to a file
bool Bitmatrix::Write(const string &fn) const
{
	//open file, check for failure
	ofstream fs;
	fs.open(fn.c_str());
	if(!(fs.is_open())) return false;

	//write each row of chars
	for(size_t i = 0; i < M.size(); i++){
		fs << M.at(i) << "\n";
	}
	
	//close file
	fs.close();
	return true;
}

//prints current matrix to stdout
void Bitmatrix::Print(size_t w) const
{
	if(w == 0) w = (size_t)-1;
	//for each row
	for(size_t r = 0; r < M.size(); r++){
		//blank line every w lines
		if(r != 0 && r % w == 0) cout << "\n";
		//for each column
		for(size_t c = 0; c < M.at(0).size(); c++){
			//space every w columns
			if(c != 0 && c % w == 0) cout << " ";
			//print the bit
			cout << M.at(r).at(c);
		}
		cout << "\n";
	}
}

//writes PGM file depicting the current matrix with the option for border and size adjustment
bool Bitmatrix::PGM(const string &fn, int p, int border) const
{
	if(p < 0 || border < 0) return false;

	//open file, check for failure
	ofstream fs;
	fs.open(fn.c_str());
	if(!(fs.is_open())) return false;

	//determine dimensions of pgm file
	size_t pgmCols = M.at(0).size() * (p + border) + border;
	size_t pgmRows = M.size() * (p + border) + border;

	//write header
	fs << "P2" << endl;
	fs << pgmCols << " " << pgmRows << endl;
	fs << "255" << endl;

	//top border
	for(int b = 0; b < border; b++){
		for(size_t c = 0; c < pgmCols; c++){
			//space between zeroes
			if(c != 0) fs << " ";
			//black pixel
			fs << "0";
		}
		fs << endl;
	}
	//for each row of bits
	for(size_t r = 0; r < M.size(); r++){
		//for each layer of pixels
		for(int y = 0; y < p; y++){
			//left border
			for(int b = 0; b < border; b++){
				//black pixel
				fs << "0 ";
			}
			//for each bit in row
			for(size_t c = 0; c < M.at(0).size(); c++){
				//output val as per bit val
				for(int x = 0; x < p; x++){
					//pixel val dependent on bit
					fs << ((M.at(r).at(c) == '1') ? "100 " : "255 ");
				}
				//between-pixel border
				for(int b = 0; b < border; b++){
					//black pixel
					fs << "0 ";
				}
			}
			fs << endl;
		}
		//between-row border
		for(int b = 0; b < border; b++){
			for(size_t c = 0; c < pgmCols; c++){
				//space between zeroes
				if(c != 0) fs << " ";
				//black pixel
				fs << "0";
			}
			fs << endl;
		}
	}
	//close file
	fs.close();

	return true;
}

//returns matrix rows
int Bitmatrix::Rows() const
{
  return M.size();
}

//returns matrix columns
int Bitmatrix::Cols() const
{
  return M.at(0).size();
}

//returns bit at given coordinates
char Bitmatrix::Val(int row, int col) const
{
	//if invalid coords return x
	if(row < 0 || col < 0 || row >= M.size() || col >= M.at(0).size()) return 'x';
	//read char at row, col
	return M.at(row).at(col);
}

//changes bit at given coordinates to given value
bool Bitmatrix::Set(int row, int col, char val)
{
	//check invalid char
	if(val != '0' && val != '1') return false;
	//check invalid pos
	if(row < 0 || col < 0 || row >= M.size() || col >= M.at(0).size()) return false;
	//set char at row, col
	M.at(row).at(col) = val;
	return true;
}

//swaps specified rows of matrix
bool Bitmatrix::Swap_Rows(int r1, int r2)
{
	//check invalid rows
	if(r1 < 0 || r2 < 0 || r1 >= M.size() || r2 >= M.size()) return false;
	//put r1 in temp var
	string rt = M.at(r1);
	//put r2 in r1
	M.at(r1) = M.at(r2);
	//put old r1 val in r2
	M.at(r2) = rt;
	return true;
}

//adds second row to first row (in-place)
bool Bitmatrix::R1_Plus_Equals_R2(int r1, int r2)
{
	//check invalid rows
	if(r1 < 0 || r2 < 0 || r1 >= M.size() || r2 >= M.size()) return false;

	//for each column
	for(size_t i = 0; i < M.at(0).size(); i++){
		//XOR each bit
		if(M.at(r1).at(i) != M.at(r2).at(i)) M.at(r1).at(i) = '1';
		else M.at(r1).at(i) = '0'; 
	}
	return true;
}

//adds two matrices together, returning pointer to sum matrix
Bitmatrix *Sum(const Bitmatrix *a1, const Bitmatrix *a2)
{
	//check if summable (same dims)
	if(a1->Rows() != a2->Rows() || a1->Cols() != a2->Cols()) return NULL;
	//new matrix pointer
	Bitmatrix* sum = new Bitmatrix(a1->Rows(), a1->Cols());
	//for each bit
	for(int r = 0; r < a1->Rows(); r++){
		for(int c = 0; c < a1->Cols(); c++){
			//check if valid, then set new matrix to the xor (aka bit sum) at that location
			if(!(sum->Set(r, c, (a1->Val(r,c) == a2->Val(r,c) ? '0' : '1') ))) return NULL;
		}
	}
	return sum;
}

//multiplies two matrices, returning pointer to sum matrix
Bitmatrix *Product(const Bitmatrix *a1, const Bitmatrix *a2)
{
	bool tmp;
	//check if compatible
	if(a1->Cols() != a2->Rows()) return NULL;
	//create new bitmatrix pointer
	Bitmatrix* pdt = new Bitmatrix(a1->Rows(), a2->Cols());
	//for each bit in new matrix
	for(int r = 0; r < pdt->Rows(); r++){
		for(int c = 0; c < pdt->Cols(); c++){
			//perform bit sum (xor) of bit products (and)
			tmp = false;
			for(int i = 0; i < a1->Cols(); i++){
				tmp = tmp != (a1->Val(r,i) == '1' && a2->Val(i,c) == '1');
			}
			//set bit to appropriate character
			pdt->Set(r, c, (tmp) ? '1' : '0');
		}
	}
	return pdt;
}

//creates matrix using only specified rows of given matrix
Bitmatrix *Sub_Matrix(const Bitmatrix *a1, const vector <int> &rows)
{
	//check if empty rows array
	if(rows.size() == 0) return NULL;
	//new matrix w correct dimensions
	Bitmatrix* sub = new Bitmatrix(rows.size(), a1->Cols());
	//for each bit
	for(size_t r = 0; r < rows.size(); r++){
		for(int c = 0; c < a1->Cols(); c++){
			//set the bit to the proper value, or return NULL if failed
			if(!(sub->Set(r, c, a1->Val(rows.at(r), c)))) return NULL;
		}
	}
	//return pointer to new matrix
	return sub;
}

//returns inverse of given matrix, if possible
Bitmatrix *Inverse(const Bitmatrix *m)
{
	//must be square
	if(m->Rows() != m->Cols()) return NULL;

	//copy of original
	Bitmatrix* og = m->Copy();
	//identity matrix
	Bitmatrix* im = new Bitmatrix(m->Rows(), m->Cols());

	int swprow;

	//setup identity matrix
	for(int i = 0; i < im->Rows(); i++){
		im->Set(i,i,'1');
	}

	//transform to upper triangular (also operating on identity)
	for(int r = 0; r < og->Rows(); r++){
		
		//if diagonal pixel is not 1
		if(og->Val(r, r) != '1'){
			//if last row, matrix is singular
			if(r == og->Rows() - 1) return NULL;
			//otherwise, try to find row to swap
			swprow = -1;
			for(int i = r + 1; i < og->Rows(); i++){
				if(og->Val(i,r) == '1'){
					swprow = i;
					break;
				}
			}
			//if no row to swap matrix is singular
			if(swprow == -1) return NULL;
			//swap rows
			og->Swap_Rows(r, swprow);
			im->Swap_Rows(r, swprow);
		}

		//if last row, done
		if(r == og->Rows() - 1) break;

		//add to any lower rows with a 1 in column r
		for(int i = r + 1; i < og->Rows(); i++){
			//check if 1 in column r
			if(og->Val(i,r) == '1'){
				//add rows
				og->R1_Plus_Equals_R2(i, r);
				im->R1_Plus_Equals_R2(i, r);
			}
		}
	}

	//clear upper triangle by adding rows from bottom up
	for(int r = og->Rows() - 2; r >= 0; r--){
		//go rightward from middle diagonal
		for(int i = r + 1; i < og->Rows(); i++){
			//if 1
			if(og->Val(r,i) == '1'){
				//remove by adding proper row
				og->R1_Plus_Equals_R2(r, i);
				im->R1_Plus_Equals_R2(r, i);
			}
		}
	}

	//return inverted matrix
	return im;

}

//djb hash function
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

//creates separate chaining hash table of given size
BM_Hash::BM_Hash(int size)
{
	//must have size > 0
	if(size <= 0) throw string("Bad size");
	//set number of "chains" to size
	Table.resize(size);
}

//stores entry in table
bool BM_Hash::Store(const string &key, Bitmatrix *bm)
{
	//must be given a key
	if(key == "") return false;
	
	//key int from key string through hash func
	unsigned int keyInt = djb_hash(key);

	//check for key already present in chain
	for(int i = 0; i < Table.at(keyInt % Table.size()).size(); i++){
		if(Table.at(keyInt % Table.size()).at(i).key == key) return false;
	}

	//define table entry
	HTE entry;
	entry.key = key;
	entry.bm = bm;

	//append to chain at appropriate index
	Table.at(keyInt % Table.size()).push_back(entry);

	return true;
}

//recalls entry from table
Bitmatrix *BM_Hash::Recall(const string &key) const
{
	//must be given a key
	if(key == "") return false;
	
	//key int from key string through hash func
	unsigned int keyInt = djb_hash(key);

	//pick the chain to which the key maps
	vector<HTE> chain = Table.at(keyInt % Table.size());
	
	//grab the HTE
	HTE entry;
	bool flag = false;
	for(int i = 0; i < chain.size(); i++){
		if(chain.at(i).key == key){
			entry = chain.at(i);
			flag = true;
			break;
		}
	}

	if(!flag) return NULL;

	//return its bitmatrix pointer
	return entry.bm;
}

//returns vector of all entries in the table
vector <HTE> BM_Hash::All() const
{
	vector <HTE> rv;
	vector <HTE> cv;

	//append all HTEs in all chains to the rv vector
	for(int i = 0; i < Table.size(); i++){
		cv = Table.at(i);
		for(int j = 0; j < cv.size(); j++){
			rv.push_back(cv.at(j));
		}
	}

	return rv;
}
