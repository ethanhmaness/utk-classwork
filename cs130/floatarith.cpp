//float_lab.cpp
//COSC130
//Ethan Maness
//2-16-21
//Does addition and subtraction with floating point values

#include <cstdio>

struct Real
{
    //sign bit
    int sign;

    //UNBIASED exponent
    long exponent;

    //Fraction including implied 1. at bit index 23
    unsigned long fraction;
};

Real Decode(int float_value);
int Encode(Real real_value);
Real Normalize(Real value);
Real Multiply(Real left, Real right);
Real Add(Real left, Real right);

int main(int argc, char *argv[])
{
    int left, right;
    char op;
    int value;
    Real rLeft, rRight, result;

    if (argc < 4) {
        printf("Usage: %s <left> <op> <right>\n", argv[0]);
        return -1;
    }

    sscanf(argv[1], "%f", (float *)&left);
    sscanf(argv[2], "%c", &op);
    sscanf(argv[3], "%f", (float *)&right);

    rLeft = Decode(left);
    rRight = Decode(right);

    if (op == 'x') {
        result = Multiply(rLeft, rRight);
    }
    else if (op == '+') {
        result = Add(rLeft, rRight);
    }
    else {
        printf("Unknown operator '%c'\n", op);
        return -2;
    }

    value = Encode(result);
    printf("%.3f %c %.3f = %.3f (0x%08x)\n", *((float*)&left), op, *((float*)&right), *((float*)&value), value); return 0;
}

//gets a singular bit from a long at an index
long GetBit(long n, unsigned int i) {
    
    //apply mask, return isolated bit (0 or 1)
    return (n >> i) & 1;
}

//does twos complement
long Twos(int n)
{
    return (~n) + 1;
}

//performs addition on the two operands
long AddLong(long leftop, long rightop){

    //start with all zero bits
    long out = 0;

    //internal use vars
    long carry = 0;
    long lbit;
    long rbit;

    //do the process for all bits
    for(int i = 0; i < 64; i++){

        //get the bits currently being added
        lbit = GetBit(leftop, i);
        rbit = GetBit(rightop, i);

        //put the output bit in the sum
        out = out | ((lbit ^ rbit ^ carry) << i);

        //calc the carry for next bit index
        carry = (lbit & rbit) | (rbit & carry) | (lbit & carry);
    }

    return out;
}

//performs subtraction on the two operands
long SubLong(long leftop, long rightop){

    //just add leftop to the negative of rightop
    return AddLong(leftop, Twos(rightop));

}

//takes an int that has been casted from a float in 32-bit IEEE-754 format and converts it to a Real object
Real Decode(int float_value)
{
    Real r;

    //0000 0000 0111 1111 1111 1111 1111 1111 masks the fraction
    r.fraction = float_value & 0x007FFFFF;
    //set the implied 1
    r.fraction |= 0x00800000;

    //0111 1111 1000 0000 0000 0000 0000 0000 masks the exponent, subtract bias to get raw value
    r.exponent = ((float_value & 0x7F800000) >> 23) - 127; 

    //1000 0000 0000 0000 0000 0000 0000 0000 masks the sign
    r.sign = (float_value & 0x80000000) >> 31; 

    return r;
}

//takes a Real object and creates an int formatted in 32-bit IEEE-754 format (like a float)
int Encode(Real real_value)
{
    int r = 0;

    //copy over the fraction, but mask off the implied 1
    r |= (real_value.fraction & 0x007FFFFF);

    //add the bias and copy over exponent (mask just in case)
    r |= ((real_value.exponent + 127) << 23) & 0x7F800000;

    //copy over sign
    r |= real_value.sign << 31;

    return r;

}

//takes a Real object and returns a Real of (approximately) same value w/ fraction as 1.something
Real Normalize(Real value)
{
    Real r = value;

    //if the part left of decimal isn't 1, do stuff to make it be 1
    while((r.fraction & 0xFF800000) >> 23 != 1){
        if(r.fraction == 0){
            //if exactly 0, don't run the shift stuff
            r.exponent = -127;
            r.fraction = 0;
            break;
        }
        //if greater than 1, leftshift and increase exponent
        if((r.fraction & 0xFF800000) >> 23 > 1){
            r.exponent += 1;
            r.fraction = r.fraction >> 1;
        //if less than 1, rightshift and decrease exponent
        } else {
            r.exponent -= 1;
            r.fraction = r.fraction << 1;
        }
    }

    //exponent clamping
    if(r.exponent < -127) r.exponent = -127;

    return r;
}

//multiplies two Real objects
Real Multiply(Real left, Real right)
{
    Real r;

    //start exponent as the bigger one
    r.exponent = left.exponent + right.exponent;

    //multiply fraction, adjust decimal
    r.fraction = (left.fraction * right.fraction) >> 23;

    //make the sign be right
    r.sign = left.sign ^ right.sign;

    //normalize
    return Normalize(r);
}

//does bit addition on two objects
Real Add(Real left, Real right)
{
    Real r;

    //make the exponent be the bigger one
    long bigExponent = left.exponent;
    if(right.exponent > left.exponent) bigExponent = right.exponent;
    r.exponent = bigExponent;

    //make the fractions signed so negative works
    long slfrac = left.fraction;
    long srfrac = right.fraction;
    long signedfrac;

    //apply twos to make the sign right
    if(left.sign) slfrac = Twos(slfrac);
    if(right.sign) srfrac = Twos(srfrac);

    //add them, shifting according to exponent
    signedfrac = AddLong((slfrac >> SubLong(bigExponent, left.exponent)), srfrac >> SubLong(bigExponent, right.exponent));

    //un-sign and store
    if(signedfrac < 0){
        r.fraction = SubLong(0,signedfrac);
        r.sign = 1;
    } else r.fraction = signedfrac;

    //normalize
    return Normalize(r);

}

