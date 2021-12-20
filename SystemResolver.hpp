#ifndef __SYSTEMRESOLVER_HPP__
#define __SYSTEMRESOLVER_HPP__

class SystemResolver {
	private:
		SystemResolver();
		~SystemResolver();
	public:
		static uid_t ResolveUID(const std::string &UID);
		static std::string ResolveUIDStr(const std::string &UID);
		static gid_t ResolveGID(const std::string &GID);
		static std::string ResolveGIDStr(const std::string &GID);
		static unsigned short ResolvePort(const std::string &Port, const char *Protocol);
		static std::string ResolvePortStr(const std::string &Port, const char *Protocol);
};
#endif

