#include "ThreeIncreasing.cpp"

int main(int argc, char **argv)
{
    int i;
    class ThreeIncreasing TheClass;
    string arg;
    int retval;
    int a;
    int b;
    int c;

    if (argc != 2) { fprintf(stderr, "usage: a.out num\n"); exit(1); }
    arg = argv[1];

    if (arg == "-") {
        cin >> a >> b >> c;
    }
    else {

        if (atoi(argv[1]) == 0) {
            a = 15;
            b = 40;
            c = 22;
        }

        if (atoi(argv[1]) == 1) {
            a = 5;
            b = 6;
            c = 6;
        }

        if (atoi(argv[1]) == 2) {
            a = 6;
            b = 1;
            c = 3000;
        }

        if (atoi(argv[1]) == 3) {
            a = 6;
            b = 4;
            c = 2;
        }

        if (atoi(argv[1]) == 4) {
            a = 4;
            b = 2;
            c = 6;
        }

        if (atoi(argv[1]) == 5) {
            a = 1;
            b = 1234;
            c = 3000;
        }

        if (atoi(argv[1]) == 6) {
            a = 2789;
            b = 2400;
            c = 1693;
        }
    }

    retval = TheClass.minEaten(a, b, c);
    cout << retval << endl;

    exit(0);
}
