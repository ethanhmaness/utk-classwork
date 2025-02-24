#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <iomanip>

using namespace std;

class BlackWhiteRectangles {
  public:
    int blackCount(vector <string> rectangles);
};

class TypeRectangle{

  public:
    unsigned short x1;
    unsigned short x2;
    unsigned short y1;
    unsigned short y2;
    unsigned char type;

};

class PatternRectangle {

  public:
    unsigned short x1;
    unsigned short x2;
    unsigned short y1;
    unsigned short y2;
    unsigned char pattern; //last 4 bits represent 2x2 pattern, reading right-to-left, top-to-bottom

};

int BlackWhiteRectangles::blackCount(vector <string> R)
{

  stringstream ss;
  int tmp;
  set<unsigned short> xs; //all vertical edge x values
  set<unsigned short> ys; //all horizontal edge y values
  set<unsigned short>::iterator xsit;
  set<unsigned short>::iterator ysit;
  TypeRectangle tr;
  PatternRectangle pr;
  vector<TypeRectangle> trs; //overlapping rectangles given
  vector<PatternRectangle> prs; //disjoint rectangles constructed
  int count;
  int density;
  
  //read in all given rectangles
  for(size_t i = 0; i < R.size(); i++){
  
    //read rectangle string
    ss.clear();
    ss.str(R[i]);
    
    //construct a type rectangle from string params
    ss >> tr.x1;
    ss >> tr.y1;
    ss >> tr.x2;
    ss >> tr.y2;
    ss >> tmp;
		tr.type = tmp;
    
    //insert rect coords into x and y sets
    xs.insert(tr.x1);
    xs.insert(tr.x2);
    ys.insert(tr.y1);
    ys.insert(tr.y2);
    
    //push type rectangle into vector
    trs.push_back(tr);
  
  }
  
  //cout << "trs: " << trs.size() << " xs: " << xs.size() << " ys: " << ys.size() << endl;
  
  //Populate pattern rectangle vector with blank rectangles for every pairing of consecutive xs and consecutive ys
  for(xsit = xs.begin(); next(xsit) != xs.end(); xsit++){
    for(ysit = ys.begin(); next(ysit) != ys.end(); ysit++){
      
      pr.x1 = *xsit;
      pr.x2 = *next(xsit);
      pr.y1 = *ysit;
      pr.y2 = *next(ysit);
      pr.pattern = 0;
      
      prs.push_back(pr);
      
    }
  }
  
  //cout << "prs: " << prs.size() << endl;
  
  //Apply pattern from each TypeRectangle onto all contained PatternRectangles
  for(size_t t = 0; t < trs.size(); t++){
  
    tr = trs[t];
    
    for(size_t p = 0; p < prs.size(); p++){
    
      pr = prs[p];
      
      //Check that pr is contained within tr
      if(pr.x1 >= tr.x1 && pr.y1 >= tr.y1 && pr.x2 <= tr.x2 && pr.y2 <= tr.y2){
      
        //cout << "tr type " << (int)tr.type << endl;
        
        //Overlay pattern based on tr type
        switch((int)tr.type){
          case 1:
            //cout << "case 1" << endl;
            pr.pattern |= 15;
            break;
            
          case 2:
            //cout << "case 2" << endl;
            pr.pattern |= ((tr.y1 % 2 == 0) ? 12 : 3);
            break;
            
          case 3:
            //cout << "case 3" << endl;
            pr.pattern |= ((tr.x1 % 2 == 0) ? 5 : 10);
            break;
            
          case 4:
            //cout << "case 4" << endl;
            pr.pattern |= ((tr.x1 % 2 == tr.y1 % 2) ? 6 : 9);
            break;
          
        }
        
        //cout << "Applied tr " << t << " of type " << tr.type << " to pr " << p << ": pattern is now " << (int)pr.pattern << endl;
        
        prs[p] = pr;
      
      }
    
    }
  
  }
  
  //Calculate and sum up black square count of all pattern rectangles
  for(size_t i = 0; i < prs.size(); i++){
    
    pr = prs[i];
    
    //cout << pr.x1 << " " << pr.y1 << " " << pr.x2 << " " << pr.y2 << " " << (int)pr.pattern << endl;
    
    if(pr.pattern == 0) continue;
    
    //The rectangle is separated into 9 sub-rectangles by the least and greatest even x and y coordinates within its bounds.
    //These are found with (x1+1)/2 * 2, x2/2 * 2, (y1+1)/2 * 2, y2/2 * 2
    //All sub-rectangles can be nonexistant (zero-width)
    
    //central even-coordinates rectangle of 2x2s
    density = (((pr.pattern >> 0) & 1) + ((pr.pattern >> 1) & 1) + ((pr.pattern >> 2) & 1) + ((pr.pattern >> 3) & 1));
    count += ((pr.x2 / 2) - ((pr.x1 + 1) / 2)) * ((pr.y2 / 2) - ((pr.y1 + 1) / 2)) * density;
    
    //left even-y-coordinates sliver of 2x1s
    density = (((pr.pattern >> 2) & 1) + ((pr.pattern >> 0) & 1));
    count += (((pr.x1 + 1) / 2) * 2 - pr.x1) * ((pr.y2 / 2) - ((pr.y1 + 1) / 2)) * density;
    
    //right even-y-coordinates sliver of 2x1s
    density = (((pr.pattern >> 3) & 1) + ((pr.pattern >> 1) & 1));
    count += (pr.x2 - (pr.x2 / 2) * 2) * ((pr.y2 / 2) - ((pr.y1 + 1) / 2)) * density;
    
    //bottom even-x-coordinates sliver of 1x2s
    density = (((pr.pattern >> 2) & 1) + ((pr.pattern >> 3) & 1));
    count += (((pr.y1 + 1) / 2) * 2 - pr.y1) * ((pr.x2 / 2) - ((pr.x1 + 1) / 2)) * density;
    
    //top even-x-coordinates sliver of 1x2s
    density = (((pr.pattern >> 0) & 1) + ((pr.pattern >> 1) & 1));
    count += (pr.y2 - (pr.y2 / 2) * 2) * ((pr.x2 / 2) - ((pr.x1 + 1) / 2)) * density;
    
    //bottom left odd corner
    count += (((pr.y1 + 1) / 2) * 2 - pr.y1) * (((pr.x1 + 1) / 2) * 2 - pr.x1) * ((pr.pattern >> 2) & 1);
    
    //bottom right odd corner
    count += (((pr.y1 + 1) / 2) * 2 - pr.y1) * (pr.x2 - (pr.x2 / 2) * 2) * ((pr.pattern >> 3) & 1);
    
    //top left odd corner
    count += (pr.y2 - (pr.y2 / 2) * 2) * (((pr.x1 + 1) / 2) * 2 - pr.x1) * ((pr.pattern >> 0) & 1);
    
    //top right odd corner
    count += (pr.y2 - (pr.y2 / 2) * 2) * (pr.x2 - (pr.x2 / 2) * 2) * ((pr.pattern >> 1) & 1);
    
  }
  
  return count;

}
