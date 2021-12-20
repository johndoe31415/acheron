#ifndef __RULE_UDP_HPP__
#define __RULE_UDP_HPP__

#include "Rule_IP.hpp"
#include "Request_UDP.hpp"

class Rule_UDP : public Rule_IP {
	public:
		Rule_UDP(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort);
		bool MatchesRequest(const Request &Request) const;
		~Rule_UDP();
};
#endif

