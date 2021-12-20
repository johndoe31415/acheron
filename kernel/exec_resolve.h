#include <linux/dcache.h>
#include <linux/mount.h>

int Exec_Resolve(struct task_struct *Task, char* Path, unsigned int Path_Length, char** Path_Start);
