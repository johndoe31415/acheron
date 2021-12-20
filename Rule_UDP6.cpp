#include "Rule_UDP6.hpp"
#include "Trace.hpp"

Rule_UDP6::Rule_UDP6(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort) : Rule_IP6(Policy, RelTimeRange, PID, UID, GID, Application, Destination, DestinationPort) {
}

bool Rule_UDP6::MatchesRequest(const Request &Request) const {
	bool Result = true;
	if (typeid(Request) != typeid(Request_UDP6)) Result = false;
		else if (!Rule_IP6::MatchesRequest(Request)) Result = false;
	Trace_Rule_MatchRequest("Rule_UDP6", Result);
	return Result;
}

Rule_UDP6::~Rule_UDP6() {
}
