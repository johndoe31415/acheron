#ifndef __REQUEST_NETLINK_HPP__
#define __REQUEST_NETLINK_HPP__

#include "Request.hpp"

class Request_Netlink : public Request {
	private:
		Type_Int NL_PID;
		Type_Int NL_Group;
	public:
		Request_Netlink(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, unsigned int NL_PID, unsigned int NL_Group) : Request(PID, UID, GID, Application) {
			this->NL_PID = NL_PID;
			this->NL_Group = NL_Group;
		}
		// {{{ inline const Type_Int& Get_NL_PID() const;
		inline const Type_Int& Get_NL_PID() const {
			return NL_PID;
		}
		// }}}
		// {{{ inline const Type_Int& Get_NL_Group() const;
		inline const Type_Int& Get_NL_Group() const {
			return NL_Group;
		}
		// }}}
		virtual ~Request_Netlink() {
		}
};
#endif

