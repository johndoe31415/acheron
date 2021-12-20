#include "Rule_UDP.hpp"
#include "Trace.hpp"

Rule_UDP::Rule_UDP(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort) : Rule_IP(Policy, RelTimeRange, PID, UID, GID, Application, Destination, DestinationPort) {
}

bool Rule_UDP::MatchesRequest(const Request &Request) const {
	bool Result = true;
	if (typeid(Request) != typeid(Request_UDP)) Result = false;
		else if (!Rule_IP::MatchesRequest(Request)) Result = false;
	Trace_Rule_MatchRequest("Rule_UDP", Result);
	return Result;
}

Rule_UDP::~Rule_UDP() {
}
