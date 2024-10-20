#ifndef MEMORYTB_CPP
#define MEMORYTB_CPP
#include "Memory.h"
SC_MODULE(MEMORYTB)
{
    sc_signal<sc_logic> clk, read_request, write_request, input_valid, ready;
    sc_signal<sc_lv<32>> data_in, address_in, data_out;
    Memory<32, 32> *UUT;
    SC_CTOR(MEMORYTB)
    {
        UUT = new Memory<32, 32>("memory_instance");
        (*UUT)(clk, data_in, address_in, read_request, write_request, input_valid, ready, data_out);

        SC_THREAD(clocking);
        SC_THREAD(scenario);
    }
    void clocking();
    void scenario();
};
void MEMORYTB::clocking()
{
    while (true)
    {
        clk = SC_LOGIC_0;
        wait(10, SC_NS);
        clk = SC_LOGIC_1;
        wait(10, SC_NS);
    }
}
void MEMORYTB::scenario()
{
    while (true)
    {
        wait(10, SC_NS);
        read_request = sc_logic_1;
        data_in = 200;
        address_in = 2;
        wait(30, SC_NS);
        read_request = sc_logic_0;
        input_valid = SC_LOGIC_1;
        write_request = sc_logic_1;
        address_in = 5;
        data_in = 255;
        wait(20, SC_NS);
        write_request = SC_LOGIC_0;
        read_request = SC_LOGIC_1;
        address_in = 5;
        wait(30, SC_NS);
    }
}
/*int sc_main(int argc, char *argv[])
{
    MEMORYTB *TOP = new MEMORYTB("TB_INSTANCE");

    sc_trace_file *VCDFile;
    VCDFile = sc_create_vcd_trace_file("MEMORYTB");
    sc_trace(VCDFile, TOP->clk, "clk");
    sc_trace(VCDFile, TOP->read_request, "read_request");
    sc_trace(VCDFile, TOP->write_request, "write_request");
    sc_trace(VCDFile, TOP->input_valid, "input_valid");
    sc_trace(VCDFile, TOP->ready, "ready");
    sc_trace(VCDFile, TOP->data_in, "data_in");
    sc_trace(VCDFile, TOP->data_out, "data_out");
    sc_trace(VCDFile, TOP->address_in, "address_in");
    sc_start(150, SC_NS);
    return 0;
}*/
#endif