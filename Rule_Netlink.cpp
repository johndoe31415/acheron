#include "Rule.hpp"
#include "Rule_Netlink.hpp"
#include "Trace.hpp"

Rule_Netlink::Rule_Netlink(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &NL_PID, const std::string &NL_Group) : Rule(Policy, RelTimeRange, PID, UID, GID, Application) {
	this->NL_PID = Type_Range_Int(NL_PID);
	this->NL_Group = Type_Range_Int(NL_Group);
}

bool Rule_Netlink::MatchesRequest(const Request &Request) const {
	bool Result = true;
	
	if (typeid(Request) != typeid(Request_Netlink)) {
		Result = false;
	} else {
		const Request_Netlink &ReqNL = dynamic_cast<const Request_Netlink&>(Request);
		if (!Rule::MatchesRequest(Request)) Result = false;
			else if (!NL_PID.Is_Satisfied(ReqNL.Get_NL_PID())) Result = false;
			else if (!NL_Group.Is_Satisfied(ReqNL.Get_NL_Group())) Result = false;
	}

	Trace_Rule_MatchRequest("Rule_Netlink", Result);

	return Result;
}

Rule_Netlink::~Rule_Netlink() {
}

