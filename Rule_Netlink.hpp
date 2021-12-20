#ifndef __RULE_NETLINK_HPP__
#define __RULE_NETLINK_HPP__

#include "Rule.hpp"
#include "Types.hpp"
#include "Request_Netlink.hpp"

class Rule_Netlink : public Rule {
	private:
		Type_Range_Int NL_PID;
		Type_Range_Int NL_Group;
	public:
		Rule_Netlink(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &NL_PID, const std::string &NL_Group);
		bool MatchesRequest(const Request &Request) const;
		// {{{ inline const Type_Range_Int Get_NL_PID() const;
		inline const Type_Range_Int Get_NL_PID() const {
			return NL_PID;
		}
		// }}}
		// {{{ inline const Type_Range_Int Get_NL_Group() const;
		inline const Type_Range_Int Get_NL_Group() const {
			return NL_Group;
		}
		// }}}
		~Rule_Netlink();
};
#endif

