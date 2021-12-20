#ifndef __REQUEST_UDP_HPP__
#define __REQUEST_UDP_HPP__

#include "Request_IP.hpp"

class Request_UDP : public Request_IP {
	public:
		Request_UDP(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, unsigned int Destination, unsigned int DestinationPort) : Request_IP(PID, UID, GID, Application, Destination, DestinationPort) {
		}
		~Request_UDP() {
		}
};
#endif

