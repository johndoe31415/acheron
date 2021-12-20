#include "Rule.hpp"
#include "Rule_IP.hpp"
#include "Trace.hpp"

Rule_IP::Rule_IP(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort) : Rule(Policy, RelTimeRange, PID, UID, GID, Application) {
	this->Destination = Type_Range_IPAddress(Destination);
	this->DestinationPort = Type_Range_Int(DestinationPort);
}

bool Rule_IP::MatchesRequest(const Request &Request) const {
	if (!dynamic_cast<const Request_IP*>(&Request)) return false;

	const Request_IP &ReqIP = dynamic_cast<const Request_IP&>(Request);
	bool Result = true;

	if (!Rule::MatchesRequest(Request)) Result = false;
		else if (!Destination.Is_Satisfied(ReqIP.Get_Destination())) Result = false;
		else if (!DestinationPort.Is_Satisfied(ReqIP.Get_DestinationPort())) Result = false;

	Trace_Rule_MatchRequest("Rule_IP", Result);

	return Result;
}

Rule_IP::~Rule_IP() {
}

