#ifndef BASEFUNCTIONS_HPP
#define BASEFUNCTIONS_HPP
#pragma once
#include <string>
#include <systemc.h>
using namespace std;
//--------------------------------------------------------------------------------
bool check_parity(sc_lv<32> operand);
void set_Eflags(sc_lv<32> operand1, sc_lv<32> operand2, sc_lv<32> result, sc_lv<32> &EFLAGS, int mode, string flags_status);
//---------------------------------ADD COMMANDS---------------------------------------
void DO_Addition(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS);
void DO_AdditionC(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<1> carry, sc_lv<32> *result, sc_lv<32> &EFLAGS);
void DO_Addition_Imm(sc_lv<32> *op1, int imm, sc_lv<32> *result, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Addition_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Addition_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//--------------------------------SUB COMMANDS----------------------------------------
void DO_Subtraction(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS);
void DO_Subtraction_Imm(sc_lv<32> *op1, int imm, sc_lv<32> *result, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Subtraction_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Subtraction_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//--------------------------------AND COMMANDS----------------------------------------
void DO_AND(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS);
void DO_AND_Imm(sc_lv<32> *op1, sc_lv<32> imm_data, sc_lv<32> *result, sc_lv<32> &EFLAGS);
sc_lv<32> DO_AND_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_AND_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//----------------------------OR COMMANDS----------------------------------
void DO_OR(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS);
void DO_OR_Imm(sc_lv<32> *op1, sc_lv<32> imm_data, sc_lv<32> *result, sc_lv<32> &EFLAGS);
sc_lv<32> DO_OR_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_OR_Imm_Mem(sc_lv<32> imm_data, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//-------------------------------COMPARE COMMANDS-------------------------------------
void DO_Compare(sc_lv<32> op1, sc_lv<32> op2, sc_lv<32> &EFLAGS);
//-------------------------------INC & DEC COMMANDS--------------------------
void DO_INC_Reg(sc_lv<32> *operand, sc_lv<32> &EFLAGS);
sc_lv<32> DO_INC_Mem(sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
void DO_DEC_Reg(sc_lv<32> *operand, sc_lv<32> &EFLAGS);
sc_lv<32> DO_DEC_Mem(sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//------------------------------------MOVE----------------------------------
void DO_Move_Regs(sc_lv<32> *operand1, sc_lv<32> *operand2);
void DO_Move_Reg(sc_lv<32> *operand, sc_lv<32> mem_data);
//--------------------------------DIVISION & MULTIPICATION COMMAND--------------------------------
void DO_Division(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result);
sc_lv<32> DO_Division_Mem(sc_lv<32> *op1, sc_lv<32> mem_data);
void DO_IDivision(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result);
sc_lv<32> DO_IDivision_Mem(sc_lv<32> *op1, sc_lv<32> mem_data);
void DO_Multiplication(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Multiplication_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<32> &EFLAGS);
//---------------------------------------------------------------------

void DO_XOR(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<32> *result);

#endif