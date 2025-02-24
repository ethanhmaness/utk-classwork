#include "SellingFruits.cpp"

int main(int argc, char **argv)
{
    int i;
    class SellingFruits TheClass;
    int retval;
    int x;
    int f;
    int d;
    int p;
    string arg;

    if (argc != 2) { fprintf(stderr, "usage: a.out num\n"); exit(1); }

    arg = argv[1];
    if (arg == "-") {

        cin >> x >> f >> d >> p;
    }
    else {

        if (atoi(argv[1]) == 0) {
            x = 3;
            f = 5;
            d = 100;
            p = 10;
        }

        if (atoi(argv[1]) == 1) {
            x = 2;
            f = 17;
            d = 20;
            p = 1;
        }

        if (atoi(argv[1]) == 2) {
            x = 1;
            f = 97;
            d = 98;
            p = 1;
        }

        if (atoi(argv[1]) == 3) {
            x = 16;
            f = 4;
            d = 8;
            p = 2;
        }

        if (atoi(argv[1]) == 4) {
            x = 17;
            f = 1;
            d = 2000000000;
            p = 4;
        }

        if (atoi(argv[1]) == 5) {
            x = 1;
            f = 1996245611;
            d = 1999990159;
            p = 123;
        }

        if (atoi(argv[1]) == 6) {
            x = 15000000;
            f = 100;
            d = 2000000000;
            p = 1;
        }

        if (atoi(argv[1]) == 7) {
            x = 1;
            f = 1000000000;
            d = 2000000000;
            p = 1000000000;
        }
    }

    retval = TheClass.maxDays(x, f, d, p);
    cout << retval << endl;

    exit(0);
}
