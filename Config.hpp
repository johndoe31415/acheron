#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "GenericException.hpp"

class Config {
	private:
		uid_t UID;
		gid_t GID;
		bool Daemonize;
		bool Initialized;
	public:
		Config() {
			Daemonize = false;
			Initialized = false;
		}
		Config(uid_t UID, gid_t GID, bool Daemonize) : UID(UID), GID(GID), Daemonize(Daemonize) {
			if (UID == 0) GENERICEXCEPTION("Cannot drop privileges to UID 0, this is not safe.");
			if (GID == 0) GENERICEXCEPTION("Cannot drop privileges to GID 0, this is not safe.");
			Initialized = true;
		};
		void DropPrivileges() {
			if (!Initialized) GENERICEXCEPTION("Bug in config parser. Uninitialized use of class Config should never happen.");
			if (setgid(GID)) GENERICSTRMEXCEPTION("Cannot drop privileges to GID " << GID << ", " << strerror(errno));
			if (setuid(UID)) GENERICSTRMEXCEPTION("Cannot drop privileges to UID " << UID << ", " << strerror(errno));
		}
		void AssertInitialized() const {
			if (!Initialized) GENERICEXCEPTION("Configuration not completely initialized - is your config file missing the [options] section?");
		}
		bool GetDaemonize() const {
			return Daemonize;
		}
		~Config() {
		}
};
#endif

