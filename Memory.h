#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <systemc.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <fstream>
using namespace std;
template <int ADDRESS_SIZE, int DATA_SIZE>
SC_MODULE(Memory)
{
    sc_in<sc_logic> clk;
    sc_in<sc_lv<DATA_SIZE>> data_in;
    sc_in<sc_lv<ADDRESS_SIZE>> address;
    sc_in<sc_logic> read_request;
    sc_in<sc_logic> write_request;
    sc_in<sc_logic> input_valid;
    sc_out<sc_logic> ready;
    sc_out<sc_lv<DATA_SIZE>> data_out;
    sc_lv<DATA_SIZE> data_registers[2000];
    SC_CTOR(Memory)
    {
        initialize_memory();
        SC_THREAD(read_memory);
        sensitive << clk.pos();
        SC_METHOD(write_memory);
        sensitive << clk.pos();
    }
    void initialize_memory();
    void read_memory();
    void write_memory();
};
template <int ADDRESS_SIZE, int DATA_SIZE>
void Memory<ADDRESS_SIZE, DATA_SIZE>::read_memory()
{
    while (true)
    {
        if (read_request == SC_LOGIC_1)
        {
            // cout << "address recieved by memory is : " << address->read().to_uint() << " " << sc_time_stamp() << endl;
            data_out.write(data_registers[(address->read()).to_uint()]);
            // cout << "data sent by memory is: " << data_registers[(address->read()).to_uint()] << endl;
            wait(clk.posedge_event());
        }
        else
            wait(clk.posedge_event());
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE>
void Memory<ADDRESS_SIZE, DATA_SIZE>::write_memory()
{
    if (write_request == SC_LOGIC_1)
    {
        if (input_valid == SC_LOGIC_1)
        {
            data_registers[(address->read()).to_uint()] = data_in;
            cout << "data[" << data_in->read().to_int() << "] written in address[" << address->read().to_uint() << "] at " << sc_time_stamp() << endl;
        }
    }
}
template <int ADDRESS_SIZE, int DATA_SIZE>
void Memory<ADDRESS_SIZE, DATA_SIZE>::initialize_memory()
{
    ifstream file("DATA.txt");
    if (!file.is_open())
    {
        cerr << "Error opening file: "
             << "DATA.txt" << endl;
        return;
    }
    string line;
    int count = 0;
    while (getline(file, line))
    {
        if (line.length() != DATA_SIZE)
        {
            cerr << "Invalid instruction size in line " << count + 1 << endl;
            continue;
        }
        sc_lv<DATA_SIZE> new_data = line.c_str();
        data_registers[count] = new_data;
        count++;
    }
    file.close();
}
#endif