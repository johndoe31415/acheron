#ifndef __REQUEST_TCP_HPP__
#define __REQUEST_TCP_HPP__

#include "Request_IP.hpp"

class Request_TCP : public Request_IP {
	public:
		Request_TCP(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, unsigned int Destination, unsigned int DestinationPort) : Request_IP(PID, UID, GID, Application, Destination, DestinationPort) {
		}
		~Request_TCP() {
		}
};
#endif

