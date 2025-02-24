#include <sstream>
#include "RadioRange.cpp"

int main(int argc, char **argv)
{
  int i;
  istringstream ss;
  string line;
  class RadioRange TheClass;
  double retval;
  vector <int> X;
  vector <int> Y;
  vector <int> R;
  int Z;

  if (argc != 2) { fprintf(stderr, "usage: a.out num or -\n"); exit(1); }

  if ((string) argv[1] == "-") {
    if (!getline(cin, line)) return 1;
    ss.clear(); ss.str(line); while (ss >> i) X.push_back(i);
    if (!getline(cin, line)) return 1;
    ss.clear(); ss.str(line); while (ss >> i) Y.push_back(i);
    if (!getline(cin, line)) return 1;
    ss.clear(); ss.str(line); while (ss >> i) R.push_back(i);
    cin >> Z;
  } else {

    if (atoi(argv[1]) == 0) {
      X.push_back(0);
      Y.push_back(0);
      R.push_back(5);
      Z = 10;
    }
  
    if (atoi(argv[1]) == 1) {
      X.push_back(0);
      Y.push_back(0);
      R.push_back(10);
      Z = 10;
    }
  
    if (atoi(argv[1]) == 2) {
      X.push_back(10);
      Y.push_back(10);
      R.push_back(10);
      Z = 10;
    }
  
    if (atoi(argv[1]) == 3) {
      X.push_back(11);
      X.push_back(-11);
      X.push_back(0);
      X.push_back(0);
      Y.push_back(0);
      Y.push_back(0);
      Y.push_back(11);
      Y.push_back(-11);
      R.push_back(10);
      R.push_back(10);
      R.push_back(10);
      R.push_back(10);
      Z = 31;
    }
  
    if (atoi(argv[1]) == 4) {
      X.push_back(100);
      Y.push_back(100);
      R.push_back(1);
      Z = 10;
    }
  
    if (atoi(argv[1]) == 5) {
      X.push_back(1000000000);
      Y.push_back(1000000000);
      R.push_back(1000000000);
      Z = 1000000000;
    }
  
    if (atoi(argv[1]) == 6) {
      X.push_back(20);
      X.push_back(-20);
      X.push_back(0);
      X.push_back(0);
      Y.push_back(0);
      Y.push_back(0);
      Y.push_back(20);
      Y.push_back(-20);
      R.push_back(50);
      R.push_back(50);
      R.push_back(50);
      R.push_back(50);
      Z = 100;
    }
  
    if (atoi(argv[1]) == 7) {
      X.push_back(0);
      X.push_back(-60);
      X.push_back(-62);
      X.push_back(-60);
      X.push_back(63);
      X.push_back(-97);
      Y.push_back(-72);
      Y.push_back(67);
      Y.push_back(61);
      Y.push_back(-8);
      Y.push_back(-32);
      Y.push_back(89);
      R.push_back(6);
      R.push_back(7);
      R.push_back(8);
      R.push_back(7);
      R.push_back(5);
      R.push_back(6);
      Z = 918;
    }
  }

  retval = TheClass.RadiusProbability(X, Y, R, Z);
  cout << retval << endl;

  exit(0);
}
