#include "simulator.h"

int main(int argc, char const *argv[])
{
    int register_file[REG_COUNT] = {}; // Initializing every register with 0
    int memory[MEM_SIZE] = {};
    struct ControlUnit controlUnit;

    size_t PC = 0;

    if (argc < 2) 
    {
        printf("Please include the machine code as an argument to the executable.");
        exit(0);
    }

    // Loading the data into memory
    size_t text_data_size = load_memory(memory, argv[1]);

    char curr_instruction[33];
    struct Instruction *instruction;
    while (1)
    {
        // Fetching the instruction from the text and data memory
        int dec_instruction = memory[PC];
        convert_dec_2_32bit(dec_instruction, curr_instruction);

        // Decoding the instruction and forming the instruction struct
        instruction = decode_instruction(curr_instruction);

        // Updating control signals
        update_control_signals(instruction->opcode, &controlUnit);

        int data1 = register_file[instruction->rs];
        int data2 = register_file[instruction->rt];
        int write_register = (controlUnit.RegDst == 0) ? instruction->rt : instruction->rd;
        int extended = extention_unit(instruction->imm, controlUnit.ExtOp);

        int alu_second_input = (controlUnit.ALUSrc == 0) ? data2 : extended;

        int alu_zero;
        int alu_out = alu(data1, alu_second_input, controlUnit.ALUOp, &alu_zero);

        int memory_out = memory[alu_out];

        int to_reg = (controlUnit.MemtoReg == 0) ? alu_out : memory_out;

        int pc_plus_1 = PC + 1;
        int reg_write_data = (controlUnit.JumpReg == 0) ? pc_plus_1 : to_reg;

        if (controlUnit.RegWrite == 1) register_file[write_register] = reg_write_data;

        int offset = (controlUnit.JumpReg == 1) ? data1 : extended;

        int pc_with_offset = pc_plus_1 + offset;

        int new_pc;

        if (controlUnit.Branch && alu_zero)
            new_pc = pc_with_offset;
        else
            new_pc = pc_plus_1;

        if (controlUnit.Jump) new_pc = offset;

        PC = new_pc;
    }

    return 0;
}

size_t load_memory(int memory[MEM_SIZE], const char *machine_code_file_name)
{
    FILE *file = fopen(machine_code_file_name, "r");
    if (file == NULL)
    {
        printf("Cannot open the machine program file.");
        exit(1);
    }

    size_t line_size = sizeof(char) * 32;
    char *line = (char *)malloc(line_size);

    int i = 0;

    while (getline(&line, &line_size, file) != -1)
    {
        memory[i++] = atoi(line);
    }
}

void update_control_signals(const int opcode, struct ControlUnit *cu)
{
    int x = 1;

    // Setting most occurring values as the default
    cu->MemWrite = 0;
    cu->MemRead = 0;
    cu->RegWrite = 1;
    cu->ALUSrc = 0;
    cu->MemtoReg = 0;
    cu->ExtOp = 1;
    cu->RegDst = 0;
    cu->Branch = 0;
    cu->Jump = 0;
    cu->JumpReg = x;

    switch (opcode)
    {
    case OPC_ADD:
        cu->ALUOp = ALU_ADD;
        cu->RegDst = 1;
        break;
    case OPC_SUB:
        cu->ALUOp = ALU_SUB;
        cu->RegDst = 1;
        break;
    case OPC_SLT:
        cu->ALUOp = ALU_SLT;
        cu->RegDst = 1;
        break;
    case OPC_OR:
        cu->ALUOp = ALU_OR;
        cu->RegDst = 1;
        break;
    case OPC_NAND:
        cu->ALUOp = ALU_NAND;
        cu->RegDst = 1;
        break;
    case OPC_ADDI:
        cu->ALUOp = ALU_ADD;
        cu->ALUSrc = 1;
        break;
    case OPC_SLTI:
        cu->ALUOp = ALU_SLT;
        cu->ALUSrc = 1;
        break;
    case OPC_ORI:
        cu->ALUOp = ALU_OR;
        cu->ALUSrc = 1;
        cu->ExtOp = 0;
        break;
    case OPC_LUI:
        cu->ALUOp = ALU_ADD;
        cu->ALUSrc = 1;
        break;
    case OPC_LW:
        cu->MemRead = 1;
        cu->ALUOp = ALU_ADD;
        cu->ALUSrc = 1;
        cu->MemtoReg = 1;
        break;
    case OPC_SW:
        cu->MemWrite = 1;
        cu->RegWrite = 0;
        cu->ALUOp = ALU_ADD;
        cu->ALUSrc = 1;
        break;
    case OPC_BEQ:
        cu->RegWrite = 0;
        cu->ALUOp = ALU_SUB;
        cu->Branch = 1;
        break;
    case OPC_JALR:
        cu->Jump = 1;
        cu->JumpReg = 1;
        break;
    case OPC_J:
        cu->RegWrite = 0;
        cu->ExtOp = 0;
        cu->Jump = 1;
        break;
    case OPC_HALT:
        cu->RegWrite = 0;
        cu->Jump = 1;
        break;
    default:
        break;
    }
}

struct Instruction *decode_instruction(char *raw_instruction)
{
    struct Instruction *instruction = (struct Instruction *)malloc(sizeof(struct Instruction));

    char opcode[5];
    char rs[5];
    char rt[5];
    char rd[5];
    char imm[17];

    strncpy(opcode, raw_instruction + (32 - 27), 4);

    int opcode_dec = convert_binary_to_dec(opcode, 4);
    instruction->opcode = opcode_dec;

    if (opcode_dec >= 0 && opcode_dec <= 4)
    {
        instruction->instType = 0;
        strncpy(rs, raw_instruction + (32 - 23), 4);
        strncpy(rt, raw_instruction + (32 - 19), 4);
        strncpy(rd, raw_instruction + (32 - 15), 4);

        instruction->rs = convert_binary_to_dec(rs, 4);
        instruction->rt = convert_binary_to_dec(rt, 4);
        instruction->rd = convert_binary_to_dec(rd, 4);
    }
    else if (opcode_dec <= 12)
    {
        instruction->instType = 1;
        strncpy(rs, raw_instruction + (32 - 23), 4);
        strncpy(rt, raw_instruction + (32 - 19), 4);
        strncpy(imm, raw_instruction + (32 - 15), 16);

        instruction->rs = convert_binary_to_dec(rs, 4);
        instruction->rt = convert_binary_to_dec(rt, 4);
        instruction->imm = convert_binary_to_dec(imm, 16);
    }
    else if (opcode_dec <= 14)
    {
        instruction->instType = 2;
        strncpy(imm, raw_instruction + (32 - 15), 16);

        instruction->imm = convert_binary_to_dec(imm, 16);
    }
    else
    {
        instruction->instType = -1;
    }
}

int extention_unit(const int input, const int op)
{
    int out = input;
    if (op == -1)
        return out << 16;
    else
        return out; // Zero extention and signed extention has no meaning in decimal
}

int alu(const int operand_a, const int operand_b, const int op, int *zero)
{
    int out;
    switch (op)
    {
    case ALU_ADD:
        out = operand_a + operand_b;
    case ALU_SUB:
        out = operand_a - operand_b;
    case ALU_SLT:
        out = (operand_a < operand_b) ? 1 : 0;
    case ALU_OR:
        out = operand_a | operand_b;
    case ALU_NAND:
        out = ~(operand_a & operand_b);
    default:
        out = operand_a;
    }

    *zero = (out == 0) ? 1 : 0;

    return out;
}

void convert_dec_2_32bit(const int decimal_number, char bits[33])
{
    for (int i = 0; i < 32; i++)
    {
        bits[i] = decimal_number & (1 << i);
    }
}

int convert_binary_to_dec(char *bin, size_t n)
{
    int out = 0;

    for (int i = n - 1; i >= 0; i--)
        if (bin[i] == '1')
            out += (int)pow(2, i);

    return out;
}
