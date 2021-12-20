#ifndef __RULE_TCP6_HPP__
#define __RULE_TCP6_HPP__

#include "Types.hpp"
#include "kernel/sharedinfo.h"
#include "Rule_IP6.hpp"
#include "Request_TCP6.hpp"

class Rule_TCP6 : public Rule_IP6 {
	public:
		Rule_TCP6(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort);
		bool MatchesRequest(const Request &Request) const;
		~Rule_TCP6();
};
#endif

