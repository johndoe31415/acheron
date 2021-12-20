#include <linux/module.h>
#include <linux/security.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/un.h>
#include <linux/netlink.h>
#include <linux/net.h>
#include <linux/dcache.h>
#include <linux/mount.h>
#include <net/sock.h>
#include <asm/current.h>

#include "init.h"
#include "exec_resolve.h"
#include "sharedinfo.h"

static const char* acheron_Resolve_SocketProtocol(unsigned char Protocol) {
	switch (Protocol) {
		case IPPROTO_IP: return "IP";
		case IPPROTO_ICMP: return "ICMP";
		case IPPROTO_IGMP: return "IGMP";
		case IPPROTO_IPIP: return "IPIP";
		case IPPROTO_TCP: return "TCP";
		case IPPROTO_EGP: return "EGP";
		case IPPROTO_PUP: return "PUP";
		case IPPROTO_UDP: return "UDP";
		case IPPROTO_IDP: return "IDP";
		case IPPROTO_DCCP: return "DCCP";
		case IPPROTO_RSVP: return "RSVP";
		case IPPROTO_GRE: return "GRE";
		case IPPROTO_IPV6: return "IPV6";
		case IPPROTO_ESP: return "ESP";
		case IPPROTO_AH: return "AH";
#ifdef BEETP
		case IPPROTO_BEETPH: return "BEETPH";
#endif
		case IPPROTO_PIM: return "PIM";
		case IPPROTO_COMP: return "COMP";
		case IPPROTO_SCTP: return "SCTP";
#ifdef UDPLITE
		case IPPROTO_UDPLITE: return "UDPLITE";
#endif
		case IPPROTO_RAW: return "RAW";
	}
	return "Unknown";
}

void acheron_Display_Debug(id_t Unique_ID, const char *Identifier, struct socket *sock, struct sockaddr *address, int addrlen) {
	struct task_struct *Current = get_current();

	char path_buffer[PATH_MAX_LENGTH];
	char *path = NULL;
	Exec_Resolve(Current, path_buffer, sizeof(path_buffer), &path);

	if (acheron_Verbosity() < 2) return;
	if (address->sa_family == AF_INET) {
		struct sockaddr_in *sin = (struct sockaddr_in*)address;
		__u32 Destination = sin->sin_addr.s_addr;
		printk(KERN_INFO "0x%x [%s: %d %d/%d] Tried to IPv4 %s-%s to %d.%d.%d.%d:%d\n", 
				Unique_ID,
				path,
				Current->pid,
				Current->uid,
				Current->gid,
				acheron_Resolve_SocketProtocol(sock->sk->sk_protocol),
				Identifier,
				(Destination & 0x000000ff) >> 0, 
				(Destination & 0x0000ff00) >> 8, 
				(Destination & 0x00ff0000) >> 16, 
				(Destination & 0xff000000) >> 24, 
				ntohs(sin->sin_port)
			);
	} else if (address->sa_family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)address;
		__be32 *Destination = sin6->sin6_addr.in6_u.u6_addr32;
		printk(KERN_INFO "0x%x [%s: %d %d/%d] Tried to IPv6 %s-%s to %x:%x:%x:%x Port %d\n", 
				Unique_ID,
				path,
				Current->pid,
				Current->uid,
				Current->gid,
				acheron_Resolve_SocketProtocol(sock->sk->sk_protocol),
				Identifier,
				ntohl(Destination[0]),
				ntohl(Destination[1]), 
				ntohl(Destination[2]), 
				ntohl(Destination[3]), 
				ntohs(sin6->sin6_port)
			);
	} else if (address->sa_family == AF_LOCAL) {
		struct sockaddr_un *sun = (struct sockaddr_un*)address;
		char MaxPath[128];
		int CopyLen = sizeof(MaxPath) - 1;
		if (addrlen - sizeof(sa_family_t) < CopyLen) CopyLen = addrlen - sizeof(sa_family_t);
		strncpy(MaxPath, sun->sun_path, CopyLen);
		MaxPath[CopyLen] = 0;
		printk(KERN_INFO "0x%x [%s: %d %d/%d] Tried to %s to local socket '%s'\n", 
				Unique_ID,
				path,
				Current->pid,
				Current->uid,
				Current->gid,
				Identifier,
				MaxPath
		);
	} else if (address->sa_family == AF_NETLINK) {
		struct sockaddr_nl *snl = (struct sockaddr_nl*)address;
		printk(KERN_INFO "0x%x [%s: %d %d/%d] Tried to %s to netlink PID %d Groups %08x\n", 
				Unique_ID,
				path,
				Current->pid,
				Current->uid,
				Current->gid,
				Identifier,
				snl->nl_pid,
				snl->nl_groups
		);
	} else {
		printk(KERN_INFO "0x%x: [%s: %d %d/%d] Tried unknown %s, addrlen %d, family = %d\n", 
				Unique_ID,
				path,
				Current->pid,
				Current->uid,
				Current->gid,
				Identifier,
				addrlen, 
				address->sa_family);
	}
}
