#include <iostream>

#include "Output.hpp"
#include "Logger.hpp"
#include "Trace.hpp"

void _Trace_RuleSet_Checkrequest(const Request &Request) {
	LOGGER_DEBUG << std::endl << "RuleSet_Checkrequest tries to match request." << std::endl;
}

void _Trace_RuleSet_Match(int Index, std::vector<Rule*>::const_iterator i, enum Destiny Policy) {
	LOGGER_DEBUG << "    Rule #" << (Index + 1) << " (" << (**i) << ") matches" << std::endl;
}

void _Trace_RuleSet_NoMatch(int Index, std::vector<Rule*>::const_iterator i, enum Destiny Policy) {
	LOGGER_DEBUG << "    Rule #" << (Index + 1) << " (" << (**i) << ") doesn't match" << std::endl;
}

void _Trace_RuleSet_DefaultPolicy(enum Destiny DefaultPolicy) {
	LOGGER_DEBUG << "    Ruleset at end, jumping to default policy " << DefaultPolicy << std::endl;
}

void _Trace_Rule_MatchRequest(const char *Location, bool Result) {
	LOGGER_DEBUG << "       MatchRequest " << Location << " does ";
	if (!Result) LOGGER_DEBUG << "NOT ";
	LOGGER_DEBUG << "match." << std::endl;
}

