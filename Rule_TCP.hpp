#ifndef __RULE_TCP_HPP__
#define __RULE_TCP_HPP__

#include "Rule_IP.hpp"
#include "Request_TCP.hpp"

class Rule_TCP : public Rule_IP {
	public:
		Rule_TCP(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort);
		bool MatchesRequest(const Request &Request) const;
		~Rule_TCP();
};
#endif

