#ifndef __REQUEST_IP6_HPP__
#define __REQUEST_IP6_HPP__

#include "kernel/sharedinfo.h"
#include "Request.hpp"

class Request_IP6 : public Request {
	private:
		Type_IP6Address Destination;
		Type_Int DestinationPort;
	public:
		Request_IP6(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, const ipv6_addr_t& Destination, unsigned int DestinationPort) : Request(PID, UID, GID, Application) {
			this->Destination = Type_IP6Address(Destination);
			this->DestinationPort = DestinationPort;
		}
		// {{{ inline const Type_IP6Address& Get_Destination() const;
		inline const Type_IP6Address& Get_Destination() const {
			return Destination;
		}
		// }}}
		// {{{ inline const Type_Int& Get_DestinationPort() const;
		inline const Type_Int& Get_DestinationPort() const {
			return DestinationPort;
		}
		// }}}
		virtual ~Request_IP6() {
		}
};
#endif

