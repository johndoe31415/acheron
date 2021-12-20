#include <linux/socket.h>
#include <linux/net.h>

uint32_t acheron_GetUniqueID(void);
int acheron_InsertAndWait(uint32_t Unique_ID, const struct Kernel_Notification *Notification);
int acheron_Have_Notification(void);
void acheron_Dispatch_Notification(struct Kernel_Notification *Notification);
int acheron_Receive_Answer(uint32_t Unique_ID, enum Destiny Destiny);
int acheron_Hook_Wait(uint32_t Unique_ID, enum ConnectionType Connection, struct socket *sock, struct sockaddr *address, int addrlen);
