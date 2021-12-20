#ifndef __RULESET_HPP__
#define __RULESET_HPP__

#include <vector>

#include "Rule.hpp"
#include "Request.hpp"

class RuleSet {
	private:
		std::vector<Rule*> Rules;
		enum Destiny DefaultPolicy;
	public:
		RuleSet(enum Destiny DefaultPolicy);
		void Append(Rule *NewRule);
		enum Destiny CheckRequest(const Request &Request) const;
		void Flush(enum Destiny DefaultPolicy);
		// {{{ inline void SetDefaultPolicy(enum Destiny DefaultPolicy);
		inline void SetDefaultPolicy(enum Destiny DefaultPolicy) {
			this->DefaultPolicy = DefaultPolicy;
		}
		// }}}
		~RuleSet();
};
#endif

