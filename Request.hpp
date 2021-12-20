#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include "Types.hpp"

class Request {
	private:
		Type_RelTime RelTime;
		Type_Int PID;
		Type_Int UID;
		Type_Int GID;
		Type_String Application;
	public:
		Request(unsigned int PID, unsigned int UID, unsigned int GID, const std::string &Application) {
			RelTime = Type_RelTime();
			this->PID = PID;
			this->UID = UID;
			this->GID = GID;
			this->Application = Application;
		}
		// {{{ inline const Type_RelTime& Get_RelTime() const;
		inline const Type_RelTime& Get_RelTime() const {
			return RelTime;
		}
		// }}}
		// {{{ inline const Type_Int& Get_PID() const;
		inline const Type_Int& Get_PID() const {
			return PID;
		}
		// }}}
		// {{{ inline const Type_Int& Get_UID() const;
		inline const Type_Int& Get_UID() const {
			return UID;
		}
		// }}}
		// {{{ inline const Type_Int& Get_GID() const;
		inline const Type_Int& Get_GID() const {
			return GID;
		}
		// }}}
		// {{{ inline const Type_String& Get_Application() const;
		inline const Type_String& Get_Application() const {
			return Application;
		}
		// }}}
		virtual ~Request() {
		}
};
#endif

