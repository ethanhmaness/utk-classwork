//mipsmach.cpp
//COSC130
//Ethan Maness
//3-24-21
//Simulates a MIPS Assembly machine by reading in binary instructions from a file

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

//list of all register namaes
const string reg_names[] = {
    "$zero", "$at", "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3",
    "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3",
    "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra"
};

// Divide the total size of the register names array by one to get the count.
const uint32_t NUM_REGS = sizeof(reg_names) / sizeof(reg_names[0]);

struct Machine {
    uint32_t regs[NUM_REGS]; // NUM_REGS is defined from the register array
    uint32_t pc; //program counter, the offset IN BYTES of the instruction
    uint32_t num_instructions; //num of instructions (total bytes / bytes per instruction)
    uint32_t *instructions; //pointer to array of instructions
    void runInstruction(uint32_t); //searches dictionary of instruction codes to run the appropriate command on the machine
};

string* cmdsplit(string); //splits a command string into the command and argument

int main(){

    Machine* mach = nullptr; //mach will point to the current machine
    FILE* bin; //file that the instructions will be read in from

    bool q = false; //quit flag
    string cmd; //the name of the current command
    string arg; //argument for command, if applicable
    string scantemp; //just used to grab the raw input from user
    string* cmdarr; //used to split the input into cmd and arg

    //while quit flag is false
    while(!q){

        //reads up to 8 bytes of user-input command
        getline(cin, scantemp);

        //split input into cmd and arg
        cmdarr = cmdsplit(scantemp);
        cmd = cmdarr[0];
        arg = cmdarr[1];
        delete[] cmdarr; //remove now-unused memory

        /*
            If-Else structure to run the appropriate code corresponding to cmd
            Will check for unknown commands and empty arguments
        */

        //"regs" -- Dump all registers in hexadecimal
        if(cmd == "regs"){
            //check if there is a Machine
            if(mach != nullptr){

                //print all the registers
                for(unsigned int i = 0; i < NUM_REGS; i++){

                    //formatting
                    cout << left << setw(5) << setfill(' ') << reg_names[i] << ": 0x" << setw(8) << setfill('0') << right << hex << mach->regs[i] << " (" << setfill(' ') << setw(5) << dec << (int) mach->regs[i] << ") ";
                    
                    //newlines
                    if((i + 1) % 4 == 0) cout << "\n"; //4 to a line
                    else if(i == NUM_REGS - 1) cout << "\n"; //make sure PC has its own line

                }
                cout << "PC   : " << mach->pc << "\n"; //print program counter below

            }

        //"reg <register>" -- Dump the register of the given name in hexadecimal and decimal
        }else if(cmd == "reg"){

            //check if there is a Machine and argument
            if(mach != nullptr && arg.length() >= 1){

                int index;

                for(unsigned int i = 0; i < NUM_REGS; i++){
                    if(arg == reg_names[i]) index = i;
                }

                //print (with formatting) the register value in hex and dec
                cout << left << setw(5) << setfill(' ') << arg << ": 0x" << setw(8) << setfill('0') << right << hex << mach->regs[index] << " (" << setfill(' ') << setw(5) << dec << mach->regs[index] << ")\n";

            }

        //"next" -- Run 1 instruction
        }else if(cmd == "next"){
            //check if there is a Machine
            if(mach != nullptr){

                //check if pc is within the range of instructions
                if(mach->pc < mach->num_instructions * 4){

                    //run instruction at the appropriate spot according to PC
                    mach->runInstruction(mach->instructions[mach->pc/4]);

                    //move pc forward to next instruction
                    mach->pc += 4;

                }else cout << "No more instructions to run.\n";

            }

        //"run" -- Run all instructions
        }else if(cmd == "run"){

            //check if there is a Machine
            if(mach != nullptr){

                //run until pc reaches end of range of instructions
                while(mach->pc < mach->num_instructions * 4){

                    //run instruction at the appropriate spot according to PC
                    mach->runInstruction(mach->instructions[mach->pc/4]);

                    //move pc forward to next instruction
                    mach->pc += 4;

                }

            }

        //"load <filename>" -- Load a new .bin of instructions
        }else if(cmd == "load"){

            //check if there is an argument
            if(arg.length() >= 1){
                
                //delete any existing machine
                if(mach != nullptr){
                    delete[] mach->instructions;
                    delete mach;
                }
                //open file provided by arg
                bin = fopen(arg.c_str(), "rb");

                //if file is opened
                if(bin != nullptr){

                    //allocate new machine in memory
                    mach = new Machine;

                    //determine number of 4-byte instructions
                    fseek(bin, 0, SEEK_END);
                    mach->num_instructions = ftell(bin) / 4;
                    fseek(bin, 0, SEEK_SET);

                    //allocate 4 bytes per instruction
                    mach->instructions = new uint32_t[mach->num_instructions];

                    //read contents of file into instructions
                    for(unsigned int i = 0; i < mach->num_instructions; i++){
                        fread(&mach->instructions[i], 1, 4, bin);
                    }

                    //init regs and pc to zero
                    mach->pc = 0;
                    for(unsigned int i = 0; i < NUM_REGS; i++) mach->regs[i] = 0;

                }else cout << "Unable to open file \"" << arg <<"\".\n";

            }else cout << "Usage: load <filename>\n";

        //"quit" -- quit the program
        }else if(cmd == "quit"){
            q = true;

        //unknown command
        }else{
            cout << "Unknown command \"" << cmd << "\".";
        }

    }

    //make sure all memory is gone
    delete[] mach->instructions;
    delete mach;

}

//takes an int and returns the bits between "from" and "to" (inclusive), right-aligned
uint32_t getBits(uint32_t val, int from, int to){

    val = val << (31 - from);
    val = val >> (to + 31 - from);

    return val;

}

//returns pointer to a 2-element string array, where [0] is the command and [1] is the argument
//only handles 1 argument, split by a single space
string* cmdsplit(string str){
    
    //find the space
    unsigned int index = str.find(' ');

    //allocate memory for string array
    string* strings = new string[2];

    //if a space was found, put the things in the array
    if(index != string::npos){
        strings[0] = str.substr(0,index);
        strings[1] = str.substr(index + 1);
    //if no space, just put str and an empty string in the array
    }else{
        strings[0] = str;
        strings[1] = "";
    }

    return strings;

}

//the big thing of instructions
//reads the bits from the Machine's instruction int passed in and does stuff
void Machine::runInstruction(uint32_t instruction){

    //get the operation code
    uint8_t opcode = getBits(instruction, 31, 26);

    //001000 - ADDI rs rt immediate
    if(opcode == 0b001000){

        uint8_t rs = 0;
        uint8_t rt = 0;
        int16_t imm = 0;

        rs = getBits(instruction, 25, 21); //rs
        rt = getBits(instruction, 20, 16); //rt
        imm = getBits(instruction, 15, 0); //immediate

        //rt = rs + immediate
        regs[rt] = regs[rs] + imm;

    //000100 - BEQ rs rt offset
    }else if(opcode == 0b000100){

        uint8_t rs = 0;
        uint8_t rt = 0;
        int16_t off = 0;

        rs = getBits(instruction, 25, 21); //rs
        rt = getBits(instruction, 20, 16); //rt
        off = getBits(instruction, 15, 0); //offset

        //if rs = rt, move pc by offset instructions
        if(regs[rs] == regs[rt]) pc += 4 * off;

    //000101 - BNE rs rt offset
    }else if(opcode == 0b000101){

        uint8_t rs = 0;
        uint8_t rt = 0;
        int16_t off = 0;

        rs = getBits(instruction, 25, 21); //rs
        rt = getBits(instruction, 20, 16); //rt
        off = getBits(instruction, 15, 0); //offset

        //if rs != rt, move pc by offset instructions
        if(regs[rs] != regs[rt]) pc += 4 * off;

    //000000 - SPECIAL, refer to special code
    }else if(opcode == 0b000000){

        //get the special opcode
        uint8_t spec = getBits(instruction, 5, 0);

        //100000 - rs rt rd 0 ADD
        if(spec == 0b100000){

            uint8_t rs = 0;
            uint8_t rt = 0;
            uint8_t rd = 0;

            rs = getBits(instruction, 25, 21); //rs
            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd

            //rd = rs + rt
            regs[rd] = regs[rs] + regs[rt];

        //100100 - rs rd rt 0 AND
        }else if(spec == 0b100100){

            uint8_t rs = 0;
            uint8_t rt = 0;
            uint8_t rd = 0;

            rs = getBits(instruction, 25, 21); //rs
            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd

            //rd = rs & rt
            regs[rd] = regs[rs] & regs[rt];

        //100101 - rs rt rd 0 OR
        }else if(spec == 0b100101){

            uint8_t rs = 0;
            uint8_t rt = 0;
            uint8_t rd = 0;

            rs = getBits(instruction, 25, 21); //rs
            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd

            //rd = rs | rt
            regs[rd] = regs[rs] | regs[rt];

        //000000 - 0 rt rd sa SLL
        }else if(spec == 0b000000){

            uint8_t rt = 0;
            uint8_t rd = 0;
            uint8_t sa = 0;

            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd
            sa = getBits(instruction, 10, 6); //sa

            //rd = rs << rt
            regs[rd] = regs[rt] << sa;

        //101010 - rs rt rd 0 SLT
        }else if(spec == 0b101010){

            uint8_t rs = 0;
            uint8_t rt = 0;
            uint8_t rd = 0;

            rs = getBits(instruction, 25, 21); //rs
            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd

            //rd = (rs < rt)
            if(regs[rs] < regs[rt]) regs[rd] = 1;
            else regs[rd] = 0;

        //000011 - 0 rt rd sa SRA
        }else if(spec == 0b000011){

            uint8_t rt = 0;
            uint8_t rd = 0;
            uint8_t sa = 0;

            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd
            sa = getBits(instruction, 10, 6); //sa

            //rd = rs >> rt (arithmetic)
            regs[rd] = ((int32_t) regs[rt]) >> sa;

        //000010 - 0 rt rd sa SRL
        }else if(spec == 0b000010){

            uint8_t rt = 0;
            uint8_t rd = 0;
            uint8_t sa = 0;

            rt = getBits(instruction, 20, 16); //rt
            rd = getBits(instruction, 15, 11); //rd
            sa = getBits(instruction, 10, 6); //sa

            //rd = rs >> rt (logical)
            regs[rd] = regs[rt] >> sa;

        }else{
            cout << "Unknown instruction...continuing.\n";
        }

    }else{
        cout << "Unknown instruction...continuing.\n";
    }

    //always set $zero back to 0
    regs[0] = 0;
    
}

