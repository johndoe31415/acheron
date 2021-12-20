#include <linux/completion.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/netlink.h>
#include <linux/net.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <net/sock.h>

#include "communication.h"
#include "sharedinfo.h"
#include "statistics.h"
#include "init.h"
#include "exec_resolve.h"

struct NotificationEntry {
	struct Kernel_Notification Kernel_Notification;
	struct completion Completion;
	enum Destiny Destiny;
};

#define NOTIFICATIONBUFFER_SIZE		32
static unsigned int Buffer_Fill = 0;
static unsigned int Buffer_WriteIdx = 0;
static unsigned int Buffer_ReadIdx = 0;
static spinlock_t acheron_Buffer_Lock = SPIN_LOCK_UNLOCKED;
static struct NotificationEntry acheron_NotificationBuffer[NOTIFICATIONBUFFER_SIZE];

static void acheron_Lock(void) {
	spin_lock(&acheron_Buffer_Lock);
}

static void acheron_Unlock(void) {
	spin_unlock(&acheron_Buffer_Lock);
}

id_t acheron_GetUniqueID(void) {
	static spinlock_t IDCounter_Cnt = SPIN_LOCK_UNLOCKED;
	static id_t Current_ID = 0;
	id_t Unique_ID;

	spin_lock(&IDCounter_Cnt);
	Unique_ID = Current_ID;
	Current_ID++;
	spin_unlock(&IDCounter_Cnt);

	return Unique_ID;
}

int acheron_InsertAndWait(id_t Unique_ID, const struct Kernel_Notification *Notification) {
	if (Buffer_Fill == NOTIFICATIONBUFFER_SIZE) {
		printk(KERN_INFO "Rejecting packet, notification buffer is full.\n");
		acheron_Statistic_Increment(STATISTIC_REJECT_BUFFER_FULL_CNT);
		return 0;
	}

	{
		int CurrentIdx;

		/* Critical section ----> */
		acheron_Lock();
		CurrentIdx = Buffer_WriteIdx;
		Buffer_Fill++;
		Buffer_WriteIdx = (Buffer_WriteIdx + 1) % NOTIFICATIONBUFFER_SIZE;
		memcpy(&acheron_NotificationBuffer[CurrentIdx].Kernel_Notification, Notification, sizeof(struct Kernel_Notification));
		acheron_NotificationBuffer[CurrentIdx].Kernel_Notification.ID = Unique_ID;
		init_completion(&acheron_NotificationBuffer[CurrentIdx].Completion);
		acheron_Unlock();
		/* <---- Critical section */

		/* Notifiy any possibly waiting poll that we're waiting for a routing decision */
		acheron_ReadyForRead();

		/* Wait until we have an answer from userspace */
		wait_for_completion(&acheron_NotificationBuffer[CurrentIdx].Completion);

		/* Now "Destiny" is set, as the completion was satisfied */
		return (acheron_NotificationBuffer[CurrentIdx].Destiny == ACCEPT);
	}
};

int acheron_Have_Notification(void) {
	return (Buffer_Fill > 0);
}

void acheron_Dispatch_Notification(struct Kernel_Notification *Notification) {
	if (Buffer_Fill <= 0) {
		printk(KERN_INFO "Trying to dispatch Kernel_Notification, but buffer size is %d.\n", Buffer_Fill);
		return;
	}
	
	/* Critical section ----> */
	acheron_Lock();
	memcpy(Notification, &acheron_NotificationBuffer[Buffer_ReadIdx].Kernel_Notification, sizeof(struct Kernel_Notification));
	acheron_Unlock();
	/* <---- Critical section */
}

int acheron_Receive_Answer(id_t Unique_ID, enum Destiny Destiny) {
	if (acheron_NotificationBuffer[Buffer_ReadIdx].Kernel_Notification.ID != Unique_ID) {
		printk(KERN_INFO "Sent out notification 0x%x, but received order 0x%x.\n", acheron_NotificationBuffer[Buffer_ReadIdx].Kernel_Notification.ID, Unique_ID);
		return 0;
	}
	
	/* Critical section ----> */
	acheron_Lock();
	acheron_NotificationBuffer[Buffer_ReadIdx].Destiny = Destiny;
	complete(&acheron_NotificationBuffer[Buffer_ReadIdx].Completion);
	Buffer_Fill--;
	Buffer_ReadIdx = (Buffer_ReadIdx + 1) % NOTIFICATIONBUFFER_SIZE;
	acheron_Unlock();
	/* <---- Critical section */
	return 1;
}

static int acheron_Unknown_Family(id_t Unique_ID, sa_family_t Family) {
	if (acheron_Verbosity() >= 2) printk(KERN_INFO "0x%x: Unknown connection family 0x%x accepted.\n", Unique_ID, Family);
	acheron_Statistic_Increment(STATISTIC_ACCEPT_NO_IP_CNT);
	return !acheron_Drop_UnknownFamily();		/* No known family (IP, Netlink, ...), default permissive */
}

static int acheron_Unknown_Protocol(id_t Unique_ID, sa_family_t Family, unsigned char Protocol) {
	acheron_Statistic_Increment(STATISTIC_ACCEPT_UNKNOWN_PROTO_CNT);
	if (acheron_Verbosity() >= 2) printk(KERN_INFO "0x%x: Unknown protocol 0x%x of family 0x%x accepted.\n", Unique_ID, Protocol, Family);
	return !acheron_Drop_UnknownProto();		/* No known subprotocol (TCP/IP, UDP/IP, ...), default restrictive  */
}

int acheron_Hook_Wait(id_t Unique_ID, enum ConnectionType Connection, struct socket *sock, struct sockaddr *address, int addrlen) {
	struct task_struct *Current;
	struct Kernel_Notification KNot;
	char path_buffer[PATH_MAX_LENGTH];
	char *path = NULL;
	int length;
	
	Current = get_current();
	length = Exec_Resolve(Current, path_buffer, sizeof(path_buffer), &path);
	
	KNot.ConnectionType = Connection;
	KNot.UID = Current->uid;
	KNot.GID = Current->gid;
	KNot.PID = Current->pid;
	strcpy(KNot.Filename, path);
	
	if (address->sa_family == AF_INET) {
		struct sockaddr_in *sin = (struct sockaddr_in*)address;
		if (sock->sk->sk_protocol == IPPROTO_TCP) {
			Proto_Specific_TCP4 *Payload = (Proto_Specific_TCP4*)&(KNot.Payload);
			KNot.Protocol = PROTO_TCP;
			Payload->Destination = sin->sin_addr.s_addr;
			Payload->DestinationPort = ntohs(sin->sin_port);
		} else if (sock->sk->sk_protocol == IPPROTO_UDP) {
			Proto_Specific_UDP4 *Payload = (Proto_Specific_UDP4*)&(KNot.Payload);
			KNot.Protocol = PROTO_UDP;
			Payload->Destination = sin->sin_addr.s_addr;
			Payload->DestinationPort = ntohs(sin->sin_port);
		} else {
			return acheron_Unknown_Protocol(Unique_ID, address->sa_family, sock->sk->sk_protocol);
		}
	} else if (address->sa_family == AF_INET6) {
		if (sock->sk->sk_protocol == IPPROTO_TCP) {
			int i;
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)address;
			Proto_Specific_TCP6 *Payload = (Proto_Specific_TCP6*)&(KNot.Payload);
			KNot.Protocol = PROTO_TCP6;
			for (i = 0; i < 4; i++) ((uint32_t*)&(Payload->Destination))[i] = sin6->sin6_addr.in6_u.u6_addr32[i];
			Payload->DestinationPort = ntohs(sin6->sin6_port);
		} else if (sock->sk->sk_protocol == IPPROTO_UDP) {
			int i;
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)address;
			Proto_Specific_UDP6 *Payload = (Proto_Specific_UDP6*)&(KNot.Payload);
			KNot.Protocol = PROTO_UDP6;
			for (i = 0; i < 4; i++) ((uint32_t*)&(Payload->Destination))[i] = sin6->sin6_addr.in6_u.u6_addr32[i];
			Payload->DestinationPort = ntohs(sin6->sin6_port);
		} else {
			return acheron_Unknown_Protocol(Unique_ID, address->sa_family, sock->sk->sk_protocol);
		}
	} else if (address->sa_family == AF_NETLINK) {
		struct sockaddr_nl *snl = (struct sockaddr_nl*)address;
		Proto_Specific_Netlink *Payload = (Proto_Specific_Netlink*)&(KNot.Payload);
		KNot.Protocol = PROTO_NETLINK;
		Payload->PID = ntohl(snl->nl_pid);
		Payload->Group = ntohl(snl->nl_groups);
	} else {
		return acheron_Unknown_Family(Unique_ID, address->sa_family);
	}

	return acheron_InsertAndWait(Unique_ID, &KNot);
}

