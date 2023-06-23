#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REG_COUNT 16
#define TXT_MEM_SIZE 8192
#define MEM_SIZE 65535

#define OPC_ADD 0
#define OPC_SUB 1
#define OPC_SLT 2
#define OPC_OR 3
#define OPC_NAND 4
#define OPC_ADDI 5
#define OPC_SLTI 6
#define OPC_ORI 7
#define OPC_LUI 8
#define OPC_LW 9
#define OPC_SW 10
#define OPC_BEQ 11
#define OPC_JALR 12
#define OPC_J 13
#define OPC_HALT 14

#define ALU_ADD 0
#define ALU_SUB 1
#define ALU_SLT 2
#define ALU_OR 3
#define ALU_NAND 4

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

/**
 * Loads the machine file code into the memory, acts as the loader
 */
size_t load_memory(int memory[MEM_SIZE], const char *machine_code_file_name);

/**
 * Updates the control signals based on the given opcode
 */
void update_control_signals(const int opcode, struct ControlUnit *cu);

/**
 * Decodes the raw string binary instruction and creates a Instruction struct
 */
struct Instruction *decode_instruction(char *raw_instruction);

/**
 * It is supposed to extend any 16 bit input to a 32 bit output, but when the numbers are in decimals, it doesn't really matter.
 * Except for the move 16bit to upper extention
 */
int extention_unit(const int input, const int op);

/**
 * The Arithmetic and Logic Unit of the system. Takes two input and does an operation based on the op signal.
 * If the output is zero, the zero signal will be set to 1
 */
int alu(const int operand_a, const int operand_b, const int op, int *zero);

/**
 * Converts a decimal number to a 32 bit char array
 */
void convert_dec_2_32bit(const int decimal_number, char bits[33]);

/**
 * Converts a n bit char array to a decimal
 */
int convert_binary_to_dec(char *bin, size_t n);