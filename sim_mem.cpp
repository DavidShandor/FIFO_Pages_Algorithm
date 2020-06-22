#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "sim_mem.h"
#define MEMORY_SIZE 100

using namespace std;

//Constructor
sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size,
                 int data_size, int bss_size, int heap_stack_size,
                 int num_of_pages, int page_size)
{
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    int frame_fifo_count = 0;
    
    //Open execute file.
    if ((this->program_fd = open(exe_file_name, O_RDONLY, 0)) < 0)
        error("Open Exe File Failed!\n");
    //Open or create Swap file.
    if ((this->swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, S_IRWXG | S_IRWXO | S_IRWXU)) < 0)
        error("Open Swap File Failed!\n");
    // Init page_table.
    this->page_table = (page_descriptor *)malloc(sizeof(page_descriptor) * num_of_pages);
    assert(page_table);

    //Fill SWAP and Main_Memory with 0.
    init_swap(this->swapfile_fd);
    memset(main_memory, '0', MEMORY_SIZE);

    //INIT Page Table.
    for (size_t i = 0; i < num_of_pages; i++)
    {
        page_table[i].V = 0;
        page_table[i].D = 0;
        page_table[i].P = 1;
        if (i < (text_size / page_size))
            page_table[i].P = 0;
    }
}
//Load value into char from main memory
char sim_mem::load(int address)
{
    int page = address / page_size;
    if (page > num_of_pages) //Check if address is legal.
        return '\0';
    int frame_fd;
    if (page_table[page].V == 1)
        return main_memory[physical_add(address)];
    else //if (page_table[page].V == 0)
    {
        if (page_table[page].P == 1) // Write Permission
        {
            if (page_table[page].D == 1) //Page is dirty, can be found on the SWAP
            {
                bring_from_swap(address);
            }
            else //if (page_table[page].D == 0)
            {
                if (address < text_size + data_size) // Text or Data page -> Bring from execute file.
                {
                    frame_fd = find_free_frame();
                    write_main(address, frame_fd, 0);
                }
                else // Bss or heap stack page
                {   // V = 0 & D = 0 & heap stack page
                    if (address > text_size + data_size + bss_size) // Heap Stack page.
                        perror("Try to Load Empty Page.");
                    //Bss page.
                    frame_fd = find_free_frame();
                    page_table[page].V = 1;
                    page_table[page].frame = frame_fd;
                    for (size_t i = 0; i < page_size; i++)
                        main_memory[(frame_fd * page_size) + i] = '0';
                }
            }
        }
        else //if (page_table[page].P == 0) //No write permission-> Text page, bring from execute file.
        {
            frame_fd = find_free_frame();
            write_main(address, frame_fd, 0);
        }
    }
    frame_fifo_counter();
    return main_memory[physical_add(address)];
}
//Store value into main memory
void sim_mem::store(int address, char value)
{
    int page = address / page_size;
    int frame_fd;
    if (page > num_of_pages) //Check if address is legal.
        return;
    if (page_table[page].V == 1)
    {
        main_memory[physical_add(address)] = value;
        return;
    }
    else //if (page_table[page].V == 0)
    {
        if (page_table[page].P == 1) // Write Permission
        {
            if (page_table[page].D == 1) //Page is dirty, can be found on the SWAP
            {
                bring_from_swap(address);
            }
            else //if (page_table[page].D == 0)
            {
                if (address < text_size + data_size)
                {
                    frame_fd = find_free_frame();
                    write_main(address, frame_fd, 0);
                }
                else // Bss or heap stack page
                {
                    page_table[page].V = 1;
                    page_table[page].frame = find_free_frame();
                    for (size_t i = 0; i < page_size; i++)
                        main_memory[(frame_fd * page_size) + i] = '0';
                }
            }
        }
        else
        {
            perror("No Write Permission.");
            return;
        }
    }
    frame_fifo_counter();
    page_table[page].D = 1;
    main_memory[physical_add(address)] = value;
}

//Destructor
sim_mem::~sim_mem()
{
    free(page_table);
    close(program_fd);
    close(swapfile_fd);
}
void sim_mem::error(const char *msg)
{
    perror(msg);
    exit(-1);
}
void sim_mem::print_memory()
{
    int i;
    printf("\n Physical memory\n");
    for (i = 0; i < MEMORY_SIZE; i++)
    {
        printf("[%c]\n", main_memory[i]);
    }
}
void sim_mem::print_swap()
{
    char *str = (char *)malloc(this->page_size * sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while (read(swapfile_fd, str, this->page_size) == this->page_size)
    {
        for (i = 0; i < page_size; i++)
        {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
}
void sim_mem::print_page_table()
{
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
    for (i = 0; i < num_of_pages; i++)
    {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n", page_table[i].V, page_table[i].D, page_table[i].P, page_table[i].frame);
    }
}
//Calculate the physical address of page.
int sim_mem::physical_add(int address)
{
    int page = address / page_size,
        offset = address % page_size;
    int frame = page_table[page].frame;
    int physical_address = (frame * page_size) + offset;

    return physical_address;
}
//Move page from memory to swap: find by FIFO frame to swap. then copy the page to the swap,
// update V and D, and clear memory frame.
void sim_mem::swap_pages(int page)
{
    int loc = page * page_size;
    int main_loc = page_size * frame_fifo_count;
    char buff[page_size];
    lseek(swapfile_fd, loc, SEEK_SET);
    write(swapfile_fd, main_memory + main_loc, page_size);
    memset(main_memory + main_loc, '0', page_size);
    page_table[page].V == 0;
    page_table[page].D == 1;
}
//Bring page from swap to memroy.
void sim_mem::bring_from_swap(int address)
{
    int frame_fd;
    frame_fd = find_free_frame();
    write_main(address, frame_fd, 1);
    // clear_swap(address);
}

//Return frame by FIFO.
int sim_mem::find_free_frame()
{
    int i = 0;
    while (i < num_of_pages)
    {
        if (page_table[i].frame == frame_fifo_count)
            if (page_table[i].V == 1)
            {
                if (page_table[i].D == 1)
                    swap_pages(i); // If frame full- move it to SWAP
                page_table[i].V = 0;
                break;
            }
        i++;
    }
    return frame_fifo_count;
}
//UNUSED FUNCTION. IGNORE. 
// void sim_mem::clear_swap(int address)
// {
//     char zero[page_size];
//     int offset = address % page_size;
//     memset(zero, '0', page_size);
//     lseek(swapfile_fd, address - offset, SEEK_SET);
//     write(swapfile_fd, zero, page_size);
// }

//Write data into main_memory.
void sim_mem::write_main(int address, int frame_fd, int flag)
{
    char buff[page_size];
    int offset = address % page_size;
    int page = address / page_size;
    int fd;

    if (flag == 0)
        fd = program_fd; // read from exec file
    else
        fd = swapfile_fd; // read from swap

    lseek(fd, (address - offset), SEEK_SET);
    read(fd, buff, page_size);

    strncpy(main_memory + (frame_fd * page_size), buff, page_size);

    //update table
    page_table[page].V = 1;
    page_table[page].frame = frame_fd;
}
//Fill SWAP with zero. Used by Constructor. 
void sim_mem::init_swap(const int swap_fd)
{
    int swap_size = page_size * num_of_pages;
    char buff[swap_size];
    memset(buff, '0', swap_size);
    write(swapfile_fd, buff, swap_size);
}
//Counter for FIFO frames and pages. 
int sim_mem::frame_fifo_counter()
{
    this->frame_fifo_count++;
    if (frame_fifo_count > MEMORY_SIZE/page_size)
        frame_fifo_count = 0;
    return frame_fifo_count;
}
