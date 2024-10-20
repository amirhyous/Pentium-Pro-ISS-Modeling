#include "Requirements.h"
string Operand_32b_MODRM[256][2] = {
    {"[EAX]", "EAX"}, {"[ECX]", "EAX"}, {"[EDX]", "EAX"}, {"[EBX]", "EAX"}, {"[SIB]", "EAX"}, {"[disp32]", "EAX"}, {"[ESI]", "EAX"}, {"[EDI]", "EAX"}, {"[EAX]", "ECX"}, {"[ECX]", "ECX"}, {"[EDX]", "ECX"}, {"[EBX]", "ECX"}, {"[SIB]", "ECX"}, {"[disp32]", "ECX"}, {"[ESI]", "ECX"}, {"[EDI]", "ECX"}, {"[EAX]", "EDX"}, {"[ECX]", "EDX"}, {"[EDX]", "EDX"}, {"[EBX]", "EDX"}, {"[SIB]", "EDX"}, {"[disp32]", "EDX"}, {"[ESI]", "EDX"}, {"[EDI]", "EDX"}, {"[EAX]", "EBX"}, {"[ECX]", "EBX"}, {"[EDX]", "EBX"}, {"[EBX]", "EBX"}, {"[SIB]", "EBX"}, {"[disp32]", "EBX"}, {"[ESI]", "EBX"}, {"[EDI]", "EBX"}, {"[EAX]", "ESP"}, {"[ECX]", "ESP"}, {"[EDX]", "ESP"}, {"[EBX]", "ESP"}, {"[SIB]", "ESP"}, {"[disp32]", "ESP"}, {"[ESI]", "ESP"}, {"[EDI]", "ESP"}, {"[EAX]", "EBP"}, {"[ECX]", "EBP"}, {"[EDX]", "EBP"}, {"[EBX]", "EBP"}, {"[SIB]", "EBP"}, {"[disp32]", "EBP"}, {"[ESI]", "EBP"}, {"[EDI]", "EBP"}, {"[EAX]", "ESI"}, {"[ECX]", "ESI"}, {"[EDX]", "ESI"}, {"[EBX]", "ESI"}, {"[SIB]", "ESI"}, {"[disp32]", "ESI"}, {"[ESI]", "ESI"}, {"[EDI]", "ESI"}, {"[EAX]", "EDI"}, {"[ECX]", "EDI"}, {"[EDX]", "EDI"}, {"[EBX]", "EDI"}, {"[SIB]", "EDI"}, {"[disp32]", "EDI"}, {"[ESI]", "EDI"}, {"[EDI]", "EDI"},

    {"[EAX+disp8]", "EAX"},
    {"[ECX+disp8]", "EAX"},
    {"[EDX+disp8]", "EAX"},
    {"[EBX+disp8]", "EAX"},
    {"[SIB+disp8]", "EAX"},
    {"[EBP+disp8]", "EAX"},
    {"[ESI+disp8]", "EAX"},
    {"[EDI+disp8]", "EAX"},
    {"[EAX+disp8]", "ECX"},
    {"[ECX+disp8]", "ECX"},
    {"[EDX+disp8]", "ECX"},
    {"[EBX+disp8]", "ECX"},
    {"[SIB+disp8]", "ECX"},
    {"[EBP+disp8]", "ECX"},
    {"[ESI+disp8]", "ECX"},
    {"[EDI+disp8]", "ECX"},
    {"[EAX+disp8]", "EDX"},
    {"[ECX+disp8]", "EDX"},
    {"[EDX+disp8]", "EDX"},
    {"[EBX+disp8]", "EDX"},
    {"[SIB+disp8]", "EDX"},
    {"[EBP+disp8]", "EDX"},
    {"[ESI+disp8]", "EDX"},
    {"[EDI+disp8]", "EDX"},
    {"[EAX+disp8]", "EBX"},
    {"[ECX+disp8]", "EBX"},
    {"[EDX+disp8]", "EBX"},
    {"[EBX+disp8]", "EBX"},
    {"[SIB+disp8]", "EBX"},
    {"[EBP+disp8]", "EBX"},
    {"[ESI+disp8]", "EBX"},
    {"[EDI+disp8]", "EBX"},
    {"[EAX+disp8]", "ESP"},
    {"[ECX+disp8]", "ESP"},
    {"[EDX+disp8]", "ESP"},
    {"[EBX+disp8]", "ESP"},
    {"[SIB+disp8]", "ESP"},
    {"[EBP+disp8]", "ESP"},
    {"[ESI+disp8]", "ESP"},
    {"[EDI+disp8]", "ESP"},
    {"[EAX+disp8]", "EBP"},
    {"[ECX+disp8]", "EBP"},
    {"[EDX+disp8]", "EBP"},
    {"[EBX+disp8]", "EBP"},
    {"[SIB+disp8]", "EBP"},
    {"[EBP+disp8]", "EBP"},
    {"[ESI+disp8]", "EBP"},
    {"[EDI+disp8]", "EBP"},
    {"[EAX+disp8]", "ESI"},
    {"[ECX+disp8]", "ESI"},
    {"[EDX+disp8]", "ESI"},
    {"[EBX+disp8]", "ESI"},
    {"[SIB+disp8]", "ESI"},
    {"[EBP+disp8]", "ESI"},
    {"[ESI+disp8]", "ESI"},
    {"[EDI+disp8]", "ESI"},
    {"[EAX+disp8]", "EDI"},
    {"[ECX+disp8]", "EDI"},
    {"[EDX+disp8]", "EDI"},
    {"[EBX+disp8]", "EDI"},
    {"[SIB+disp8]", "EDI"},
    {"[EBP+disp8]", "EDI"},
    {"[ESI+disp8]", "EDI"},
    {"[EDI+disp8]", "EDI"},

    {"[EAX+disp32]", "EAX"},
    {"[ECX+disp32]", "EAX"},
    {"[EDX+disp32]", "EAX"},
    {"[EBX+disp32]", "EAX"},
    {"[SIB+disp32]", "EAX"},
    {"[EBP+disp32]", "EAX"},
    {"[ESI+disp32]", "EAX"},
    {"[EDI+disp32]", "EAX"},
    {"[EAX+disp32]", "ECX"},
    {"[ECX+disp32]", "ECX"},
    {"[EDX+disp32]", "ECX"},
    {"[EBX+disp32]", "ECX"},
    {"[SIB+disp32]", "ECX"},
    {"[EBP+disp32]", "ECX"},
    {"[ESI+disp32]", "ECX"},
    {"[EDI+disp32]", "ECX"},
    {"[EAX+disp32]", "EDX"},
    {"[ECX+disp32]", "EDX"},
    {"[EDX+disp32]", "EDX"},
    {"[EBX+disp32]", "EDX"},
    {"[SIB+disp32]", "EDX"},
    {"[EBP+disp32]", "EDX"},
    {"[ESI+disp32]", "EDX"},
    {"[EDI+disp32]", "EDX"},
    {"[EAX+disp32]", "EBX"},
    {"[ECX+disp32]", "EBX"},
    {"[EDX+disp32]", "EBX"},
    {"[EBX+disp32]", "EBX"},
    {"[SIB+disp32]", "EBX"},
    {"[EBP+disp32]", "EBX"},
    {"[ESI+disp32]", "EBX"},
    {"[EDI+disp32]", "EBX"},
    {"[EAX+disp32]", "ESP"},
    {"[ECX+disp32]", "ESP"},
    {"[EDX+disp32]", "ESP"},
    {"[EBX+disp32]", "ESP"},
    {"[SIB+disp32]", "ESP"},
    {"[EBP+disp32]", "ESP"},
    {"[ESI+disp32]", "ESP"},
    {"[EDI+disp32]", "ESP"},
    {"[EAX+disp32]", "EBP"},
    {"[ECX+disp32]", "EBP"},
    {"[EDX+disp32]", "EBP"},
    {"[EBX+disp32]", "EBP"},
    {"[SIB+disp32]", "EBP"},
    {"[EBP+disp32]", "EBP"},
    {"[ESI+disp32]", "EBP"},
    {"[EDI+disp32]", "EBP"},
    {"[EAX+disp32]", "ESI"},
    {"[ECX+disp32]", "ESI"},
    {"[EDX+disp32]", "ESI"},
    {"[EBX+disp32]", "ESI"},
    {"[SIB+disp32]", "ESI"},
    {"[EBP+disp32]", "ESI"},
    {"[ESI+disp32]", "ESI"},
    {"[EDI+disp32]", "ESI"},
    {"[EAX+disp32]", "EDI"},
    {"[ECX+disp32]", "EDI"},
    {"[EDX+disp32]", "EDI"},
    {"[EBX+disp32]", "EDI"},
    {"[SIB+disp32]", "EDI"},
    {"[EBP+disp32]", "EDI"},
    {"[ESI+disp32]", "EDI"},
    {"[EDI+disp32]", "EDI"},

    {"EAX", "EAX"},
    {"ECX", "EAX"},
    {"EDX", "EAX"},
    {"EBX", "EAX"},
    {"ESP", "EAX"},
    {"EBP", "EAX"},
    {"ESI", "EAX"},
    {"EDI", "EAX"},
    {"EAX", "ECX"},
    {"ECX", "ECX"},
    {"EDX", "ECX"},
    {"EBX", "ECX"},
    {"ESP", "ECX"},
    {"EBP", "ECX"},
    {"ESI", "ECX"},
    {"EDI", "ECX"},
    {"EAX", "EDX"},
    {"ECX", "EDX"},
    {"EDX", "EDX"},
    {"EBX", "EDX"},
    {"ESP", "EDX"},
    {"EBP", "EDX"},
    {"ESI", "EDX"},
    {"EDI", "EDX"},
    {"EAX", "EBX"},
    {"ECX", "EBX"},
    {"EDX", "EBX"},
    {"EBX", "EBX"},
    {"ESP", "EBX"},
    {"EBP", "EBX"},
    {"ESI", "EBX"},
    {"EDI", "EBX"},
    {"EAX", "ESP"},
    {"ECX", "ESP"},
    {"EDX", "ESP"},
    {"EBX", "ESP"},
    {"ESP", "ESP"},
    {"EBP", "ESP"},
    {"ESI", "ESP"},
    {"EDI", "ESP"},
    {"EAX", "EBP"},
    {"ECX", "EBP"},
    {"EDX", "EBP"},
    {"EBX", "EBP"},
    {"ESP", "EBP"},
    {"EBP", "EBP"},
    {"ESI", "EBP"},
    {"EDI", "EBP"},
    {"EAX", "ESI"},
    {"ECX", "ESI"},
    {"EDX", "ESI"},
    {"EBX", "ESI"},
    {"ESP", "ESI"},
    {"EBP", "ESI"},
    {"ESI", "ESI"},
    {"EDI", "ESI"},
    {"EAX", "EDI"},
    {"ECX", "EDI"},
    {"EDX", "EDI"},
    {"EBX", "EDI"},
    {"ESP", "EDI"},
    {"EBP", "EDI"},
    {"ESI", "EDI"},
    {"EDI", "EDI"}};
string Address_32b_SIB[256][2] = {

    {"[EAX]", "EAX"}, {"[EAX]", "ECX"}, {"[EAX]", "EDX"}, {"[EAX]", "EBX"}, {"[EAX]", "ESP"}, {"[EAX]", "[*]"}, {"[EAX]", "ESI"}, {"[EAX]", "EDI"}, {"[ECX]", "EAX"}, {"[ECX]", "ECX"}, {"[ECX]", "EDX"}, {"[ECX]", "EBX"}, {"[ECX]", "ESP"}, {"[ECX]", "[*]"}, {"[ECX]", "ESI"}, {"[ECX]", "EDI"}, {"[EDX]", "EAX"}, {"[EDX]", "ECX"}, {"[EDX]", "EDX"}, {"[EDX]", "EBX"}, {"[EDX]", "ESP"}, {"[EDX]", "[*]"}, {"[EDX]", "ESI"}, {"[EDX]", "EDI"}, {"[EBX]", "EAX"}, {"[EBX]", "ECX"}, {"[EBX]", "EDX"}, {"[EBX]", "EBX"}, {"[EBX]", "ESP"}, {"[EBX]", "[*]"}, {"[EBX]", "ESI"}, {"[EBX]", "EDI"}, {"[none]", "EAX"}, {"[none]", "ECX"}, {"[none]", "EDX"}, {"[none]", "EBX"}, {"[none]", "ESP"}, {"[none]", "[*]"}, {"[none]", "ESI"}, {"[none]", "EDI"}, {"[EBP]", "EAX"}, {"[EBP]", "ECX"}, {"[EBP]", "EDX"}, {"[EBP]", "EBX"}, {"[EBP]", "ESP"}, {"[EBP]", "[*]"}, {"[EBP]", "ESI"}, {"[EBP]", "EDI"}, {"[ESI]", "EAX"}, {"[ESI]", "ECX"}, {"[ESI]", "EDX"}, {"[ESI]", "EBX"}, {"[ESI]", "ESP"}, {"[ESI]", "[*]"}, {"[ESI]", "ESI"}, {"[ESI]", "EDI"}, {"[EDI]", "EAX"}, {"[EDI]", "ECX"}, {"[EDI]", "EDX"}, {"[EDI]", "EBX"}, {"[EDI]", "ESP"}, {"[EDI]", "[*]"}, {"[EDI]", "ESI"}, {"[EDI]", "EDI"},

    {"[2EAX]", "EAX"},
    {"[2EAX]", "ECX"},
    {"[2EAX]", "EDX"},
    {"[2EAX]", "EBX"},
    {"[2EAX]", "ESP"},
    {"[2EAX]", "[*]"},
    {"[2EAX]", "ESI"},
    {"[2EAX]", "EDI"},
    {"[2ECX]", "EAX"},
    {"[2ECX]", "ECX"},
    {"[2ECX]", "EDX"},
    {"[2ECX]", "EBX"},
    {"[2ECX]", "ESP"},
    {"[2ECX]", "[*]"},
    {"[2ECX]", "ESI"},
    {"[2ECX]", "EDI"},
    {"[2EDX]", "EAX"},
    {"[2EDX]", "ECX"},
    {"[2EDX]", "EDX"},
    {"[2EDX]", "EBX"},
    {"[2EDX]", "ESP"},
    {"[2EDX]", "[*]"},
    {"[2EDX]", "ESI"},
    {"[2EDX]", "EDI"},
    {"[2EDX]", "EAX"},
    {"[2EDX]", "ECX"},
    {"[2EDX]", "EDX"},
    {"[2EDX]", "EBX"},
    {"[2EDX]", "ESP"},
    {"[2EDX]", "[*]"},
    {"[2EDX]", "ESI"},
    {"[2EDX]", "EDI"},
    {"[none]", "EAX"},
    {"[none]", "ECX"},
    {"[none]", "EDX"},
    {"[none]", "EBX"},
    {"[none]", "ESP"},
    {"[none]", "[*]"},
    {"[none]", "ESI"},
    {"[none]", "EDI"},
    {"[2EBP]", "EAX"},
    {"[2EBP]", "ECX"},
    {"[2EBP]", "EDX"},
    {"[2EBP]", "EBX"},
    {"[2EBP]", "ESP"},
    {"[2EBP]", "[*]"},
    {"[2EBP]", "ESI"},
    {"[2EBP]", "EDI"},
    {"[2ESI]", "EAX"},
    {"[2ESI]", "ECX"},
    {"[2ESI]", "EDX"},
    {"[2ESI]", "EBX"},
    {"[2ESI]", "ESP"},
    {"[2ESI]", "[*]"},
    {"[2ESI]", "ESI"},
    {"[2ESI]", "EDI"},
    {"[2EDI]", "EAX"},
    {"[2EDI]", "ECX"},
    {"[2EDI]", "EDX"},
    {"[2EDI]", "EBX"},
    {"[2EDI]", "ESP"},
    {"[2EDI]", "[*]"},
    {"[2EDI]", "ESI"},
    {"[2EDI]", "EDI"},

    {"[4EAX]", "EAX"},
    {"[4EAX]", "ECX"},
    {"[4EAX]", "EDX"},
    {"[4EAX]", "EBX"},
    {"[4EAX]", "ESP"},
    {"[4EAX]", "[*]"},
    {"[4EAX]", "ESI"},
    {"[4EAX]", "EDI"},
    {"[4ECX]", "EAX"},
    {"[4ECX]", "ECX"},
    {"[4ECX]", "EDX"},
    {"[4ECX]", "EBX"},
    {"[4ECX]", "ESP"},
    {"[4ECX]", "[*]"},
    {"[4ECX]", "ESI"},
    {"[4ECX]", "EDI"},
    {"[4EDX]", "EAX"},
    {"[4EDX]", "ECX"},
    {"[4EDX]", "EDX"},
    {"[4EDX]", "EBX"},
    {"[4EDX]", "ESP"},
    {"[4EDX]", "[*]"},
    {"[4EDX]", "ESI"},
    {"[4EDX]", "EDI"},
    {"[4EDX]", "EAX"},
    {"[4EDX]", "ECX"},
    {"[4EDX]", "EDX"},
    {"[4EDX]", "EBX"},
    {"[4EDX]", "ESP"},
    {"[4EDX]", "[*]"},
    {"[4EDX]", "ESI"},
    {"[4EDX]", "EDI"},
    {"[none]", "EAX"},
    {"[none]", "ECX"},
    {"[none]", "EDX"},
    {"[none]", "EBX"},
    {"[none]", "ESP"},
    {"[none]", "[*]"},
    {"[none]", "ESI"},
    {"[none]", "EDI"},
    {"[4EBP]", "EAX"},
    {"[4EBP]", "ECX"},
    {"[4EBP]", "EDX"},
    {"[4EBP]", "EBX"},
    {"[4EBP]", "ESP"},
    {"[4EBP]", "[*]"},
    {"[4EBP]", "ESI"},
    {"[4EBP]", "EDI"},
    {"[4ESI]", "EAX"},
    {"[4ESI]", "ECX"},
    {"[4ESI]", "EDX"},
    {"[4ESI]", "EBX"},
    {"[4ESI]", "ESP"},
    {"[4ESI]", "[*]"},
    {"[4ESI]", "ESI"},
    {"[4ESI]", "EDI"},
    {"[4EDI]", "EAX"},
    {"[4EDI]", "ECX"},
    {"[4EDI]", "EDX"},
    {"[4EDI]", "EBX"},
    {"[4EDI]", "ESP"},
    {"[4EDI]", "[*]"},
    {"[4EDI]", "ESI"},
    {"[4EDI]", "EDI"},

    {"[8EAX]", "EAX"},
    {"[8EAX]", "ECX"},
    {"[8EAX]", "EDX"},
    {"[8EAX]", "EBX"},
    {"[8EAX]", "ESP"},
    {"[8EAX]", "[*]"},
    {"[8EAX]", "ESI"},
    {"[8EAX]", "EDI"},
    {"[8ECX]", "EAX"},
    {"[8ECX]", "ECX"},
    {"[8ECX]", "EDX"},
    {"[8ECX]", "EBX"},
    {"[8ECX]", "ESP"},
    {"[8ECX]", "[*]"},
    {"[8ECX]", "ESI"},
    {"[8ECX]", "EDI"},
    {"[8EDX]", "EAX"},
    {"[8EDX]", "ECX"},
    {"[8EDX]", "EDX"},
    {"[8EDX]", "EBX"},
    {"[8EDX]", "ESP"},
    {"[8EDX]", "[*]"},
    {"[8EDX]", "ESI"},
    {"[8EDX]", "EDI"},
    {"[8EDX]", "EAX"},
    {"[8EDX]", "ECX"},
    {"[8EDX]", "EDX"},
    {"[8EDX]", "EBX"},
    {"[8EDX]", "ESP"},
    {"[8EDX]", "[*]"},
    {"[8EDX]", "ESI"},
    {"[8EDX]", "EDI"},
    {"[none]", "EAX"},
    {"[none]", "ECX"},
    {"[none]", "EDX"},
    {"[none]", "EBX"},
    {"[none]", "ESP"},
    {"[none]", "[*]"},
    {"[none]", "ESI"},
    {"[none]", "EDI"},
    {"[8EBP]", "EAX"},
    {"[8EBP]", "ECX"},
    {"[8EBP]", "EDX"},
    {"[8EBP]", "EBX"},
    {"[8EBP]", "ESP"},
    {"[8EBP]", "[*]"},
    {"[8EBP]", "ESI"},
    {"[8EBP]", "EDI"},
    {"[8ESI]", "EAX"},
    {"[8ESI]", "ECX"},
    {"[8ESI]", "EDX"},
    {"[8ESI]", "EBX"},
    {"[8ESI]", "ESP"},
    {"[8ESI]", "[*]"},
    {"[8ESI]", "ESI"},
    {"[8ESI]", "EDI"},
    {"[8EDI]", "EAX"},
    {"[8EDI]", "ECX"},
    {"[8EDI]", "EDX"},
    {"[8EDI]", "EBX"},
    {"[8EDI]", "ESP"},
    {"[8EDI]", "[*]"},
    {"[8EDI]", "ESI"},
    {"[8EDI]", "EDI"}

};
vector<sc_lv<120>> readInstructionsFromFile(const string &filename)
{
    vector<sc_lv<120>> test_instructions;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return test_instructions;
    }
    string line;
    int instructionCount = 0;
    while (getline(file, line) && instructionCount < 16)
    {
        if (line.length() != 120)
        {
            cerr << "Invalid instruction size in line " << instructionCount + 1 << endl;
            continue;
        }
        sc_lv<120> instruction = line.c_str();
        test_instructions.push_back(instruction);
        instructionCount++;
    }
    file.close();
    return test_instructions;
}
void DO_REG_IMM(sc_lv<8> ModRm, sc_lv<32> *operand, sc_lv<32> imm_data, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 0:
    {
        DO_Addition_Imm(operand, imm_data.to_int(), operand, EFLAGS);
        break;
    }
    case 1:
    {
        DO_OR_Imm(operand, imm_data, operand, EFLAGS);
        break;
    }
    case 4:
    {
        DO_AND_Imm(operand, imm_data, operand, EFLAGS);
        break;
    }
    case 5:
    {
        DO_Subtraction_Imm(operand, imm_data.to_int(), operand, EFLAGS);
        break;
    }
    case 7:
    {
        DO_Compare(imm_data, *operand, EFLAGS);
    }
    default:
        break;
    }
}
sc_lv<32> DO_MEM_IMM(sc_lv<8> ModRm, sc_lv<32> mem_data, sc_lv<32> imm_data, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 0:
    {
        return DO_AND_Imm_Mem(imm_data, mem_data, EFLAGS);
        break;
    }
    case 1:
    {
        return DO_OR_Imm_Mem(imm_data, mem_data, EFLAGS);
        break;
    }
    case 4:
    {
        return DO_Addition_Imm_Mem(imm_data, mem_data, EFLAGS);
        break;
    }
    case 5:
    {
        DO_Subtraction_Imm_Mem(imm_data, mem_data, EFLAGS);
        break;
    }
    case 7:
    {
        DO_Compare(imm_data, mem_data, EFLAGS);
        break;
    }
    default:
        break;
    }
    return 0;
}
sc_lv<32> DO_Unary_Reg(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<8> ModRm, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 4:
    {
        DO_Multiplication(op1, op2, op2, EFLAGS);
        break;
    }
    case 5:
    {
        DO_Division(op1, op2, op2);
        break;
    }
    case 7:
    {
        DO_IDivision(op1, op2, op2);
        break;
    }
    default:
        break;
    }
    return 0;
}
sc_lv<32> DO_Unary_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<8> ModRm, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 4:
    {
        return DO_Multiplication_Mem(op1, mem_data, EFLAGS);
        break;
    }
    case 5:
    {
        return DO_Division_Mem(op1, mem_data);
        break;
    }
    case 7:
    {
        return DO_IDivision_Mem(op1, mem_data);
        break;
    }
    default:
        break;
    }
    return 0;
}
void DO_INCorDEC_Reg(sc_lv<32> *op1, sc_lv<8> ModRm, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 0:
    {
        DO_INC_Reg(op1, EFLAGS);
        break;
    }
    case 1:
    {
        DO_DEC_Reg(op1, EFLAGS);
        break;
    }
    default:
        break;
    }
}
sc_lv<32> DO_INCorDEC_Mem(sc_lv<32> mem_data, sc_lv<8> ModRm, sc_lv<32> &EFLAGS)
{
    int mode = ModRm.range(5, 3).to_uint();
    switch (mode)
    {
    case 0:
    {
        return DO_INC_Mem(mem_data, EFLAGS);
        break;
    }
    case 1:
    {
        return DO_DEC_Mem(mem_data, EFLAGS);
        break;
    }
    default:
        break;
    }
    return 0;
}
vector<sc_lv<120>> erase_front(vector<sc_lv<120>> Buffer)
{
    vector<sc_lv<120>> out;
    for (int i = 1; i < Buffer.size(); i++)
    {
        out.push_back(Buffer[i]);
    }
    return out;
}