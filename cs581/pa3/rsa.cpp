#include <iostream>
#include <string>
#include <stdexcept>
#include <cstring>
using namespace std;

// Stores the integer represented by s in i, if possible.
// If s cannot be parsed as an integer, returns false. Otherwise, returns true.
bool try_stoi(char* s, int& i)
{
    try
    {
        i = stoi(s);
        return true;
    }
    catch (const invalid_argument& err)
    {
        return false;
    }
}

// Calculate the private key for a given public key and totient using the extended Euclidean algorithm.
// Throws an error if public_key and totient are not relatively prime.
int private_key(int public_key, int totient)
{

    int t = 0,
        t_next = 1,
        r = totient,
        r_next = public_key,
        q, t_prev, r_prev;

    while (r_next != 0)
    {
        q = (r > 0) ? r / r_next : (r / r_next) - 1;

        t_prev = t;
        t = t_next;
        t_next = t_prev - q * t;

        r_prev = r;
        r = r_next;
        r_next = r_prev - q * r;
    }

    if (r > 1)
    {
        printf("r: %d\n", r);
        throw "public_key and totient must be relatively prime";
    }

    if (t < 0)
    {
        t += totient;
    }

    return t;
}

// Perform modular exponentiation
int mod_exp(int base, int exp, int modulo)
{
    long c = 1, e = 0;
    while (e < exp)
    {
        e++;
        c = (base * c) % modulo;
        if (c < 0)
        {
            c += modulo;
        }
    }
    return c;
}

// Append ASCII values of up to four characters to create a 32-bit int
int ascii_str_to_int(char* str)
{
    if (strlen(str) > 5)
    {
        throw "ASCII string cannot be longer than 4 characters, excluding the null termination";
    }
    char buff[4] = {};
    strncpy(buff, str, 4);
    return (int) buff[0] | (((int) buff[1]) << 8) | (((int) buff[2]) << 16) | (((int) buff[3]) << 24);
}

// Separate an int by byte into a char string
char* int_to_ascii_str(int i)
{
    char* buff = (char*) malloc(5);
    memset(buff, 0, 5);
    buff[0] = i & (255);
    buff[1] = (i & (255 << 8)) >> 8;
    buff[2] = (i & (255 << 16)) >> 16;
    buff[3] = (i & (255 << 24)) >> 24;
    return buff;
}

bool is_prime(int n)
{
    if (n <= 1)
        return false;
 
    for (int i = 2; i <= n / 2; i++)
        if (n % i == 0)
            return false;
 
    return true;
}

// Performs RSA using the primes and message provided in command-line arguments
int main(int argc, char* argv[])
{
    // Check arg count
    if (argc != 4)
    {
        printf("Usage: %s <prime 1> <prime 2> <message>\n", argv[0]);
        return 1;
    }

    // Get primes from args
    int p, q;
    char* failstr;
    if (!((failstr = argv[1]) && try_stoi(argv[1], p) && (failstr = argv[2]) && try_stoi(argv[2], q)))
    {
        printf("Cannot convert %s to integer\n", failstr);
        return 1;
    }

    char* msg = argv[3];

    int failint;
    if(!((failint = p) && is_prime(p) && (failint = q) && is_prime(q)))
    {
        printf("%d is not prime\n", failint);
        return 1;
    }

    int n = p * q;
    int plaintext = ascii_str_to_int(msg);

    if (plaintext >= n)
    {
        printf("Plaintext %s (value %d) outside of message domain %d. Plaintext must be smaller than the product of the two primes.\n", msg, plaintext, n);
        return 1;
    }

    int totient = (p-1) * (q-1);
    int e = 65537;

    int d = private_key(e, totient);

    int ciphertext = mod_exp(plaintext, e, n);
    int decrypted_text = mod_exp(ciphertext, d, n);

    printf("%d\n", e);
    printf("%d\n", d);
    printf("%s\n", msg);
    printf("%d\n", ciphertext);
    printf("%s\n", int_to_ascii_str(decrypted_text));

    return 0;
}