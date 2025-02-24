#include <iostream>
#include <string>
#include <vector>
#include "BlackWhiteRectangles.cpp"
using namespace std;

int main(int argc, char **argv)
{
  int i;
  class BlackWhiteRectangles TheClass;
  int retval;
  string l;
  vector <string> rectangles;

  if (argc != 2) { fprintf(stderr, "usage: a.out num or -\n"); exit(1); }

  if ((string) argv[1] == "-") {
    while (getline(cin, l)) rectangles.push_back(l);
  } else {

    if (atoi(argv[1]) == 0) {
      rectangles.push_back("1 1 6 8 1");
      rectangles.push_back("11 1 16 8 2");
      rectangles.push_back("1 11 6 18 3");
      rectangles.push_back("11 11 16 18 4");
    }
  
    if (atoi(argv[1]) == 1) {
      rectangles.push_back("1 1 11 11 1");
      rectangles.push_back("1 1 11 11 2");
      rectangles.push_back("9 9 19 19 3");
      rectangles.push_back("9 9 19 19 4");
    }
  
    if (atoi(argv[1]) == 2) {
      rectangles.push_back("1 6 9 9 1");
      rectangles.push_back("1 9 7 10 1");
      rectangles.push_back("1 5 8 7 1");
      rectangles.push_back("2 6 8 8 1");
      rectangles.push_back("2 1 3 4 1");
    }
  
    if (atoi(argv[1]) == 3) {
      rectangles.push_back("7 5 9 6 4");
      rectangles.push_back("6 3 9 6 4");
      rectangles.push_back("8 3 9 8 4");
      rectangles.push_back("5 9 10 10 4");
      rectangles.push_back("5 3 8 5 4");
    }
  
    if (atoi(argv[1]) == 4) {
      rectangles.push_back("7 1 10 3 1");
      rectangles.push_back("8 2 9 7 4");
      rectangles.push_back("5 3 7 4 3");
      rectangles.push_back("4 3 9 4 2");
      rectangles.push_back("3 1 9 2 1");
    }

    if (atoi(argv[1]) == 5) {
      rectangles.push_back("1 12 7 16 3");
      rectangles.push_back("2 10 7 13 3");
      rectangles.push_back("2 2 10 11 4");
    }
  }

  retval = TheClass.blackCount(rectangles);
  cout << retval << endl;

  exit(0);
}
