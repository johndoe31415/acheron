#ifndef __REQUEST_UDP6_HPP__
#define __REQUEST_UDP6_HPP__

#include "kernel/sharedinfo.h"
#include "Request_IP6.hpp"

class Request_UDP6 : public Request_IP6 {
	public:
		Request_UDP6(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, const ipv6_addr_t& Destination, unsigned int DestinationPort) : Request_IP6(PID, UID, GID, Application, Destination, DestinationPort) {
		}
		~Request_UDP6() {
		}
};
#endif

