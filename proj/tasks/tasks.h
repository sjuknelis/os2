#ifndef TASKS_H
#define TASKS_H

void init_task(void* func);
void run_tasks();
void set_blocked(char blocked);
void kspawn(char* buf);
void kexit(char* buf);

#endif