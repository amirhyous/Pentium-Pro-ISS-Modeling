#include "BaseFunctions.h"
//--------------------------------------------------------------------------------
bool check_parity(sc_lv<32> operand)
{
    int count = 0;
    for (int i = 0; i < operand.length(); ++i)
    {
        if (operand[i] == '1')
        {
            count++;
        }
    }
    return (count % 2) == 0;
}
void set_Eflags(sc_lv<32> operand1, sc_lv<32> operand2, sc_lv<32> result, sc_lv<32> &EFLAGS, int mode, string flags_status) //--> flags: false: 0, true: 1, clear: 2
{                                                                                                                           //--> mode: 1: ADD, 2: SUB, 3: AND, 4: OR, 5: MUL,
    int OF = flags_status[0], SF = flags_status[1], ZF = flags_status[2], AF = flags_status[3], PF = flags_status[4],
        CF = flags_status[5], TF = flags_status[6], IF = flags_status[7], DF = flags_status[8], NT = flags_status[9], RF = flags_status[10];
    // ZF (Zero Flag)
    if (ZF)
    {
        if (result.to_uint() == 0)
            EFLAGS[6] = 1;
        else
            EFLAGS[6] = 0;
    }
    // SF (Sign Flag)
    if (SF)
    {
        if (result[31] == 1)
            EFLAGS[7] = 1;
        else
            EFLAGS[7] = 0;
    }
    if (CF)
    {
        // CF (Carry Flag)
    }
    if (PF)
    {
        EFLAGS[2] = check_parity(result);
    }
    // OF (Overflow Flag)
    if (OF)
    {
        if (mode == 1) // Add
        {
            if ((operand1[31] == 0) && (operand2[31] == 0) && (result[31] == 1))
                EFLAGS[11] = 1;
            else if ((operand1[31] == 1) && (operand2[31] == 1) && (result[31] == 0))
                EFLAGS[11] = 1;
            else
                EFLAGS[11] = 0;
        }
        else if (mode == 2) // Subtract
        {
            if (((operand1[31] ^ operand2[31]) == 1) && ((result[31] ^ operand1[31]) == 1))
                EFLAGS[11] = 1;
            else
                EFLAGS[11] = 0;
        }
    }
    cout << "EFLAGS UPDATE : " << EFLAGS << endl;
}
//---------------------------------ADD COMMANDS---------------------------------------
void DO_Addition(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "Addition Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is : " << op2->to_int() << endl;
    *result = op1->to_int() + op2->to_int();
    cout << "result is : " << result->to_int() << endl;
    set_Eflags(*op1, *op2, *result, EFLAGS, 1, "11111100000");
}
// void DO_AdditionC(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<1> carry, sc_lv<32> *result, sc_lv<32> &EFLAGS)
// {
//     *result = op1->to_int() + op2->to_int() + carry.to_uint();
//     set_Eflags(*op1, *op2, *result, EFLAGS, 1, "11111100000");
//     cout << "result is : " << result->to_int() << endl;
// }
void DO_Addition_Imm(sc_lv<32> *op1, int imm, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "Addition Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is Immediate : " << imm << endl;
    *result = op1->to_int() + imm;
    sc_lv<32> imm_data = imm;
    set_Eflags(*op1, imm_data, *result, EFLAGS, 1, "11111100000");
    cout << "result is : " << result->to_int() << endl;
}
sc_lv<32> DO_Addition_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Addition Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is : " << mem_data.to_int() << endl;
    sc_lv<32> result = op1->to_int() + mem_data.to_int();
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(*op1, mem_data, result, EFLAGS, 1, "11111100000");
    return result;
}
sc_lv<32> DO_Addition_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Addition Called" << endl;
    cout << "operand 1 is memory data : " << mem_data.to_int() << endl;
    cout << "operand 2 is Immediate : " << imm_data.to_int() << endl;
    sc_lv<32> result = imm_data.to_int() + mem_data.to_int();
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(imm_data, mem_data, result, EFLAGS, 1, "11111100000");
    return result;
}
//--------------------------------------SUB COMMANDS-----------------------------------------
void DO_Subtraction(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "Subtraction Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is : " << op2->to_int() << endl;
    *result = op1->to_int() - op2->to_int();
    set_Eflags(*op1, *op2, *result, EFLAGS, 2, "11111100000");
    cout << "result is : " << result->to_int() << endl;
}
void DO_Subtraction_Imm(sc_lv<32> *op1, int imm, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "Subtraction Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is Immediate : " << imm << endl;
    *result = op1->to_int() - imm;
    sc_lv<32> imm_data = imm;
    set_Eflags(*op1, imm_data, *result, EFLAGS, 2, "11111100000");
    cout << "result is : " << result->to_int() << endl;
}
sc_lv<32> DO_Subtraction_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Subtraction Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is : " << mem_data.to_int() << endl;
    sc_lv<32> result = op1->to_int() - mem_data.to_int();
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(*op1, mem_data, result, EFLAGS, 2, "11111100000");
    return result;
}
sc_lv<32> DO_Subtraction_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Subtraction Called" << endl;
    cout << "operand 1 is memory data : " << mem_data.to_int() << endl;
    cout << "operand 2 is Immediate : " << imm_data.to_int() << endl;
    sc_lv<32> result = imm_data.to_int() - mem_data.to_int();
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(imm_data, mem_data, result, EFLAGS, 2, "11111100000");
    return result;
}
//--------------------------------AND COMMANDS----------------------------------------
void DO_AND(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "AND Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is : " << *op2 << endl;
    *result = *op1 & *op2;
    cout << "result is : " << *result << endl;
    set_Eflags(*op1, *op2, *result, EFLAGS, 3, "21101200000");
}
void DO_AND_Imm(sc_lv<32> *op1, sc_lv<32> imm_data, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "AND Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is Immediate : " << imm_data << endl;
    *result = *op1 & imm_data;
    cout << "result is : " << result << endl;
    set_Eflags(*op1, imm_data, *result, EFLAGS, 3, "21101200000");
}
sc_lv<32> DO_AND_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "AND Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is memory data : " << mem_data << endl;
    sc_lv<32> result = *op1 & mem_data;
    cout << "result is : " << result << endl;
    set_Eflags(*op1, mem_data, result, EFLAGS, 3, "21101200000");
    return result;
}
sc_lv<32> DO_AND_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "AND Called" << endl;
    cout << "operand 1 is memory data : " << mem_data << endl;
    cout << "operand 2 is Immediate : " << imm_data << endl;
    sc_lv<32> result = imm_data & mem_data;
    cout << "result is : " << result << endl;
    set_Eflags(imm_data, mem_data, result, EFLAGS, 3, "21101200000");
    return result;
}
//----------------------------OR COMMANDS----------------------------------
void DO_OR(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "OR Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is : " << *op2 << endl;
    *result = *op1 | *op2;
    cout << "result is : " << *result << endl;
    set_Eflags(*op1, *op2, *result, EFLAGS, 3, "21101200000");
}
void DO_OR_Imm(sc_lv<32> *op1, sc_lv<32> imm_data, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "OR Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is Immediate : " << imm_data << endl;
    *result = *op1 | imm_data;
    cout << "result is : " << result << endl;
    set_Eflags(*op1, imm_data, *result, EFLAGS, 3, "21101200000");
}
sc_lv<32> DO_OR_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "OR Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is memory data : " << mem_data << endl;
    sc_lv<32> result = *op1 | mem_data;
    cout << "result is : " << result << endl;
    set_Eflags(*op1, mem_data, result, EFLAGS, 3, "21101200000");
    return result;
}
sc_lv<32> DO_OR_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "OR Called" << endl;
    cout << "operand 1 is memory data : " << mem_data << endl;
    cout << "operand 2 is Immediate : " << imm_data << endl;
    sc_lv<32> result = imm_data | mem_data;
    cout << "result is : " << result << endl;
    set_Eflags(imm_data, mem_data, result, EFLAGS, 3, "21101200000");
    return result;
}
//-------------------------------COMPARE COMMANDS-------------------------------------
void DO_Compare(sc_lv<32> op1, sc_lv<32> op2, sc_lv<32> &EFLAGS)
{
    cout << "Compare Called" << endl;
    cout << "operand 1 is : " << op1.to_int() << endl;
    cout << "operand 2 is : " << op2.to_int() << endl;
    sc_lv<32> result = op1.to_int() - op2.to_int();
    set_Eflags(op1, op2, result, EFLAGS, 2, "11111100000");
}
//-------------------------------INC & DEC COMMANDS--------------------------
void DO_INC_Reg(sc_lv<32> *operand, sc_lv<32> &EFLAGS)
{
    cout << "Increment Called" << endl;
    cout << "operand is : " << operand->to_int() << endl;
    sc_lv<32> operand_prev = *operand;
    sc_lv<32> one = 1;
    *operand = operand->to_int() + 1;
    cout << "result is : " << operand->to_int() << endl;
    set_Eflags(operand_prev, one, *operand, EFLAGS, 1, "11111100000");
}
sc_lv<32> DO_INC_Mem(sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Increment Called" << endl;
    cout << "operand is memory data : " << mem_data.to_int() << endl;
    sc_lv<32> one = 1;
    sc_lv<32> result = mem_data.to_int() + 1;
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(mem_data, one, result, EFLAGS, 1, "11111100000");
    return result;
}

void DO_DEC_Reg(sc_lv<32> *operand, sc_lv<32> &EFLAGS)
{
    cout << "Decrement Called" << endl;
    cout << "operand is : " << operand->to_int() << endl;
    sc_lv<32> operand_prev = *operand;
    sc_lv<32> one = 1;
    *operand = operand->to_int() - 1;
    cout << "result is : " << operand->to_int() << endl;
    set_Eflags(operand_prev, one, *operand, EFLAGS, 2, "11111100000");
}
sc_lv<32> DO_DEC_Mem(sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Decrement Called" << endl;
    cout << "operand is memory data : " << mem_data.to_int() << endl;
    sc_lv<32> one = 1;
    sc_lv<32> result = mem_data.to_int() - 1;
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(mem_data, one, result, EFLAGS, 2, "11111100000");
    return result;
}
//------------------------------------MOVE----------------------------------
void DO_Move_Regs(sc_lv<32> *operand1, sc_lv<32> *operand2)
{
    cout << "Move Called" << endl;
    cout << "Source value : " << operand1->to_uint() << endl;
    cout << "Destination previous value: " << operand2->to_uint() << endl;
    *operand2 = operand1->to_uint();
    cout << "Destination current value: " << operand2->to_uint() << endl;
}
void DO_Move_Reg(sc_lv<32> *operand, sc_lv<32> mem_data)
{
    cout << "Move Called" << endl;
    cout << "Source value : " << mem_data.to_uint() << endl;
    cout << "Destination previous value: " << operand->to_uint() << endl;
    *operand = mem_data;
    cout << "Destination current value: " << operand->to_uint() << endl;
}
//--------------------------------DIVISION & MULTIPICATION COMMAND--------------------------------
void DO_Division(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result)
{
    cout << "Division Called" << endl;
    cout << "operand 1 is : " << op1->to_uint() << endl;
    cout << "operand 2 is : " << op2->to_uint() << endl;
    *result = op1->to_uint() / op2->to_uint();
    cout << "result is : " << result->to_uint() << endl;
}

sc_lv<32> DO_Division_Mem(sc_lv<32> *op1, sc_lv<32> mem_data)
{
    cout << "Division Called" << endl;
    cout << "operand 1 is : " << op1->to_uint() << endl;
    cout << "operand 2 is memory data : " << mem_data.to_uint() << endl;
    sc_lv<32> result = op1->to_uint() / mem_data.to_uint();
    cout << "result is : " << result.to_uint() << endl;
    return result;
}

void DO_IDivision(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result)
{
    cout << "Signed Division Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is : " << op2->to_int() << endl;
    *result = op1->to_int() / op2->to_int();
    cout << "result is : " << result->to_int() << endl;
}

sc_lv<32> DO_IDivision_Mem(sc_lv<32> *op1, sc_lv<32> mem_data)
{
    cout << "Signed Division Called" << endl;
    cout << "operand 1 is : " << op1->to_int() << endl;
    cout << "operand 2 is memory data : " << mem_data.to_int() << endl;
    sc_lv<32> result = op1->to_int() / mem_data.to_int();
    cout << "result is : " << result.to_int() << endl;
    return result;
}

void DO_Multiplication(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS)
{
    cout << "Multiplication Called" << endl;
    cout << "operand 1 is : " << op1->to_uint() << endl;
    cout << "operand 2 is : " << op2->to_uint() << endl;
    *result = op1->to_uint() * op2->to_uint();
    cout << "result is : " << result->to_uint() << endl;
    set_Eflags(*op1, *op2, *result, EFLAGS, 5, "10000100000");
}

sc_lv<32> DO_Multiplication_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS)
{
    cout << "Multiplication Called" << endl;
    cout << "operand 1 is : " << op1->to_uint() << endl;
    cout << "operand 2 is memory data : " << mem_data.to_uint() << endl;
    sc_lv<32> result = op1->to_uint() * mem_data.to_uint();
    cout << "result is : " << result.to_int() << endl;
    set_Eflags(*op1, mem_data, result, EFLAGS, 5, "10000100000");
    return result;
}

//---------------------------------------------------------------------

void DO_XOR(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result)
{
    cout << "XOR Called" << endl;
    cout << "operand 1 is : " << *op1 << endl;
    cout << "operand 2 is : " << *op2 << endl;
    *result = *op1 ^ *op2;
    cout << "result is : " << *result << endl;
}