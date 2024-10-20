#ifndef REQUIREMENTS_HPP
#define REQUIREMENTS_HPP
#include <systemc.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <fstream>
#include "BaseFunctions.h"
using namespace std;
extern string Operand_32b_MODRM[256][2];
extern string Address_32b_SIB[256][2];
vector<sc_lv<120>> readInstructionsFromFile(const string &filename);
void DO_REG_IMM(sc_lv<8> ModRm, sc_lv<32> *operand, sc_lv<32> imm_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_MEM_IMM(sc_lv<8> ModRm, sc_lv<32> mem_data, sc_lv<32> imm_data, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Unary_Reg(sc_lv<32> *op1, sc_lv<32> *op2, sc_lv<8> ModRm, sc_lv<32> &EFLAGS);
sc_lv<32> DO_Unary_Mem(sc_lv<32> *op1, sc_lv<32> mem_data, sc_lv<8> ModRm, sc_lv<32> &EFLAGS);
void DO_INCorDEC_Reg(sc_lv<32> *op1, sc_lv<8> ModRm, sc_lv<32> &EFLAGS);
sc_lv<32> DO_INCorDEC_Mem(sc_lv<32> mem_data, sc_lv<8> ModRm, sc_lv<32> &EFLAGS);
vector<sc_lv<120>> erase_front(vector<sc_lv<120>> Buffer);
#endif