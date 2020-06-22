#ifndef __SIM_MEM
#define __SIM_MEM
#include <iostream>
#include <math.h>

using namespace std;
#define MEMORY_SIZE 100
extern char main_memory[MEMORY_SIZE];


typedef struct page_descriptor 
{
    unsigned int V;     // valid
    unsigned int D;     // dirty
    unsigned int P;     // permission
    unsigned int frame; //the number of a frame if in case it is page-mapped
} page_descriptor;

class sim_mem
{
    int swapfile_fd; //swap file fd
    int program_fd;  //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    page_descriptor *page_table; //pointer to page table
    int page_fifo_count;
    int frame_fifo_count;

public:
    sim_mem(char exe_file_name[], char swap_file_name[], 
                    int text_size, int data_size, int bss_size, 
                    int heap_stack_size, int num_of_pages, int page_size);
    ~sim_mem();
    char load(int address);
    void store(int address, char value);
    void print_memory();
    void print_swap();
    void print_page_table();

    //Assistant Functions
    void error(const char *msg);
    int  physical_add(int address);
    void swap_pages(int address);
    void page_fifo_counter();
    void bring_from_swap(int address);
    int find_free_frame();
    void clear_swap(int address);
    void write_main(int address, int frame_fd,int flag);
    void init_swap(const int swap_fd);
    void init_main_memory(const int main_fd);
    int frame_fifo_counter();

};

#endif // __SIM_MEM