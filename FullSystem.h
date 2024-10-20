#ifndef FULLSYSTEM_CPP
#define FULLSYSTEM_CPP
#include "ISS.h"
#include "Memory.h"
SC_MODULE(FullSystem)
{
    sc_signal<sc_logic, SC_MANY_WRITERS> clk, read_request, write_request, input_valid, mem_ready;
    sc_signal<sc_lv<32>, SC_MANY_WRITERS> AddressBus, DataBus_cpu_to_mem, DataBus_mem_to_cpu;
    CPU_ISS<32, 32, 32, 16, 32, 32, 120> *Processor;
    Memory<32, 32> *MemoryElement;
    SC_CTOR(FullSystem)
    {
        Processor = new CPU_ISS<32, 32, 32, 16, 32, 32, 120>("CPU_INSTANCE");
        (*Processor)(clk, AddressBus, DataBus_mem_to_cpu, DataBus_cpu_to_mem, read_request, write_request, input_valid, mem_ready);
        MemoryElement = new Memory<32, 32>("MEMORY_INSTANCE");
        (*MemoryElement)(clk, DataBus_cpu_to_mem, AddressBus, read_request, write_request, input_valid, mem_ready, DataBus_mem_to_cpu);
        SC_THREAD(clocking);
        SC_THREAD(scenario);
    }
    void clocking();
    void scenario();
};

// #include "FullSystem.h"
void FullSystem::clocking()
{
    while (true)
    {
        clk = SC_LOGIC_0;
        wait(10, SC_NS);
        clk = SC_LOGIC_1;
        wait(10, SC_NS);
    }
}
void FullSystem::scenario()
{
    mem_ready = SC_LOGIC_1;
}

#endif