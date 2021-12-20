#ifndef __SHAREDINFO_H__
#define __SHAREDINFO_H__

#ifndef __KERNEL__
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
#endif

#define PATH_MAX_LENGTH 256

enum ConnectionType { CONNECT, BIND };
enum Protocols { PROTO_TCP, PROTO_UDP, PROTO_TCP6, PROTO_UDP6, PROTO_NETLINK };
enum Destiny { REJECT, ACCEPT };

typedef uint32_t id_t;
typedef uint32_t ipv4_addr_t;
typedef uint16_t ipv4_port_t;
typedef struct { unsigned char u8[16]; } ipv6_addr_t;
typedef uint16_t ipv6_port_t;


struct Proto_Specific_IP {
	ipv4_addr_t Destination;
	ipv4_port_t DestinationPort;
};

struct Proto_Specific_Netlink {
	uint32_t PID;
	uint32_t Group;
};

struct Proto_Specific_IPv6 {
	ipv6_addr_t Destination;
	ipv6_port_t DestinationPort;
};

#define GREATER_PAYLOAD_1			(sizeof(struct Proto_Specific_IP))
#define GREATER_PAYLOAD_2			((GREATER_PAYLOAD_1 > (sizeof(struct Proto_Specific_Netlink))) ? GREATER_PAYLOAD_1 : sizeof(struct Proto_Specific_Netlink))
#define GREATER_PAYLOAD_3			((GREATER_PAYLOAD_2 > (sizeof(struct Proto_Specific_IPv6))) ? GREATER_PAYLOAD_2 : sizeof(struct Proto_Specific_IPv6))
#define MAX_SPECIFIC_PAYLOAD		(GREATER_PAYLOAD_3)

struct Proto_Specific {
	unsigned char Data[MAX_SPECIFIC_PAYLOAD];
};

struct Kernel_Notification {
	id_t ID;
	enum ConnectionType ConnectionType;
	enum Protocols Protocol;
	pid_t PID;
	uid_t UID;
	gid_t GID;
	char Filename[PATH_MAX_LENGTH];
	struct Proto_Specific Payload;
};


typedef struct Proto_Specific_IP Proto_Specific_TCP4;
typedef struct Proto_Specific_IP Proto_Specific_UDP4;
typedef struct Proto_Specific_IPv6 Proto_Specific_TCP6;
typedef struct Proto_Specific_IPv6 Proto_Specific_UDP6;
typedef struct Proto_Specific_Netlink Proto_Specific_Netlink;

struct Userland_Response {
	id_t ID;
	enum Destiny Destiny;
};

#endif
