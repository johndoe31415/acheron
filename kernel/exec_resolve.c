#include <linux/dcache.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/mm.h>

int Exec_Resolve(struct task_struct *Task, char* Path, unsigned int Path_Length, char** Path_Start) {
	struct dentry *dentry = NULL;
	struct vm_area_struct *vma;
	struct mm_struct *mm = get_task_mm(Task);
	unsigned int length = 0;
	char* start;

	start = &Path[Path_Length-1];
	*Path_Start = start;
	*start = 0;

	if(!mm)
		return 0;
	
	down_read(&mm->mmap_sem);
	vma = mm->mmap;
	
	while(vma) {
		if((vma->vm_flags & VM_EXECUTABLE) && vma->vm_file)
			break;
		vma = vma->vm_next;
	}
	
	if(vma) {
		dentry = dget(vma->vm_file->f_dentry);
	}

	up_read(&mm->mmap_sem);
	mmput(mm);

	while(dentry) {
		struct dentry *parent = NULL; 
		
		if(dentry->d_name.len + length + 1 > Path_Length-1) {
			// complete path does not fit in buffer
			return 0;
		}


		start -= dentry->d_name.len;

		strncpy(start, dentry->d_name.name, dentry->d_name.len);
		--start;
		*start = '/';

		length += dentry->d_name.len + 1;
			
		parent = dget_parent(dentry);
		if(IS_ROOT(parent))
			parent = NULL;
	
		dput(dentry);
		dentry = parent;
	}
	
	*Path_Start = start;

	return length;
}
