void acheron_ReadyForRead(void);
int acheron_CharDev_Open(struct inode *Inode, struct file *File);
int acheron_CharDev_Close(struct inode *Inode, struct file *File);
ssize_t acheron_CharDev_Write(struct file* filename, const char __user *Readbuffer, size_t Readbuffer_Length, loff_t *Offset);
ssize_t acheron_CharDev_Read(struct file *File, char __user *Writebuffer, size_t Writebuffer_Length, loff_t *Offset);
unsigned int acheron_CharDev_Poll(struct file *File, struct poll_table_struct *PollTable);
