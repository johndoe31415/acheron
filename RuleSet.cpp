#include <iostream>
#include "Trace.hpp"
#include "RuleSet.hpp"

RuleSet::RuleSet(enum Destiny DefaultPolicy) {
	this->DefaultPolicy = DefaultPolicy;
}

void RuleSet::Append(Rule *NewRule) {
	Rules.push_back(NewRule);
}

void RuleSet::Flush(enum Destiny DefaultPolicy) {
	this->DefaultPolicy = DefaultPolicy;

	std::vector<Rule*>::const_iterator i;
	for (i = Rules.begin(); i != Rules.end(); i++) delete *i;

	Rules.clear();
}

enum Destiny RuleSet::CheckRequest(const Request &Request) const {
	std::vector<Rule*>::const_iterator i;
	Trace_RuleSet_Checkrequest(Request);
	for (i = Rules.begin(); i != Rules.end(); i++) {
		if ((*i)->MatchesRequest(Request)) {
			Trace_RuleSet_Match(i - Rules.begin(), i, (*i)->GetPolicy());
			return (*i)->GetPolicy();
		} else {
			Trace_RuleSet_NoMatch(i - Rules.begin(), i, (*i)->GetPolicy());
		}
	}
	Trace_RuleSet_DefaultPolicy(DefaultPolicy);
	return DefaultPolicy;
}

RuleSet::~RuleSet() {
	std::vector<Rule*>::const_iterator i;
	for (i = Rules.begin(); i != Rules.end(); i++) delete *i;
}

