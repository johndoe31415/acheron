#include "Rule_TCP.hpp"
#include "Trace.hpp"

Rule_TCP::Rule_TCP(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort) : Rule_IP(Policy, RelTimeRange, PID, UID, GID, Application, Destination, DestinationPort) {
}

bool Rule_TCP::MatchesRequest(const Request &Request) const {
	bool Result = true;
	if (typeid(Request) != typeid(Request_TCP)) Result = false;
		else if (!Rule_IP::MatchesRequest(Request)) Result = false;
	Trace_Rule_MatchRequest("Rule_TCP", Result);
	return Result;
}

Rule_TCP::~Rule_TCP() {
}
