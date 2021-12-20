#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/poll.h>

#include "sharedinfo.h"
#include "notification_buffer.h"
#include "init.h"

struct fd_private {
	wait_queue_head_t WaitQueue;
};
static struct fd_private *InUse = NULL;

void acheron_ReadyForRead(void) {
	if (!InUse) return;	/* Nobody is listening */
	wake_up(&(InUse->WaitQueue));
}

int acheron_CharDev_Open(struct inode *Inode, struct file *File) {
	if (InUse) return -EBUSY;
	File->private_data = kmalloc(sizeof(struct fd_private), GFP_KERNEL);
	InUse = File->private_data;
	if (!File->private_data) return -ENOMEM;
	{
		struct fd_private *Private = (struct fd_private*)File->private_data;
		init_waitqueue_head(&(Private->WaitQueue));
	}
	return 0;
}

int acheron_CharDev_Close(struct inode *Inode, struct file *File) {
	if (!InUse) return -EBUSY;
	kfree(File->private_data);
	if (acheron_Has_Autoshutdown()) {
		acheron_Reset_Autoshutdown();
	}
	InUse = NULL;
	return 0;
}

ssize_t acheron_CharDev_Write(struct file* filename, const char __user *Readbuffer, size_t Readbuffer_Length, loff_t *Offset) {
	if (Readbuffer_Length != sizeof(struct Userland_Response)) return -EINVAL;
	{
		struct Userland_Response Userland_Response;
		if (copy_from_user(&Userland_Response, Readbuffer, sizeof(struct Userland_Response))) return -EFAULT;
		if (!acheron_Receive_Answer(Userland_Response.ID, Userland_Response.Destiny)) return -EDOM;
	}
	return Readbuffer_Length;
}

ssize_t acheron_CharDev_Read(struct file *File, char __user *Writebuffer, size_t Writebuffer_Length, loff_t *Offset) {
	if (Writebuffer_Length != sizeof(struct Kernel_Notification)) return -EINVAL;
	if (!acheron_Have_Notification()) return -EDOM;

	{
		struct Kernel_Notification Kernel_Notification;
		acheron_Dispatch_Notification(&Kernel_Notification);
		if (copy_to_user(Writebuffer, &Kernel_Notification, sizeof(struct Kernel_Notification))) return -EFAULT;
	}
	
	return sizeof(struct Kernel_Notification);
}

unsigned int acheron_CharDev_Poll(struct file *File, struct poll_table_struct *PollTable) {
	struct fd_private *Private = (struct fd_private*)File->private_data;
	if (acheron_Verbosity() >= 3) printk(KERN_INFO "poll_wait waits.\n");
	wait_event_interruptible(Private->WaitQueue, acheron_Have_Notification());
	if (acheron_Verbosity() >= 3) printk(KERN_INFO "poll_wait returned.\n");
	if (!acheron_Have_Notification()) {
		if (acheron_Verbosity() >= 3) printk(KERN_INFO "...but was unsuccessful.\n");
		return -EINTR;
	}
	return POLLIN | POLLRDNORM;
}

