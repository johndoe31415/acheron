#ifndef __TRACE_HPP__
#define __TRACE_HPP__

#include <vector>

#include "kernel/sharedinfo.h"
#include "Request.hpp"
#include "Rule.hpp"

void _Trace_RuleSet_Checkrequest(const Request &Request);
void _Trace_DisplayPolicy(enum Destiny Policy);
void _Trace_RuleSet_Match(int Index, std::vector<Rule*>::const_iterator i, enum Destiny Policy);
void _Trace_RuleSet_NoMatch(int Index, std::vector<Rule*>::const_iterator i, enum Destiny Policy);
void _Trace_RuleSet_DefaultPolicy(enum Destiny DefaultPolicy);
void _Trace_Rule_MatchRequest(const char *Location, bool Result);

#ifdef TRACE
#define Trace_RuleSet_Checkrequest(x)	_Trace_RuleSet_Checkrequest(x)
#define Trace_RuleSet_Match(x, y, z)	_Trace_RuleSet_Match(x, y, z)
#define Trace_RuleSet_NoMatch(x, y, z)	_Trace_RuleSet_NoMatch(x, y, z)
#define Trace_RuleSet_DefaultPolicy(x)	_Trace_RuleSet_DefaultPolicy(x)
#define Trace_Rule_MatchRequest(x, y)	_Trace_Rule_MatchRequest(x, y)
#else
#define Trace_RuleSet_Checkrequest(x)
#define Trace_RuleSet_Match(x, y, z)
#define Trace_RuleSet_NoMatch(x, y, z)
#define Trace_RuleSet_DefaultPolicy(x)
#define Trace_Rule_MatchRequest(x, y)
#endif

#endif
