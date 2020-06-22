#include <iostream>
#include <math.h>
#include "sim_mem.h"
using namespace std;

char main_memory[MEMORY_SIZE];

int main()
{
char val, val1, val2;
sim_mem mem_sm((char*)"test", (char*)"swap_file" ,25, 50, 25,25, 25, 5);
for(int i  = 0;i <10 ; i ++){
val = mem_sm.load(1);
val = mem_sm.load (6);
mem_sm.store(90,'8');
val = mem_sm.load (13);
mem_sm.store(1,'X');
val = mem_sm.load (18);
mem_sm.store(51,'g');
val = mem_sm.load (23);
mem_sm.store(0,'0');
val = mem_sm.load (27);
mem_sm.store(85,'7');
val = mem_sm.load (29);
mem_sm.store(75,'5');
val = mem_sm.load (32);
mem_sm.store(42,'8');
mem_sm.store(10,'2');
mem_sm.store(23,'4');
val = mem_sm.load (39);
val = mem_sm.load (41);
mem_sm.store(64,'2');
val = mem_sm.load (48);
val = mem_sm.load (53);
mem_sm.store(98,'i');
val = mem_sm.load (55);
mem_sm.store(80,'6');
mem_sm.store(47,'9');
val = mem_sm.load (64);
mem_sm.store(24,'p');
mem_sm.store(31,'6');
mem_sm.store(65,'o');
val = mem_sm.load (68);
val = mem_sm.load (73);
mem_sm.store(69,'3');
val = mem_sm.load (78);
mem_sm.store(29,'5');;
val = mem_sm.load (85);
mem_sm.store(105,'1');
val = mem_sm.load (89);
mem_sm.store(51,'0');
val = mem_sm.load (92);
val = mem_sm.load (97);
mem_sm.store(95,'9');
val = mem_sm.load (101);
mem_sm.store(17,'3');
val = mem_sm.load (106);
mem_sm.store(100,'0');
mem_sm.store(36,'7');
val = mem_sm.load (110);
mem_sm.store(58,'1');
val = mem_sm.load (117);
mem_sm.store(120,'4');
mem_sm.store(115,'3');
val = mem_sm.load (120);
mem_sm.store(110,'2');
val = mem_sm.load (180);
mem_sm.store(6,'1');
mem_sm.store(70,'4');
}
//printf("Val1 is: %c, Val2 is: %c\n", val1, val2);
mem_sm.print_memory();
mem_sm.print_swap();
mem_sm.print_page_table();
}
