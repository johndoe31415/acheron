#ifndef __REQUEST_IP_HPP__
#define __REQUEST_IP_HPP__

#include "Request.hpp"

class Request_IP : public Request {
	private:
		Type_IPAddress Destination;
		Type_Int DestinationPort;
	public:
		Request_IP(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application, unsigned int Destination, unsigned int DestinationPort) : Request(PID, UID, GID, Application) {
			this->Destination = ntohl(Destination);
			this->DestinationPort = DestinationPort;
		}
		// {{{ inline const Type_IPAddress& Get_Destination() const;
		inline const Type_IPAddress& Get_Destination() const {
			return Destination;
		}
		// }}}
		// {{{ inline const Type_Int& Get_DestinationPort() const;
		inline const Type_Int& Get_DestinationPort() const {
			return DestinationPort;
		}
		// }}}
		virtual ~Request_IP() {
		}
};
#endif

