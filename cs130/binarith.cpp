/*  Ethan Maness
    2/11/21
    Allows user to perform basic mathematic operations
*/

#include <cstdio>

int Multiply(int leftop, int rightop);
int Add(int leftop, int rightop);
int Sub(int leftop, int rightop);
int Twos(int op);
int Div(int leftop, int rightop);
int Mod(int leftop, int rightop);

int main(int argc, char *argv[])
{
   int left, right, result;
   char op;

   if (4 != argc)
   {
       printf("Usage: %s <left> <op> <right>\n", argv[0]);
       return -1;
   }
   sscanf(argv[1], "%d", &left);
   sscanf(argv[2], "%c", &op);
   sscanf(argv[3], "%d", &right);

    switch (op)
   {
   case 'm':
   case 'x':
   case '*':
       result = Multiply(left, right);
       break;
   case 'a':
   case '+':
       result = Add(left, right);
       break;
   case 's':
   case '-':
       result = Sub(left, right);
       break;
   case 'd':
   case '/':
        result = Div(left, right);
        break;
    case '%':
        result = Mod(left, right);
        break;
    default:
        result = -11111111;
        break;
   }
    printf("%d\n", result);
    return 0;
}

//Write your functions here

//gets a singular bit from an int at an index
int GetBit(int n, unsigned int i) {
    
    //apply mask, return isolated bit (0 or 1)
    return (n >> i) & 1;
}

//performs addition on the two operands
int Add(int leftop, int rightop){

    //start with all zero bits
    int out = 0;

    //internal use vars
    int carry = 0;
    int lbit;
    int rbit;

    //do the process for all bits
    for(int i = 0; i < 32; i++){

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

//performs twos complement on the operand
int Twos(int op){

    //just use flipped_value + 1
    return Add(~op, 1);

}

//performs subtraction on the two operands
int Sub(int leftop, int rightop){

    //just add leftop to the negative of rightop
    return Add(leftop, Twos(rightop));

}

//performs multiplication on the two operands
int Multiply(int leftop, int rightop){

    //start with all zeroes
    int out = 0;

    //if exactly one operand is negative, output will be negative
    int sign = GetBit(leftop, 31) ^ GetBit(rightop, 31);

    //make both operands positive for purposes of performing arithmetic
    if(GetBit(leftop,31)) leftop = Twos(leftop);
    if(GetBit(rightop,31)) rightop = Twos(rightop);

    //multiply every digit on the bottom by the top, sum together
    for(int i = 0; i < 32; i++){

        if(GetBit(rightop,0)) out = Add(out, leftop << i);
        rightop = rightop >> 1;

        //stop if there's no more 1's on the bottom
        if(rightop == 0) break;

    }

    if(sign) out = Twos(out);

    return out;

}

//performs restoring division
int Div(int leftop, int rightop){

    //if exactly one operand is negative, output will be negative
    int sign = GetBit(leftop, 31) ^ GetBit(rightop, 31);

    //make both operands positive for purposes of performing arithmetic
    if(GetBit(leftop,31)) leftop = Twos(leftop);
    if(GetBit(rightop,31)) rightop = Twos(rightop);

    //store operands in larger datatype
    long remainder = leftop;
    long divisor = rightop;

    //start quotient at zero
    int quotient = 0;

    //perform restoring division
    for(int i = 31; i >= 0; i--){

        //find next remainder val
        remainder = remainder - (divisor << i);
        //if non-negative, set bit in quotient to 1 and keep new remainder
        if(remainder >= 0) quotient = quotient | (1 << i);
        else remainder = remainder + (divisor << i);

    }

    //apply stored sign
    if(sign) quotient = Twos(quotient);
    return quotient;

}

//computes the remainder after performing restoring division
int Mod(int leftop, int rightop){

    //compute remainder
    return Sub(leftop, Multiply(rightop,Div(leftop,rightop)));

}