#include "pgm.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

bool Pgm::Read(const std::string &file)
{
  ifstream fin;
  string s;
  size_t i, j, r, c, v;

  fin.open(file.c_str());
  if (fin.fail()) return false;
  
  if (!(fin >> s)) return false;
  if (s != "P2") return false;
  if (!(fin >> c) || c <= 0) return false;
  if (!(fin >> r) || r <= 0) return false;
  if (!(fin >> i) || i != 255) return false;

  Pixels.resize(r);
  for (i = 0; i < r; i++) {
    Pixels[i].clear();
    for (j = 0; j < c; j++) {
      if (!(fin >> v) || v > 255) return false;
      Pixels[i].push_back(v);
    }
  }
  if (fin >> s) return false;
  fin.close();
  return true;
}

bool Pgm::Write(const string& file) const{

	if(Pixels.size() == 0) return false;

	ofstream fo;

	fo.open(file.c_str());
	if(fo.fail()) return false;

	int rows = Pixels.size();
	int cols = Pixels.at(0).size();
	int x = 0;

	fo << "P2" << endl << cols << " " << rows << endl << 255 << endl;
	
	for(int i = 0; i < Pixels.size(); i++){
		for(int j = 0; j < Pixels.at(i).size(); j++){
			fo << Pixels.at(i).at(j);
			x++;
			if(x == 20){
				x = 0;
				fo << endl;
			} else if(i != Pixels.size() - 1 || j != Pixels.at(0).size() - 1) fo << " ";
			else fo << endl;
		}
	}
	
	fo.close();
	return true;
}

bool Pgm::Create(size_t r, size_t c, size_t pv){

	if(pv > 255) return false;

	Pixels.clear();
	vector<int> row;
	row.resize(c, pv);

	for(int y = 0; y < r; y++){
		Pixels.push_back(row);
	}

	return true;
}

bool Pgm::Clockwise(){

	vector< vector<int> > rtd;
	vector<int> row;

	for(int c = 0; c < Pixels.at(0).size(); c++){
		row.clear();
		for(int r = Pixels.size() - 1; r >= 0; r--){
			row.push_back(Pixels.at(r).at(c));
		}
		rtd.push_back(row);
	}

	Pixels = rtd;

	return true;
}

bool Pgm::Cclockwise(){
    
	vector< vector<int> > rtd;
	vector<int> row;

	for(int c = Pixels.at(0).size() - 1; c >= 0; c--){
		row.clear();
		for(int r = 0; r < Pixels.size(); r++){
			row.push_back(Pixels.at(r).at(c));
		}
		rtd.push_back(row);
	}

	Pixels = rtd;
	
	return true;
}

bool Pgm::Pad(size_t w, size_t pv){
    
	if(pv > 255) return false;

	vector< vector<int> > padded;
	vector<int> row;
	int newWidth = 2 * w + Pixels.at(0).size();

	for(int r = 0; r < w; r++){
		row.clear();
		for(int c = 0; c < newWidth; c++){
			row.push_back(pv);
		}
		padded.push_back(row);
	}

	for(int r = 0; r < Pixels.size(); r++){
		row.clear();
		for(int c = 0; c < w; c++){
			row.push_back(pv);
		}
		for(int c = 0; c < Pixels.at(0).size(); c++){
			row.push_back(Pixels.at(r).at(c));
		}
		for(int c = 0; c < w; c++){
			row.push_back(pv);
		}
		padded.push_back(row);
	}

	for(int r = 0; r < w; r++){
		row.clear();
		for(int c = 0; c < newWidth; c++){
		    row.push_back(pv);
	    }
	    padded.push_back(row);
	}
	
	Pixels = padded;

	return true;
}

bool Pgm::Panel(size_t r, size_t c){

	if(r == 0 || c == 0) return false;

	vector< vector<int> > tiled;
	vector<int> row;

	for(int i = 0; i < Pixels.size() * r; i++){
		row.clear();
		for(int j = 0; j < Pixels.at(0).size() * c; j++){
			row.push_back(Pixels.at(i % Pixels.size()).at(j % Pixels.at(0).size()));
		}
		tiled.push_back(row);
	}

	Pixels = tiled;
	
	return true;

}

bool Pgm::Crop(size_t r, size_t c, size_t rows, size_t cols){

	if(r + rows >= Pixels.size() || c + cols >= Pixels.at(0).size()) return false;

	vector< vector<int> > crop;
	vector<int> row;

	for(int i = r; i < r + rows; i++){
		row.clear();
		for(int j = c; j < c + cols; j++){
			row.push_back(Pixels.at(i).at(j));
		}
		crop.push_back(row);
	}
	
	Pixels = crop;

	return true;
}
