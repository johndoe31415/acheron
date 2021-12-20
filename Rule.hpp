#ifndef __RULE_HPP__
#define __RULE_HPP__

#include "Types.hpp"
#include "Request.hpp"
#include "kernel/sharedinfo.h"

class Rule {
	private:
		Type_Range_RelTime RelTime;
		Type_Range_Int PID;
		Type_Range_Int UID;
		Type_Range_Int GID;
		Type_Range_String Application;
		enum Destiny Policy;
	public:
		Rule();
		Rule(enum Destiny Policy, const std::string &RelTimeRange, const std::string &PID, const std::string &UID, const std::string &GID, const std::string &Application);
		virtual bool MatchesRequest(const Request &Request) const;
		// {{{ inline enum Destiny GetPolicy() const;
		inline enum Destiny GetPolicy() const {
			return Policy;
		}
		// }}}
		// {{{ inline const Type_Range_RelTime& Get_RelTime() const;
		inline const Type_Range_RelTime& Get_RelTime() const {
			return RelTime;
		}
		// }}}
		// {{{ inline const Type_Range_Int& Get_PID() const;
		inline const Type_Range_Int& Get_PID() const {
			return PID;
		}
		// }}}
		// {{{ inline const Type_Range_Int& Get_UID() const;
		inline const Type_Range_Int& Get_UID() const {
			return UID;
		}
		// }}}
		// {{{ inline const Type_Range_Int& Get_GID() const;
		inline const Type_Range_Int& Get_GID() const {
			return GID;
		}
		// }}}
		// {{{ inline const Type_Range_String& Get_Application() const;
		inline const Type_Range_String& Get_Application() const {
			return Application;
		}
		// }}}
		
		virtual ~Rule();
};
#endif

