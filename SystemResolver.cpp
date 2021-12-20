#include <string>
#include <sstream>

#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <netdb.h>

#include "SystemResolver.hpp"
#include "GenericException.hpp"

SystemResolver::SystemResolver() { }
SystemResolver::~SystemResolver() { }

uid_t SystemResolver::ResolveUID(const std::string &UID) {
	struct passwd *PWEnt;
	PWEnt = getpwnam(UID.c_str());
	if (!PWEnt) {
		GENERICSTRMEXCEPTION("Cannot resolve password entry " << UID << " to UID.");
	}
	return PWEnt->pw_uid;
}

std::string SystemResolver::ResolveUIDStr(const std::string &UID) {
	if (UID.find_first_not_of("0123456789-") == std::string::npos) return UID;
	
	std::stringstream Strm;
	Strm << ResolveUID(UID);
	return Strm.str();
}

gid_t SystemResolver::ResolveGID(const std::string &GID) {
	struct group *GREnt;
	GREnt = getgrnam(GID.c_str());
	if (!GREnt) {
		GENERICSTRMEXCEPTION("Cannot resolve group entry " << GID << " to GID.");
	}
	return GREnt->gr_gid;
}

std::string SystemResolver::ResolveGIDStr(const std::string &GID) {
	if (GID.find_first_not_of("0123456789-") == std::string::npos) return GID;

	std::stringstream Strm;
	Strm << ResolveGID(GID);
	return Strm.str();
}

unsigned short SystemResolver::ResolvePort(const std::string &Port, const char *Protocol) {
	struct servent *Service;
	Service = getservbyname(Port.c_str(), Protocol);
	if (!Service) {
		GENERICSTRMEXCEPTION("Cannot resolve service entry " << Port << " for protocol " << Protocol << " to port number.");
	}
	return ntohs(Service->s_port);
}

std::string SystemResolver::ResolvePortStr(const std::string &Port, const char *Protocol) {
	if (Port.find_first_not_of("0123456789-") == std::string::npos) return Port;
	std::stringstream Strm;
	Strm << ResolvePort(Port, Protocol);
	return Strm.str();
}

