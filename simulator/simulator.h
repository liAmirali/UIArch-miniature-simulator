#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REG_COUNT 16
#define TXT_MEM_SIZE 8192
#define MEM_SIZE 65535

const int OPC_ADD = 0;
const int OPC_SUB = 1;
const int OPC_SLT = 2;
const int OPC_OR = 3;
const int OPC_NAND = 4;
const int OPC_ADDI = 5;
const int OPC_SLTI = 6;
const int OPC_ORI = 7;
const int OPC_LUI = 8;
const int OPC_LW = 9;
const int OPC_SW = 10;
const int OPC_BEQ = 11;
const int OPC_JALR = 12;
const int OPC_J = 13;
const int OPC_HALT = 14;

const int ALU_ADD = 0;
const int ALU_SUB = 1;
const int ALU_SLT = 2;
const int ALU_OR = 3;
const int ALU_NAND = 4;

struct Instruction
{
    /** 0 -> R-type, 1 -> I-type and 2 -> J-type */
    size_t instType; 
    char bits[33];
    int opcode;
    int rs;
    int rt;
    int rd;
    int imm;
};

struct ControlUnit
{
    /** Read from data memory 1(=enabled) or 0(=disabled) */
    int MemRead;
    /**  Write to data memory 1(=enabled) or 0(=disabled) */
    int MemWrite;
    /** Write to register file 1(=enabled) or 0(=disabled) */
    int RegWrite;
    /** ALU Operations: ALU_ADD, ALU_SUB, ALU_SLT, ALU_OR, ALU_NAND */
    int ALUOp;
    /** ALU Source: 0 selects Data 2, 1 selects extended number */
    int ALUSrc;
    /** If set to 0, the data from ALU will be selected; otherwise the memory data will be selected */
    int MemtoReg;
    /** 0 -> zero extention, 1 -> signed extention, -1 -> move to upper 16 bits */
    int ExtOp;
    /** 0 -> rt, 1 -> rd */
    int RegDst;
    /** is 1 if it is a branch instruction */
    int Branch;
    /** is 1 if it is a jump instruction */
    int Jump;
    /** is 1 if it is a specifically a jump to register instruction */
    int JumpReg;
};
