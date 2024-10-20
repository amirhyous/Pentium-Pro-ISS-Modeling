#ifndef ISS_HPP
#define ISS_HPP
#pragma once
#include "Requirements.h"
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
SC_MODULE(CPU_ISS)
{
    typedef void (CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::*FunctionPtr)(vector<sc_lv<8>> Instruction_Bytes);
    typedef int (CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::*AddressPtr)();
    typedef int (CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::*LenghtPtr)(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-----------------------PORTS------------------------------------------------------------
    sc_in<sc_logic> clk;
    sc_out<sc_lv<ADDRESS_SIZE>> AddressBus;
    sc_in<sc_lv<DATA_SIZE>> DataBus_in;
    sc_out<sc_lv<DATA_SIZE>> DataBus_out;
    sc_out<sc_logic> read_request, write_request, input_valid;
    sc_in<sc_logic> mem_ready;
    //-----------------------INTERNAL REGISTERS------------------------------------------------
    sc_lv<REGISTER_WIDTH> EAX, EBX, ECX, EDX, EBP, ESI, EDI, ESP; // General Register Set
    sc_lv<SEGMENT_REGISTER_WIDTH> CS, DS, SS, ES, FS, GS;
    sc_lv<EFLAG_WIDTH> EFLAGS;
    sc_lv<EFLAG_WIDTH> *EFLAGS_ptr;
    sc_lv<EIP_WIDTH> EIP;
    sc_lv<MAX_INST_WIDTH> Instruction;
    // needs better implementation
    sc_lv<8> disp8;
    sc_lv<32> disp32;
    sc_lv<8> SIB;
    sc_lv<ADDRESS_SIZE> effective_address;
    sc_lv<32> memory_write_data;
    sc_lv<32> memory_read_data;
    FunctionPtr opcode_1B[16][16];
    AddressPtr Mem_addr_table[24];
    LenghtPtr opcode_1B_length[16][16];
    LenghtPtr opcode_2B_length[16][16];
    LenghtPtr opcode_3B_length[16][16];
    sc_lv<REGISTER_WIDTH> Dummy; // needed for SIB implementation. Doesn't store anything and doesn't have any functionality.
    sc_event Instruction_done, read_done, write_done, read_Inst, read_data, write_data, fetch_done, pre_dec, pre_dec_done;
    sc_lv<32> PC;
    bool called;
    sc_lv<32> Instruction_parts[8];
    vector<sc_lv<120>> Instruction_Buffer;
    bool second_pre_dec;
    //------------------------------------------
    int EAX_addr() { return EAX.to_uint(); }
    int ECX_addr() { return ECX.to_uint(); }
    int EDX_addr() { return EDX.to_uint(); }
    int EBX_addr() { return EBX.to_uint(); }
    int SIB_addr() { return SIB.to_uint(); }
    int disp32_addr() { return disp32.to_uint(); }
    int ESI_addr() { return ESI.to_uint(); }
    int EDI_addr() { return EDI.to_uint(); }
    int EAX_disp8_addr() { return EAX.to_uint() + disp8.to_uint(); }
    int ECX_disp8_addr() { return ECX.to_uint() + disp8.to_uint(); }
    int EDX_disp8_addr() { return EDX.to_uint() + disp8.to_uint(); }
    int EBX_disp8_addr() { return EBX.to_uint() + disp8.to_uint(); }
    int SIB_disp8_addr() { return SIB.to_uint() + disp8.to_uint(); }
    int EBP_disp8_addr() { return EBP.to_uint() + disp8.to_uint(); }
    int ESI_disp8_addr() { return ESI.to_uint() + disp8.to_uint(); }
    int EDI_disp8_addr() { return EDI.to_uint() + disp8.to_uint(); }
    int EAX_disp32_addr() { return EAX.to_uint() + disp32.to_uint(); }
    int ECX_disp32_addr() { return ECX.to_uint() + disp32.to_uint(); }
    int EDX_disp32_addr() { return EDX.to_uint() + disp32.to_uint(); }
    int EBX_disp32_addr() { return EBX.to_uint() + disp32.to_uint(); }
    int SIB_disp32_addr() { return SIB.to_uint() + disp32.to_uint(); }
    int EBP_disp32_addr() { return EBP.to_uint() + disp32.to_uint(); }
    int ESI_disp32_addr() { return ESI.to_uint() + disp32.to_uint(); }
    int EDI_disp32_addr() { return EDI.to_uint() + disp32.to_uint(); }
    //--------------------MAPS---------------------------
    const map<string, sc_lv<REGISTER_WIDTH> &> Register_Map{
        {"EAX", EAX},
        {"EBX", EBX},
        {"ECX", ECX},
        {"EDX", EDX},
        {"EBP", EBP},
        {"ESI", ESI},
        {"EDI", EDI},
        {"ESP", ESP}};
    const map<string, sc_lv<SEGMENT_REGISTER_WIDTH> &> Segment_Map{
        {"CS", CS},
        {"DS", DS},
        {"SS", SS},
        {"ES", ES},
        {"FS", FS},
        {"GS", GS}};
    const map<string, sc_lv<REGISTER_WIDTH> &> Index_Map{
        {"000", EAX},
        {"011", EBX},
        {"001", ECX},
        {"010", EDX},
        {"101", EBP},
        {"110", ESI},
        {"111", EDI},
        {"100", Dummy}};
    const map<string, sc_lv<REGISTER_WIDTH> &> Base_Map{
        {"000", EAX},
        {"001", ECX},
        {"010", EDX},
        {"011", EBX},
        {"100", ESP},
        {"101", Dummy},
        {"110", ESI},
        {"111", EDI}};
    const map<string, int> Address_Map{
        {"[EAX]", 0},
        {"[ECX]", 1},
        {"[EDX]", 2},
        {"[EBX]", 3},
        {"[SIB]", 4},
        {"[disp32]", 5},
        {"[ESI]", 6},
        {"[EDI]", 7},

        {"[EAX+disp8]", 8},
        {"[ECX+disp8]", 9},
        {"[EDX+disp8]", 10},
        {"[EBX+disp8]", 11},
        {"[SIB+disp8]", 12},
        {"[EBP+disp8]", 13},
        {"[ESI+disp8]", 14},
        {"[EDI+disp8]", 15},

        {"[EAX+disp32]", 16},
        {"[ECX+disp32]", 17},
        {"[EDX+disp32]", 18},
        {"[EBX+disp32]", 19},
        {"[SIB+disp32]", 20},
        {"[EBP+disp32]", 21},
        {"[ESI+disp32]", 22},
        {"[EDI+disp32]", 23}};
    //--------------------CONSTRUCTOR----------------
    SC_CTOR(CPU_ISS)
    {
        PC = 1799;
        second_pre_dec = false;
        initialize();
        EFLAGS_ptr = &EFLAGS;
        table1();
        table1_length();
        Address_init();
        SC_THREAD(decoding);
        sensitive << clk.pos();
        SC_THREAD(read_mem);
        sensitive << clk.pos();
        SC_THREAD(write_mem);
        sensitive << clk.pos();
        SC_THREAD(read_Instruction);
        sensitive << clk.pos();
        SC_THREAD(pre_decode);
        sensitive << clk.pos();
        //  SC_METHOD(set_default);
        //  sensitive << clk.pos();
    }
    //-----------------MISC---------------
    void Address_init();
    void decoding();
    void initialize();
    void table1();
    void table1_length();
    void print_register_values();
    void set_default();
    void pre_decode();
    //-----------------IMM to MEM/REG COMMANDS-----------
    void T1_81_IMM_TO_REG(vector<sc_lv<8>> Instruction_Bytes);
    void T1_83_IMM_TO_REG(vector<sc_lv<8>> Instruction_Bytes);
    //-----------------ADD COMMANDS-----------------------
    void T1_00_ADD(vector<sc_lv<8>> Instruction_Bytes);
    void T1_01_ADD(vector<sc_lv<8>> Instruction_Bytes);
    void T1_02_ADD(vector<sc_lv<8>> Instruction_Bytes);
    void T1_03_ADD(vector<sc_lv<8>> Instruction_Bytes);
    void T1_04_ADD(vector<sc_lv<8>> Instruction_Bytes);
    void T1_05_ADD(vector<sc_lv<8>> Instruction_Bytes);
    //-----------------SUB COMMANDS-----------------------
    void T1_29_SUB(vector<sc_lv<8>> Instruction_Bytes);
    void T1_2B_SUB(vector<sc_lv<8>> Instruction_Bytes);
    void T1_2D_SUB(vector<sc_lv<8>> Instruction_Bytes);
    //-------------------AND COMMANDS---------------------
    void T1_20_AND(vector<sc_lv<8>> Instruction_Bytes);
    void T1_21_AND(vector<sc_lv<8>> Instruction_Bytes);
    void T1_22_AND(vector<sc_lv<8>> Instruction_Bytes);
    void T1_23_AND(vector<sc_lv<8>> Instruction_Bytes);
    void T1_24_AND(vector<sc_lv<8>> Instruction_Bytes);
    void T1_25_AND(vector<sc_lv<8>> Instruction_Bytes);
    //--------------------DIVISION & MULTIPICATION COMMANDS----------------------------
    void T1_F7_Unary(vector<sc_lv<8>> Instruction_Bytes);
    //------------------------MOVE---------------------------
    void T1_89_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes);
    void T1_8B_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes);
    void T1_C7_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes);
    void T1_A1_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes);
    void T1_A3_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes);
    //-------------------CMP COMMANDS----------------------
    void T1_38_CMP(vector<sc_lv<8>> Instruction_Bytes);
    void T1_39_CMP(vector<sc_lv<8>> Instruction_Bytes);
    void T1_3A_CMP(vector<sc_lv<8>> Instruction_Bytes);
    void T1_3B_CMP(vector<sc_lv<8>> Instruction_Bytes);
    void T1_3C_CMP(vector<sc_lv<8>> Instruction_Bytes);
    void T1_3D_CMP(vector<sc_lv<8>> Instruction_Bytes);
    //-------------------------------------------------
    void T1_10_ADC(vector<sc_lv<8>> Instruction_Bytes);
    void T1_11_ADC(vector<sc_lv<8>> Instruction_Bytes);
    void T1_30_XOR(vector<sc_lv<8>> Instruction_Bytes);
    //----------------------INC & DEC---------------------------------
    void T1_FE_INCorDEC(vector<sc_lv<8>> Instruction_Bytes);
    void T1_FF_INCorDEC(vector<sc_lv<8>> Instruction_Bytes);
    //-------------------OR COMMANDS----------------------
    void T1_09_OR(vector<sc_lv<8>> Instruction_Bytes);
    void T1_0B_OR(vector<sc_lv<8>> Instruction_Bytes);
    void T1_0D_OR(vector<sc_lv<8>> Instruction_Bytes);
    //************************Length*********************************
    //-----------------IMM to MEM/REG COMMANDS LENGTH-----------
    int T1_81_IMM_TO_REG_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_83_IMM_TO_REG_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);

    //-----------------ADD COMMANDS LENGTH-----------------------
    int T1_00_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_01_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_02_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_03_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_04_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_05_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-----------------SUB COMMANDS LENGTH-----------------------
    int T1_29_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_2B_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_2D_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-------------------AND COMMANDS LENGTH---------------------
    int T1_20_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_21_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_22_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_23_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_24_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_25_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //--------------------DIVISION & MULTIPICATION COMMANDS LENGTH----------------------------
    int T1_F7_Unary_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //------------------------MOVE---------------------------
    int T1_89_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_8B_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_C7_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_A1_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_A3_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-------------------CMP COMMANDS LENGTH----------------------
    int T1_38_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_39_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_3A_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_3B_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_3C_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_3D_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-------------------------------------------------
    int T1_10_ADC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_11_ADC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_30_XOR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //----------------------INC & DEC LENGTH---------------------------------
    int T1_FE_INCorDEC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_FF_INCorDEC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-------------------OR COMMANDS LENGTH----------------------
    int T1_09_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_0B_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    int T1_0D_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location);
    //-----------------IMM and Disp---------------------
    void load_disp8(vector<sc_lv<8>> Instruction_Bytes, int disp_location)
    {
        disp8 = Instruction_Bytes[disp_location];
        cout << "disp8 is :" << disp8.to_uint() << endl;
    }
    void load_disp32(vector<sc_lv<8>> Instruction_Bytes, int disp_location)
    {
        disp32 = (Instruction_Bytes[disp_location], Instruction_Bytes[disp_location + 1], Instruction_Bytes[disp_location + 2], Instruction_Bytes[disp_location + 3]);
        cout << "disp32 is :" << disp32.to_uint() << endl;
    }
    sc_lv<32> load_imm(vector<sc_lv<8>> Instruction_Bytes, int imm_location, bool s);
    int find_imm_location(vector<sc_lv<8>> instruction_bytes);
    void find_disp(vector<sc_lv<8>> instruction_bytes, int mod_location);
    //------------------CONVERT FUNCTIONS-----------------------
    sc_lv<REGISTER_WIDTH> *convert_to_reg(string name);
    sc_lv<SEGMENT_REGISTER_WIDTH> *convert_to_seg(string name);
    int convert_to_address(string name);
    sc_lv<REGISTER_WIDTH> *convert_to_Index(string name);
    sc_lv<REGISTER_WIDTH> *convert_to_Base(string name);
    int convert_to_SIB_address(vector<sc_lv<8>> Instruction_Bytes, int ModRm_location);
    //--------------------MEMORY FUNCTIONS--------------------
    void read_mem();
    void write_mem();
    void read_Instruction();
};

//--------------------------------------------------------------------IMM & DISP-------------------------------------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
sc_lv<32> CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::load_imm(vector<sc_lv<8>> Instruction_Bytes, int imm_location, bool s)
{
    int imm_size = Instruction_Bytes.size() - imm_location;
    sc_lv<32> imm_data;

    switch (imm_size)
    {
    case 1:
    {
        sc_lv<24> dummy0;
        if (s)
        {
            if (Instruction_Bytes[imm_location][7] == 1)
                dummy0 = 0xFFFFFF;
            else
                dummy0 = 0x000000;
        }
        else
            dummy0 = 0x000000;

        imm_data = (dummy0, Instruction_Bytes[imm_location]);
        break;
    }
    case 2:
    {
        sc_lv<16> dummy1;
        if (s)
        {
            if (Instruction_Bytes[imm_location][7] == 1)
                dummy1 = 0xFFFF;
            else
                dummy1 = 0x0000;
        }
        else
            dummy1 = 0x0000;

        imm_data = (dummy1, Instruction_Bytes[imm_location], Instruction_Bytes[imm_location + 1]);
        break;
    }
    case 4:
    {
        imm_data = (Instruction_Bytes[imm_location], Instruction_Bytes[imm_location + 1], Instruction_Bytes[imm_location + 2], Instruction_Bytes[imm_location + 3]);
        break;
    }
    default:
        imm_data = 0;
        break;
    }
    cout << "imm is: " << imm_data.to_int() << endl;
    return (imm_data);
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::find_disp(vector<sc_lv<8>> instruction_bytes, int mod_location)
{
    sc_lv<8> modRM = instruction_bytes[mod_location];
    int mod = modRM.range(7, 6).to_uint();
    switch (mod)
    {
    case 0:
    {
        if (modRM.range(2, 0).to_uint() == 5)
        {
            load_disp32(instruction_bytes, 2);
        }
        break;
    }
    case 1:
    {
        load_disp8(instruction_bytes, 2);
        break;
    }
    case 2:
    {
        load_disp32(instruction_bytes, 2);
    }
    default:
        break;
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::find_imm_location(vector<sc_lv<8>> instruction_bytes)
{
    int opc_location = 0;
    int imm_location = opc_location + 2;
    int SIB_location = opc_location + 2;

    sc_lv<8> ModRM_byte = instruction_bytes[1];
    switch (ModRM_byte.range(7, 6).to_uint())
    {
    case 0:
    {
        if (ModRM_byte.range(2, 0) == "100")
            if (instruction_bytes[SIB_location].range(2, 0) == "101")
                imm_location = opc_location + 7;
            else if (ModRM_byte.range(2, 0) == "101")
                imm_location = opc_location + 7;
    }
    case 1:
    {
        if (ModRM_byte.range(2, 0) == "100")
            imm_location = opc_location + 4;
        else
            imm_location = opc_location + 3;
        break;
    }
    case 2:
    {
        if (ModRM_byte.range(2, 0) == "100")
            imm_location = opc_location + 7;
        else
            imm_location = opc_location + 6;
        break;
    }
    }
    return imm_location;
}
//-------------------------------------------------------------------CONVERT FUNCTIONS--------------------------------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
sc_lv<REGISTER_WIDTH> *CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_reg(string name)
{
    for (auto it = Register_Map.begin(); it != Register_Map.end(); it++)
    {
        if (it->first == name)
            return &(it->second);
    }
    return NULL;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
sc_lv<SEGMENT_REGISTER_WIDTH> *CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_seg(string name)
{
    for (auto it = Segment_Map.begin(); it != Segment_Map.end(); it++)
    {
        if (it->first == name)
            return &(it->second);
    }
    return NULL;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_address(string name)
{
    int index;
    for (auto it = Address_Map.begin(); it != Address_Map.end(); it++)
    {
        if (it->first == name)
        {
            index = it->second;
        }
    }
    int result = (this->*Mem_addr_table[index])();
    return result;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
sc_lv<REGISTER_WIDTH> *CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_Index(string name)
{
    for (auto it = Index_Map.begin(); it != Index_Map.end(); it++)
    {
        if (it->first == name)
            return &(it->second);
    }
    return NULL;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
sc_lv<REGISTER_WIDTH> *CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_Base(string name)
{
    for (auto it = Base_Map.begin(); it != Base_Map.end(); it++)
    {
        if (it->first == name)
            return &(it->second);
    }
    return NULL;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::convert_to_SIB_address(vector<sc_lv<8>> Instruction_Bytes, int ModRm_location)
{
    sc_lv<8> Modrm = Instruction_Bytes[ModRm_location];
    sc_lv<8> SIB = Instruction_Bytes[ModRm_location + 1];
    int scale;
    int scale_mode = SIB.range(7, 6).to_uint();
    switch (scale_mode)
    {
    case 0:
    {
        scale = 1;
        break;
    }
    case 1:
    {
        scale = 2;
        break;
    }
    case 2:
    {
        scale = 4;
        break;
    }
    case 3:
    {
        scale = 8;
        break;
    }
    default:
        scale = 0;
        break;
    }
    cout << "scale : " << scale << endl;
    sc_lv<32> *index_ptr = convert_to_Index(SIB.range(5, 3).to_string());
    int index;
    if (index_ptr == &Dummy)
    {
        index = 0;
    }
    else
    {
        index = index_ptr->to_uint();
    }
    cout << "index is : " << index << endl;
    sc_lv<32> *Base_ptr = convert_to_Base(SIB.range(2, 0).to_string());
    int Base;
    int mode = Modrm.range(7, 6).to_uint();
    if (Base_ptr == &Dummy)
    {
        switch (mode)
        {
        case 0:
        {
            load_disp32(Instruction_Bytes, ModRm_location + 2);
            return scale * index + disp32.to_uint();
            break;
        }
        case 1:
        {
            load_disp8(Instruction_Bytes, ModRm_location + 2);
            return scale * index + disp8.to_uint() + EBP.to_uint();
            break;
        }
        case 2:
        {
            load_disp32(Instruction_Bytes, ModRm_location + 2);
            return scale * index + disp32.to_uint() + EBP.to_uint();
            break;
        }
        default:
            break;
        }
    }
    else
    {
        Base = Base_ptr->to_uint();
        switch (mode)
        {
        case 0:
        {
            return scale * index + Base;
            break;
        }
        case 1:
        {
            load_disp8(Instruction_Bytes, ModRm_location + 2);
            return scale * index + disp8.to_uint() + Base;
            break;
        }
        case 2:
        {
            load_disp32(Instruction_Bytes, ModRm_location + 2);
            return scale * index + disp32.to_uint() + Base;
            break;
        }
        default:
            break;
        }
    }
    return 0;
}
//------------------------------------------------------------------MISC------------------------------------------------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::initialize()
{
    Instruction = "0000000100100000XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    EAX = 1;
    EBX = 20;
    ECX = 30;
    EDX = 40;
    EBP = 50;
    ESI = 60;
    EDI = 70;
    ESP = 80;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::table1()
{
    opcode_1B[0][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_00_ADD;
    opcode_1B[0][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_01_ADD;
    opcode_1B[0][2] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_02_ADD;
    opcode_1B[0][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_03_ADD;
    opcode_1B[0][4] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_04_ADD;
    opcode_1B[0][5] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_05_ADD;

    opcode_1B[2][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_29_SUB;
    opcode_1B[2][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_2B_SUB;
    opcode_1B[2][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_2D_SUB;

    opcode_1B[8][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_81_IMM_TO_REG;
    opcode_1B[8][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_83_IMM_TO_REG;

    opcode_1B[2][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_20_AND;
    opcode_1B[2][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_21_AND;
    opcode_1B[2][2] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_22_AND;
    opcode_1B[2][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_23_AND;
    opcode_1B[2][4] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_24_AND;
    opcode_1B[2][5] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_25_AND;

    opcode_1B[0][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_09_OR;
    opcode_1B[0][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_0B_OR;
    opcode_1B[0][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_0D_OR;

    opcode_1B[3][8] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_38_CMP;
    opcode_1B[3][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_39_CMP;
    opcode_1B[3][10] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3A_CMP;
    opcode_1B[3][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3B_CMP;
    opcode_1B[3][12] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3C_CMP;
    opcode_1B[3][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3D_CMP;

    opcode_1B[15][14] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FE_INCorDEC;
    opcode_1B[15][15] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FF_INCorDEC;

    opcode_1B[15][7] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_F7_Unary;

    opcode_1B[8][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_89_MOV_DATA;
    opcode_1B[8][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_8B_MOV_DATA;
    opcode_1B[12][7] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_C7_MOV_DATA;
    opcode_1B[10][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_A1_MOV_DATA;
    opcode_1B[10][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_A3_MOV_DATA;

    // opcode_1B[1][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_10_ADC;
    // opcode_1B[1][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_11_ADC;
    // opcode_1B[2][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_20_AND;
    // opcode_1B[3][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_30_XOR;
    // opcode_1B[4][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_40_INC;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::table1_length()
{
    opcode_1B_length[0][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_00_ADD_Len;
    opcode_1B_length[0][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_01_ADD_Len;
    opcode_1B_length[0][2] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_02_ADD_Len;
    opcode_1B_length[0][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_03_ADD_Len;
    opcode_1B_length[0][4] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_04_ADD_Len;
    opcode_1B_length[0][5] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_05_ADD_Len;

    opcode_1B_length[2][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_29_SUB_Len;
    opcode_1B_length[2][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_2B_SUB_Len;
    opcode_1B_length[2][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_2D_SUB_Len;

    opcode_1B_length[8][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_81_IMM_TO_REG_Len;
    opcode_1B_length[8][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_83_IMM_TO_REG_Len;

    opcode_1B_length[2][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_20_AND_Len;
    opcode_1B_length[2][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_21_AND_Len;
    opcode_1B_length[2][2] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_22_AND_Len;
    opcode_1B_length[2][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_23_AND_Len;
    opcode_1B_length[2][4] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_24_AND_Len;
    opcode_1B_length[2][5] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_25_AND_Len;

    opcode_1B_length[0][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_09_OR_Len;
    opcode_1B_length[0][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_0B_OR_Len;
    opcode_1B_length[0][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_0D_OR_Len;

    opcode_1B_length[3][8] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_38_CMP_Len;
    opcode_1B_length[3][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_39_CMP_Len;
    opcode_1B_length[3][10] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3A_CMP_Len;
    opcode_1B_length[3][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3B_CMP_Len;
    opcode_1B_length[3][12] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3C_CMP_Len;
    opcode_1B_length[3][13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3D_CMP_Len;

    opcode_1B_length[15][14] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FE_INCorDEC_Len;
    opcode_1B_length[15][15] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FF_INCorDEC_Len;

    opcode_1B_length[15][7] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_F7_Unary_Len;

    opcode_1B_length[8][9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_89_MOV_DATA_Len;
    opcode_1B_length[8][11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_8B_MOV_DATA_Len;
    opcode_1B_length[12][7] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_C7_MOV_DATA_Len;
    opcode_1B_length[10][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_A1_MOV_DATA_Len;
    opcode_1B_length[10][3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_A3_MOV_DATA_Len;

    // opcode_1B[1][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_10_ADC;
    // opcode_1B[1][1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_11_ADC;
    // opcode_1B[2][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_20_AND;
    // opcode_1B[3][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_30_XOR;
    // opcode_1B[4][0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_40_INC;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::print_register_values()
{
    cout << "EAX Register value: " << EAX.to_int() << endl;
    cout << "EBX Register value: " << EBX.to_int() << endl;
    cout << "ECX Register value: " << ECX.to_int() << endl;
    cout << "EDX Register value: " << EDX.to_int() << endl;
    cout << "EBP Register value: " << EBP.to_int() << endl;
    cout << "ESI Register value: " << ESI.to_int() << endl;
    cout << "EDI Register value: " << EDI.to_int() << endl;
    cout << "ESP Register value: " << ESP.to_int() << endl;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::Address_init()
{
    Mem_addr_table[0] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EAX_addr;
    Mem_addr_table[1] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ECX_addr;
    Mem_addr_table[2] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDX_addr;
    Mem_addr_table[3] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EBX_addr;
    Mem_addr_table[4] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::SIB_addr;
    Mem_addr_table[5] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::disp32_addr;
    Mem_addr_table[6] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ESI_addr;
    Mem_addr_table[7] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDI_addr;
    Mem_addr_table[8] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EAX_disp8_addr;
    Mem_addr_table[9] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ECX_disp8_addr;
    Mem_addr_table[10] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDX_disp8_addr;
    Mem_addr_table[11] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EBX_disp8_addr;
    Mem_addr_table[12] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::SIB_disp8_addr;
    Mem_addr_table[13] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EBP_disp8_addr;
    Mem_addr_table[14] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ESI_disp8_addr;
    Mem_addr_table[15] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDI_disp8_addr;
    Mem_addr_table[16] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EAX_disp32_addr;
    Mem_addr_table[17] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ECX_disp32_addr;
    Mem_addr_table[18] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDX_disp32_addr;
    Mem_addr_table[19] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EBX_disp32_addr;
    Mem_addr_table[20] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::SIB_disp32_addr;
    Mem_addr_table[21] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EBP_disp32_addr;
    Mem_addr_table[22] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::ESI_disp32_addr;
    Mem_addr_table[23] = &CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::EDI_disp32_addr;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::set_default()
{
    read_request = SC_LOGIC_0;
    write_request = SC_LOGIC_0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::pre_decode()
{
    while (true)
    {
        wait(pre_dec);
        string Inst_buffer;
        vector<sc_lv<8>> opcode_bytes;
        if (second_pre_dec)
        {
            for (int i = 4; i < 8; i++)
            {
                Inst_buffer += Instruction_parts[i].to_string();
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                Inst_buffer += Instruction_parts[i].to_string();
            }
        }
        while (Inst_buffer.length() != 0) // main pre-decode
        {
            opcode_bytes.clear();
            int inst_length = 0;
            int opcode_location = 0; // location as byte
            // byte count is from left (MSB)
            sc_lv<8> first_byte = Inst_buffer.substr(0, 8).c_str();
            if ((first_byte == 0x66) || (first_byte == 0x67) || (64 < first_byte.to_uint() < 79))
            {
                // it is either rex or prefix.
                // opcode location must be updated.
                // instruction should be updated.
            }
            // detecting opcode
            sc_lv<8> opcode_first_byte = Inst_buffer.substr(opcode_location * 8, 8).c_str();
            inst_length++;
            opcode_bytes.push_back(opcode_first_byte);
            if (opcode_first_byte == 0x0F)
            {
                inst_length++;
                sc_lv<8> opcode_second_byte = Inst_buffer.substr(8 * (opcode_location + 1), 8).c_str();
                opcode_bytes.push_back(opcode_second_byte);
                if (opcode_second_byte == 0x38 || opcode_second_byte == 0x3A)
                {
                    sc_lv<8> opcode_third_byte = Inst_buffer.substr(8 * (opcode_location + 2), 8).c_str();
                    opcode_bytes.push_back(opcode_third_byte);
                    inst_length++;
                }
            }
            switch (opcode_bytes.size())
            {
            case 1:
            {
                int row = opcode_bytes[0].range(7, 4).to_uint();
                int column = opcode_bytes[0].range(3, 0).to_uint();
                inst_length += (this->*opcode_1B_length[row][column])(opcode_bytes, Inst_buffer, inst_length);
                break;
            }
            case 2:
            {
                int row = opcode_bytes[1].range(7, 4).to_uint();
                int column = opcode_bytes[1].range(3, 0).to_uint();
                inst_length += (this->*opcode_2B_length[row][column])(opcode_bytes, Inst_buffer, inst_length);
                break;
            }
            case 3:
            {
                int row = opcode_bytes[2].range(7, 4).to_uint();
                int column = opcode_bytes[2].range(3, 0).to_uint();
                inst_length += (this->*opcode_3B_length[row][column])(opcode_bytes, Inst_buffer, inst_length);
                break;
            }
            default:
                break;
            }
            string effective_instruction = Inst_buffer.substr(0, inst_length * 8);
            int len = 120 - effective_instruction.length();
            for (int i = 0; i < len; i++)
            {
                effective_instruction += 'X';
            }
            sc_lv<120> temp = effective_instruction.c_str();
            cout << "effective inst: " << temp << endl;
            Instruction_Buffer.push_back(temp);
            Inst_buffer.erase(0, inst_length * 8);
        }
        pre_dec_done.notify();
        wait(clk.posedge_event());
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::decoding()
{
    int inst = 0;
    while (true)
    {
        if (PC == 1815)
        {
            if (inst == 7)
                break;
            else
                inst++;
        }
        wait(clk.posedge_event());
        if (Instruction_Buffer.size() == 0)
        {
            if (second_pre_dec)
            {
                pre_dec.notify();
                wait(pre_dec_done);
                second_pre_dec = false;
            }
            else
            {
                read_Inst.notify();
                wait(fetch_done);
                pre_dec.notify();
                wait(pre_dec_done);
                second_pre_dec = true;
            }
        }
        Instruction = Instruction_Buffer[0];
        int inst_len;
        for (int i = MAX_INST_WIDTH; i >= 0; i--)
        {
            if (Instruction[i] == 'X')
            {
                inst_len = (MAX_INST_WIDTH - i) / 8;
                break;
            }
            inst_len = (MAX_INST_WIDTH - i) / 8;
        }
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "Instruction is :" << Instruction << endl;
        vector<sc_lv<8>> Instruction_Bytes;
        for (int i = 0; i < inst_len; i++)
        {
            Instruction_Bytes.push_back(Instruction.range(119 - 8 * i, 112 - 8 * i));
        }

        int opcode_location;
        // will be completed later
        opcode_location = 0;
        int row = Instruction_Bytes[opcode_location].range(7, 4).to_uint();
        int column = Instruction_Bytes[opcode_location].range(3, 0).to_uint();
        (this->*opcode_1B[row][column])(Instruction_Bytes);
        Instruction_Buffer = erase_front(Instruction_Buffer);
        print_register_values();
        wait(clk.posedge_event());
    }
}
//---------------------------------------------------------IMM TO REG/MEM COMMANDS----------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_81_IMM_TO_REG(vector<sc_lv<8>> Instruction_Bytes)
{
    // This instruction is Immediate to Register Addition/Immediate to Memory Addition.
    // Since s=0, Immediate won't be sign extended.
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // IMM to Reg
    {
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        int imm_location = 2;
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        DO_REG_IMM(ModRM_Byte, operand, imm_data, EFLAGS);
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // IMM to Mem (with SIB Byte)
    {
        find_disp(Instruction_Bytes, mod_location);
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // IMM to Mem (without SIB Byte)
    {
        find_disp(Instruction_Bytes, mod_location);
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_83_IMM_TO_REG(vector<sc_lv<8>> Instruction_Bytes)
{
    // This instruction is Immediate to Register Addition/Immediate to Memory Addition.
    // Since s=1, Immediate won't be sign extended.
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // IMM to Reg
    {
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        int imm_location = 2;
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, true);
        DO_REG_IMM(ModRM_Byte, operand, imm_data, EFLAGS);
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // IMM to Mem (with SIB Byte)
    {
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, true);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // IMM to Mem (without SIB Byte)
    {
        find_disp(Instruction_Bytes, mod_location);
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, true);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
//---------------------------------------------------------MEMORY READ/WRITE------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::read_mem()
{
    while (true)
    {
        wait(read_data);
        AddressBus = effective_address;
        read_request = SC_LOGIC_1;
        wait(clk.posedge_event());
        // cout << "address bus at " << sc_time_stamp() << "is: " << AddressBus->read().to_uint() << endl;
        while (mem_ready != SC_LOGIC_1)
            wait(clk.posedge_event());
        wait(clk.posedge_event());
        memory_read_data = DataBus_in->read().range(31, 0);
        read_request = SC_LOGIC_0;
        read_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::write_mem()
{
    while (true)
    {
        wait(write_data);
        AddressBus = effective_address;
        write_request = SC_LOGIC_1;
        while (mem_ready != SC_LOGIC_1)
            wait(clk.posedge_event());
        wait(clk.posedge_event());
        input_valid = SC_LOGIC_1;
        DataBus_out = memory_write_data;
        wait(clk.posedge_event());
        // cout << "current bus is : " << DataBus_out->read().to_uint() << endl;
        write_request = SC_LOGIC_0;
        write_done.notify();
        wait();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::read_Instruction()
{
    while (true)
    {
        wait(read_Inst);
        // flush
        for (int i = 0; i < 8; i++)
        {
            effective_address = PC;
            wait(clk.posedge_event());
            read_data.notify();
            wait(read_done);
            Instruction_parts[i] = memory_read_data;
            PC = PC.to_uint() + 1;
            wait(clk.posedge_event());
        }
        fetch_done.notify();
    }
}
//---------------------------------------------------------ADD COMMANDS-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_00_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    // operating mode not supported in this code
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_01_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Addition(operand1, operand2, operand2, EFLAGS);

        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = DO_Addition_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        // AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_Addition_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_02_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    // operating mode not supported in this code.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_03_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Addition(operand1, operand2, operand1, EFLAGS);

        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        cout << "address is : " << effective_address << endl;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Addition(operand, &memory_read_data, operand, EFLAGS);

        Instruction_done.notify();
    }
    else // simple memory to register without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address;
        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        cout << "address is : " << addr << endl;
        effective_address = addr;
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);
        DO_Addition(operand, &memory_read_data, operand, EFLAGS);
        wait(clk.posedge_event());
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_04_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    // operating mode not supported in this code
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_05_ADD(vector<sc_lv<8>> Instruction_Bytes)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    int imm_location = 1;
    sc_lv<32> imm_data;
    imm_data = load_imm(Instruction_Bytes, imm_location, false);
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    DO_Addition_Imm(EAX_, imm_data.to_int(), EAX_, EFLAGS);

    Instruction_done.notify();
}
//---------------------------------------------------------SUB COMMANDS-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_29_SUB(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Subtraction(operand1, operand2, operand2, EFLAGS);

        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = DO_Subtraction_Mem(operand, memory_read_data, EFLAGS);
        memory_write_data = DO_Addition_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_Subtraction_Mem(operand, memory_read_data, EFLAGS);
        memory_write_data = DO_Addition_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_2B_SUB(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Subtraction(operand1, operand2, operand1, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        cout << "address is : " << effective_address << endl;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Subtraction(operand, &memory_read_data, operand, EFLAGS);

        Instruction_done.notify();
    }
    else // simple memory to register without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);

        effective_address;
        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        cout << "address is : " << addr << endl;
        effective_address = addr;
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_Subtraction(operand, &memory_read_data, operand, EFLAGS);

        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_2D_SUB(vector<sc_lv<8>> Instruction_Bytes)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    int imm_location = 1;
    sc_lv<32> imm_data;
    imm_data = load_imm(Instruction_Bytes, imm_location, false);
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    DO_Subtraction_Imm(EAX_, imm_data.to_int(), EAX_, EFLAGS);
    cout << "sub:" << EAX.to_int() << endl;
    Instruction_done.notify();
}
//---------------------------------------------------------AND COMMANDS-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_20_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    // operating mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_21_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_AND(operand1, operand2, operand2, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = DO_AND_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_AND_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_22_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    // operating mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_23_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_AND(operand1, operand2, operand1, EFLAGS);

        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        cout << "address is : " << effective_address << endl;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_AND(operand, &memory_read_data, operand, EFLAGS);
    }
    else // simple memory to register without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);

        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        cout << "address is : " << addr << endl;
        effective_address = addr;
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_AND(operand, &memory_read_data, operand, EFLAGS);
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_24_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    // mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_25_AND(vector<sc_lv<8>> Instruction_Bytes)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    int imm_location = 1;
    sc_lv<32> imm_data;
    imm_data = load_imm(Instruction_Bytes, imm_location, false);
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    DO_AND_Imm(EAX_, imm_data, EAX_, EFLAGS);
    Instruction_done.notify();
}
//-------------------------------------------------------CMP COMMANDS--------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_38_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    // mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_39_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Compare(*operand1, *operand2, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Compare(memory_read_data, *operand, EFLAGS);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_Compare(memory_read_data, *operand, EFLAGS);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3A_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    // mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3B_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Compare(*operand2, *operand1, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Compare(*operand, memory_read_data, EFLAGS);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_Compare(*operand, memory_read_data, EFLAGS);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3C_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    // mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3D_CMP(vector<sc_lv<8>> Instruction_Bytes)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    int imm_location = 1;
    sc_lv<32> imm_data;
    imm_data = load_imm(Instruction_Bytes, imm_location, false);
    DO_Compare(imm_data, EAX, EFLAGS);
    Instruction_done.notify();
}
//-------------------------------------------------------INC & DEC COMMANDS----------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FE_INCorDEC(vector<sc_lv<8>> Instruction_Bytes)
{
    // mode not supported.
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FF_INCorDEC(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : Increments a register
    {
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        DO_INCorDEC_Reg(operand, ModRM_Byte, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // Increments memory
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        cout << "address is : " << effective_address << endl;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_INCorDEC_Mem(memory_read_data, ModRM_Byte, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // Increments Memory
    {
        find_disp(Instruction_Bytes, mod_location);
        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        cout << "address is : " << addr << endl;
        effective_address = addr;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_INCorDEC_Mem(memory_read_data, ModRM_Byte, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
//--------------------------------------------Unary(DIV & IDIV)--------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_F7_Unary(vector<sc_lv<8>> Instruction_Bytes)
{
    // This instruction is Immediate to Register Div/IDiv to Memory Addition.
    // Since s=0, Immediate won't be sign extended.
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    if (table_index > 192)
    {
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        DO_Unary_Reg(EAX_, operand, ModRM_Byte, EFLAGS);

        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") //  (with SIB Byte)
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location); // check!
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_Unary_Mem(EAX_, memory_read_data, ModRM_Byte, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // IMM to Mem (without SIB Byte)
    {
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]); // check!
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_Unary_Mem(EAX_, memory_read_data, ModRM_Byte, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
//-------------------------------MOVE-------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_89_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Move_Regs(operand1, operand2);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = operand->to_uint();

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = operand->to_uint();

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}

template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_8B_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Move_Regs(operand2, operand1);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_Move_Reg(operand, memory_read_data);
        Instruction_done.notify();
    }
    else // simple memory to register without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);

        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        effective_address = addr;
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_Move_Reg(operand, memory_read_data);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_C7_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // IMM to Reg
    {
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        int imm_location = 2;
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        DO_Move_Reg(operand, imm_data);
        Instruction_done.notify();
        //
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // IMM to Mem (with SIB Byte)
    {
        find_disp(Instruction_Bytes, mod_location);
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        memory_write_data = imm_data;
        // read_mem();
        // memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // IMM to Mem (without SIB Byte)
    {
        find_disp(Instruction_Bytes, mod_location);
        int imm_location = find_imm_location(Instruction_Bytes);
        sc_lv<32> imm_data;
        imm_data = load_imm(Instruction_Bytes, imm_location, false);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        memory_write_data = imm_data;
        // read_mem();
        // memory_write_data = DO_MEM_IMM(ModRM_Byte, memory_read_data, imm_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_A1_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    load_disp32(Instruction_Bytes, 1);
    effective_address = disp32;
    AddressBus = effective_address;
    wait(clk.posedge_event());
    read_request = SC_LOGIC_1;
    wait(read_done);
    DO_Move_Reg(EAX_, memory_read_data);
    Instruction_done.notify();
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_A3_MOV_DATA(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    load_disp32(Instruction_Bytes, 1);
    effective_address = disp32;
    memory_write_data = EAX;
    cout << "address is: " << endl;
    write_request = SC_LOGIC_1;
    wait(write_done);
    Instruction_done.notify();
}
//----------------------------------------------------------OR--------------------------------------------------------------------////////////////////////check!!!!!!
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_09_OR(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_OR(operand1, operand2, operand2, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        memory_write_data = DO_OR_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
    else // simple register to memory without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);
        effective_address = convert_to_address(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        memory_write_data = DO_OR_Mem(operand, memory_read_data, EFLAGS);

        write_data.notify();
        wait(write_done);
        Instruction_done.notify();
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_0B_OR(vector<sc_lv<8>> Instruction_Bytes)
{
    int mod_location = 1; // needs completion
    sc_lv<8> ModRM_Byte = Instruction_Bytes[mod_location];
    int table_index = ModRM_Byte.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        sc_lv<REGISTER_WIDTH> *operand1 = convert_to_reg(Operand_32b_MODRM[table_index][0]);
        sc_lv<REGISTER_WIDTH> *operand2 = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_OR(operand1, operand2, operand1, EFLAGS);
        Instruction_done.notify();
    }
    else if ((ModRM_Byte.range(7, 6) == "00" || ModRM_Byte.range(7, 6) == "01" || ModRM_Byte.range(7, 6) == "10") && ModRM_Byte.range(2, 0) == "100") // SIB needed
    {
        effective_address = convert_to_SIB_address(Instruction_Bytes, mod_location);
        cout << "address is : " << effective_address << endl;
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        DO_OR(operand, &memory_read_data, operand, EFLAGS);

        Instruction_done.notify();
    }
    else // simple memory to register without SIB byte
    {
        find_disp(Instruction_Bytes, mod_location);

        int addr;
        addr = convert_to_address(Operand_32b_MODRM[table_index][0]);
        cout << "address is : " << addr << endl;
        effective_address = addr;
        sc_lv<REGISTER_WIDTH> *operand = convert_to_reg(Operand_32b_MODRM[table_index][1]);
        AddressBus = effective_address;
        wait(clk.posedge_event());
        read_data.notify();
        wait(read_done);

        DO_OR(operand, &memory_read_data, operand, EFLAGS);

        Instruction_done.notify();
    }
}

template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
void CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_0D_OR(vector<sc_lv<8>> Instruction_Bytes)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    int imm_location = 1;
    sc_lv<32> imm_data;
    imm_data = load_imm(Instruction_Bytes, imm_location, false);
    sc_lv<REGISTER_WIDTH> *EAX_;
    EAX_ = &EAX;
    DO_OR_Imm(EAX_, imm_data, EAX_, EFLAGS);
    Instruction_done.notify();
}

//**********************************************************LENGTH FUNCTIONS************************************************************
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_81_IMM_TO_REG_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // This instruction is Immediate to Register Addition/Immediate to Memory Addition.
    // Since s=0, Immediate won't be sign extended ,Scince S = 0 imm is 32 bit.
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 10; //[*] + 32'b imm
            }
            else
                return 6; // SIB + ModRM + 32'b imm
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 7; // ModRM + SIB + Disp8 + 32'b imm
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 10; // ModRM + SIB + Disp32 + 32'b imm
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 9; // disp32: exp + 32'b imm
            }
            else
                return 5; // ModRM + 32'b imm
        }
        else if (ModRM.range(7, 6) == "01")
            return 6; // ModRM + disp8 + 32'b imm

        else if (ModRM.range(7, 6) == "10")
            return 9; // ModRM + disp32 + 32'b imm
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_83_IMM_TO_REG_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // This instruction is Immediate to Register Addition/Immediate to Memory Addition.
    // Since s=1, Immediate won't be sign extended.
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 7; //[*] + 8'b imm
            }
            else
                return 3; // SIB + ModRM + 8'b imm
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 4; // ModRM + SIB + Disp8 + 8'b imm
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 7; // ModRM + SIB + Disp32 + 8'b imm
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 6; // disp32: exp + 8'b imm
            }
            else
                return 2; // ModRM + 8'b imm
        }
        else if (ModRM.range(7, 6) == "01")
            return 3; // ModRM + disp8 + 8'b imm

        else if (ModRM.range(7, 6) == "10")
            return 6; // ModRM + disp32 + 8'b imm
    }
    return 0;
}
//---------------------------------------------------------ADD COMMANDS LENGTH-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_00_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // operating mode not supported in this code
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_01_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_02_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // operating mode not supported in this code.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_03_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6;
            }
            else
                return 2;
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3;
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6;
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5;
            }
            else
                return 1;
        }
        else if (ModRM.range(7, 6) == "01")
            return 2;
        else if (ModRM.range(7, 6) == "10")
            return 5;
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_04_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // operating mode not supported in this code
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_05_ADD_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    sc_lv<8> opcode = opcode_bytes[0];
    if (opcode[0] == SC_LOGIC_0)
        return 1; // immediate 8 bit
    else if (opcode[1] == SC_LOGIC_1)
        return 4; // immediate 32 bit
}
//---------------------------------------------------------SUB COMMANDS LENGTH-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_29_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_2B_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6;
            }
            else
                return 2;
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3;
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6;
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5;
            }
            else
                return 1;
        }
        else if (ModRM.range(7, 6) == "01")
            return 2;
        else if (ModRM.range(7, 6) == "10")
            return 5;
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_2D_SUB_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    sc_lv<8> opcode = opcode_bytes[0];
    if (opcode[0] == SC_LOGIC_0)
        return 1; // immediate 8 bit
    else if (opcode[1] == SC_LOGIC_1)
        return 4; // immediate 32 bit
}
//---------------------------------------------------------AND COMMANDS LENGTH-----------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_20_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // operating mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_21_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_22_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // operating mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_23_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_24_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_25_AND_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    sc_lv<8> opcode = opcode_bytes[0];
    if (opcode[0] == SC_LOGIC_0)
        return 1; // immediate 8 bit
    else if (opcode[1] == SC_LOGIC_1)
        return 4; // immediate 32 bit
}
//-------------------------------------------------------CMP COMMANDS LENGTH--------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_38_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_39_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3A_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3B_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
            return 0;
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3C_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_3D_CMP_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    sc_lv<8> opcode = opcode_bytes[0];
    if (opcode[0] == SC_LOGIC_0)
        return 1; // immediate 8 bit
    else if (opcode[1] == SC_LOGIC_1)
        return 4; // immediate 32 bit
}
//-------------------------------------------------------INC & DEC COMMANDS LENGTH----------------------------------------------------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FE_INCorDEC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // mode not supported.
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::T1_FF_INCorDEC_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();
    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
            return 0;
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
//--------------------------------------------Unary(DIV & IDIV) LENGTH--------------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_F7_Unary_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
//-------------------------------MOVE LENGTH-------------------------
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_89_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}

template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_8B_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_C7_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // This instruction is Immediate to Register Addition/Immediate to Memory Addition.
    // Since s=0, Immediate won't be sign extended ,Scince S = 0 imm is 32 bit.
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 5;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 10; //[*] + 32'b imm
            }
            else
                return 6; // SIB + ModRM + 32'b imm
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 7; // ModRM + SIB + Disp8 + 32'b imm
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 10; // ModRM + SIB + Disp32 + 32'b imm
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 9; // disp32: exp + 32'b imm
            }
            else
                return 5; // ModRM + 32'b imm
        }
        else if (ModRM.range(7, 6) == "01")
            return 6; // ModRM + disp8 + 32'b imm

        else if (ModRM.range(7, 6) == "10")
            return 9; // ModRM + disp32 + 32'b imm
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_A1_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    return 4; // 32'b disp
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_A3_MOV_DATA_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    return 4; // 32'b disp
}
//----------------------------------------------------------OR LENGTH--------------------------------------------------------------------////////////////////////check!!!!!!
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_09_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(5, 3) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}
template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_0B_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    sc_lv<8> opcode = opcode_bytes[0];
    sc_lv<8> ModRM = Inst_Buffer.substr(8 * location, 8).c_str();

    int table_index = ModRM.to_uint();
    if (table_index > 192) // Mod = 11 : simple register to register Instruction
    {
        return 1;
    }
    else if ((ModRM.range(7, 6) == "00" || ModRM.range(7, 6) == "01" || ModRM.range(7, 6) == "10") && ModRM.range(2, 0) == "100") // SIB needed
    {
        sc_lv<8> SIB = Inst_Buffer[8 * (location + 1), 8 * (location + 1) + 7];
        if (ModRM.range(7, 6) == "00")
        {
            if (SIB.range(2, 0) == "101")
            {
                return 6; //[*]
            }
            else
                return 2; // SIB + ModRM
        }
        else if (ModRM.range(7, 6) == "01")
        {
            return 3; // ModRM + SIB + Disp8
        }
        else if (ModRM.range(7, 6) == "10")
        {
            return 6; // ModRM + SIB + Disp32
        }
    }
    else // simple register to memory without SIB byte
    {
        if (ModRM.range(7, 6) == "00")
        {
            if (ModRM.range(2, 0) == "101")
            {
                return 5; // disp32: exp
            }
            else
                return 1; // ModRM
        }
        else if (ModRM.range(7, 6) == "01")
            return 2; // ModRM + disp8

        else if (ModRM.range(7, 6) == "10")
            return 5; // ModRM + disp32
    }
    return 0;
}

template <int ADDRESS_SIZE, int DATA_SIZE, int REGISTER_WIDTH, int SEGMENT_REGISTER_WIDTH, int EFLAG_WIDTH, int EIP_WIDTH, int MAX_INST_WIDTH>
int CPU_ISS<ADDRESS_SIZE, DATA_SIZE, REGISTER_WIDTH, SEGMENT_REGISTER_WIDTH, EFLAG_WIDTH, EIP_WIDTH, MAX_INST_WIDTH>::
    T1_0D_OR_Len(vector<sc_lv<8>> opcode_bytes, string Inst_Buffer, int location)
{
    // Immediate to AL,AX or EAX. Since we are doing 32 bit operation, we use EAX.
    // Since this instruction doesn't need ModR/M byte, the next Bytes are Immediate data.
    sc_lv<8> opcode = opcode_bytes[0];
    if (opcode[0] == SC_LOGIC_0)
        return 1; // immediate 8 bit
    else if (opcode[1] == SC_LOGIC_1)
        return 4; // immediate 32 bit
}
//----------------------------------------------------------INITIALIZE------------------------------------------------------------------

/*int sc_main(int argc, char *argv[])
{
    CPU_ISS<64, 32, 32, 16, 32, 32, 120> UUT("UUT");
    UUT.initialize();
    // cout << endl;
    // cout << UUT.Instruction << endl;
    // UUT.decoding();
    return 0;
}*/
#endif