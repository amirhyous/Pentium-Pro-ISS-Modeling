#include "FullSystem.h"
int sc_main(int argc, char *argv[])
{
    FullSystem *UUT = new FullSystem("FULL_SYSTEM_INSTANCE");
    sc_trace_file *VCDFile;
    VCDFile = sc_create_vcd_trace_file("Full_System");
    VCDFile->set_time_unit(1, SC_NS);
    sc_trace(VCDFile, UUT->clk, "clk");
    sc_trace(VCDFile, UUT->read_request, "read_request");
    sc_trace(VCDFile, UUT->write_request, "write_request");
    sc_trace(VCDFile, UUT->mem_ready, "mem_ready");
    sc_trace(VCDFile, UUT->MemoryElement->data_registers[1], "mem1");
    sc_trace(VCDFile, UUT->DataBus_mem_to_cpu, "mem2cpu");
    sc_trace(VCDFile, UUT->AddressBus, "address");
    sc_trace(VCDFile, UUT->Processor->PC, "PC");
    sc_start(20000, SC_NS);
    return 0;
}