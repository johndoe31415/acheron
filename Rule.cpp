#include "Request.hpp"
#include "Rule.hpp"
#include "Trace.hpp"

Rule::Rule() {
}

Rule::Rule(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application) {
	this->Policy = Policy;
	this->RelTime = Type_Range_RelTime(RelTimeRange);
	this->PID = Type_Range_Int(PID);
	this->UID = Type_Range_Int(UID);
	this->GID = Type_Range_Int(GID);
	this->Application = Type_Range_String(Application);
}

bool Rule::MatchesRequest(const Request &Request) const {
	bool Result = true;

	if (!RelTime.Is_Satisfied(Request.Get_RelTime())) Result = false;
		else if (!PID.Is_Satisfied(Request.Get_PID())) Result = false;
		else if (!UID.Is_Satisfied(Request.Get_UID())) Result = false;
		else if (!GID.Is_Satisfied(Request.Get_GID())) Result = false;
		else if (!Application.Is_Satisfied(Request.Get_Application())) Result = false;

	Trace_Rule_MatchRequest("Rule", Result);

	return Result;
}

Rule::~Rule() {
}

