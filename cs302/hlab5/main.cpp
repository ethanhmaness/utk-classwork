#include "BearCries.cpp"

int main(int argc, char **argv)
{
  int i;
  class BearCries TheClass;
  int retval;
  string message;

  if (argc != 2) { 
    fprintf(stderr, "usage: a.out num or - to enter the string on stdin\n"); 
    exit(1); 
  }

  if ((string) argv[1] == "-") {
    cin >> message;
  } else {
    if (atoi(argv[1]) == 0) {
      message = ";_;;_____;";
    }
  
    if (atoi(argv[1]) == 1) {
      message = ";;;___;;;";
    }
  
    if (atoi(argv[1]) == 2) {
      message = "_;__;";
    }
  
    if (atoi(argv[1]) == 3) {
      message = ";_____________________________________;";
    }
  
    if (atoi(argv[1]) == 4) {
      message = ";__;____;";
    }
  
    if (atoi(argv[1]) == 5) {
      message = ";_;;__;_;;";
    }
  }

  retval = TheClass.count(message);
  cout << retval << endl;

  exit(0);
}
