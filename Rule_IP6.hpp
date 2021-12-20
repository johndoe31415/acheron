#ifndef __RULE_IP6_HPP__
#define __RULE_IP6_HPP__

#include "Rule.hpp"
#include "Types.hpp"
#include "Request_IP6.hpp"

class Rule_IP6 : public Rule {
	private:
		Type_Range_IP6Address Destination;
		Type_Range_Int DestinationPort;
	public:
		Rule_IP6(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application, const std::string &Destination, const std::string &DestinationPort);
		virtual bool MatchesRequest(const Request &Request) const;
		// {{{ inline const Type_Range_IP6Address& Get_Destination() const;
		inline const Type_Range_IP6Address& Get_Destination() const {
			return Destination;
		}
		// }}}
		// {{{ inline const Type_Range_Int& Get_DestinationPort() const;
		inline const Type_Range_Int& Get_DestinationPort() const {
			return DestinationPort;
		}
		// }}}
		virtual ~Rule_IP6();
};
#endif

