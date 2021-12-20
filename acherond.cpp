#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>

#include "kernel/sharedinfo.h"

#include "ComplexParser.hpp"
#include "GenericException.hpp"
#include "StringParser.hpp"

#include "Types.hpp"

#include "Request.hpp"
#include "Request_TCP.hpp"
#include "Request_UDP.hpp"
#include "Request_TCP6.hpp"
#include "Request_UDP6.hpp"
#include "Request_Netlink.hpp"
#include "Rule.hpp"
#include "Rule_IP.hpp"
#include "Rule_IP6.hpp"
#include "Rule_TCP.hpp"
#include "Rule_UDP.hpp"
#include "Rule_TCP6.hpp"
#include "Rule_UDP6.hpp"
#include "Rule_Netlink.hpp"
#include "RuleSet.hpp"
#include "SystemResolver.hpp"
#include "Config.hpp"
#include "Output.hpp"
#include "Logger.hpp"

static bool Quit = false;

void DisplayRequest(const struct Kernel_Notification &KNot) {
	LOGGER << std::hex;
	LOGGER << "ID: " << KNot.ID;
	LOGGER << std::dec;

	if (KNot.ConnectionType == CONNECT) {
		LOGGER << ", Connect";
	} else if (KNot.ConnectionType == BIND) {
		LOGGER << ", Bind";
	} else {
		LOGGER << ", Unknown connection type -> REJECT" << std::endl;
		return;
	}

	LOGGER << ", PID " << KNot.PID;
	LOGGER << ", UID " << KNot.UID;
	LOGGER << ", GID " << KNot.GID;
	LOGGER << ", Application " << KNot.Filename;

	if (KNot.Protocol == PROTO_TCP) {
		Proto_Specific_TCP4 *Payload = (Proto_Specific_TCP4*)&KNot.Payload;
		LOGGER << ", TCP [";
		LOGGER << "IP ";
		LOGGER << ((Payload->Destination & 0x000000ff) >> 0) << ".";
		LOGGER << ((Payload->Destination & 0x0000ff00) >> 8) << ".";
		LOGGER << ((Payload->Destination & 0x00ff0000) >> 16) << ".";
		LOGGER << ((Payload->Destination & 0xff000000) >> 24);

		LOGGER << ", Port " << Payload->DestinationPort;
		LOGGER << "]";
	} else if (KNot.Protocol == PROTO_UDP) {
		LOGGER << ", UDP [";
		Proto_Specific_UDP4 *Payload = (Proto_Specific_UDP4*)&KNot.Payload;
		LOGGER << "IP ";
		LOGGER << ((Payload->Destination & 0x000000ff) >> 0) << ".";
		LOGGER << ((Payload->Destination & 0x0000ff00) >> 8) << ".";
		LOGGER << ((Payload->Destination & 0x00ff0000) >> 16) << ".";
		LOGGER << ((Payload->Destination & 0xff000000) >> 24);

		LOGGER << ", Port " << Payload->DestinationPort;
		LOGGER << "]";
	} else if (KNot.Protocol == PROTO_TCP6) {
		LOGGER << ", IPv6 TCP [";
		Proto_Specific_TCP6 *Payload = (Proto_Specific_TCP6*)&KNot.Payload;
		LOGGER << "IP ";
		LOGGER << Type_IP6Address(Payload->Destination);
		LOGGER << ", Port " << Payload->DestinationPort;
		LOGGER << "]";
	} else if (KNot.Protocol == PROTO_UDP6) {
		LOGGER << ", IPv6 UDP [";
		Proto_Specific_UDP6 *Payload = (Proto_Specific_UDP6*)&KNot.Payload;
		LOGGER << "IP ";
		LOGGER << Type_IP6Address(Payload->Destination);
		LOGGER << ", Port " << Payload->DestinationPort;
		LOGGER << "]";
	} else if (KNot.Protocol == PROTO_NETLINK) {
		LOGGER << ", Netlink [";
		Proto_Specific_Netlink *Payload = (Proto_Specific_Netlink*)&KNot.Payload;
		LOGGER << "PID " << Payload->PID;
		LOGGER << ", Group " << Payload->Group;
		LOGGER << "]";
	} else {
		LOGGER << ", Unknown protocol " << KNot.Protocol;
	}

	LOGGER << std::flush;
}

Request* GenerateRequest(const struct Kernel_Notification &KNot) {
	if (KNot.Protocol == PROTO_TCP) {
		Proto_Specific_TCP4 *Payload = (Proto_Specific_TCP4*)&KNot.Payload;
		return new Request_TCP(KNot.PID, KNot.UID, KNot.GID, std::string(KNot.Filename), Payload->Destination, Payload->DestinationPort);
	} else if (KNot.Protocol == PROTO_UDP) {
		Proto_Specific_UDP4 *Payload = (Proto_Specific_UDP4*)&KNot.Payload;
		return new Request_UDP(KNot.PID, KNot.UID, KNot.GID, std::string(KNot.Filename), Payload->Destination, Payload->DestinationPort);
	} else if (KNot.Protocol == PROTO_TCP6) {
		Proto_Specific_TCP6 *Payload = (Proto_Specific_TCP6*)&KNot.Payload;
		return new Request_TCP6(KNot.PID, KNot.UID, KNot.GID, std::string(KNot.Filename), Payload->Destination, Payload->DestinationPort);
	} else if (KNot.Protocol == PROTO_UDP6) {
		Proto_Specific_UDP6 *Payload = (Proto_Specific_UDP6*)&KNot.Payload;
		return new Request_UDP6(KNot.PID, KNot.UID, KNot.GID, std::string(KNot.Filename), Payload->Destination, Payload->DestinationPort);
	} else if (KNot.Protocol == PROTO_NETLINK) {
		Proto_Specific_Netlink *Payload = (Proto_Specific_Netlink*)&KNot.Payload;
		return new Request_Netlink(KNot.PID, KNot.UID, KNot.GID, std::string(KNot.Filename), Payload->PID, Payload->Group);
	} else {
		GENERICEXCEPTION("Cannot convert kernel notification to internal data structure.");
	}
	return NULL;
}

void DestroyRequest(Request *Request) {
	delete Request;
}

int GetFWDescriptor(const char *DeviceFilename) {
	int fd;
	fd = open(DeviceFilename, O_RDWR);
	if (fd == -1) {
		GENERICSTRMEXCEPTION("Cannot open firewall device " << DeviceFilename << ": " << strerror(errno) << std::endl);
	}
	return fd;
}

void Firewall(int fd, const RuleSet &ConnectRuleset, const RuleSet &BindRuleset, bool AsDaemon) {
	if (AsDaemon) {
		if (fork() != 0) exit(EXIT_SUCCESS);
		if (fork() != 0) exit(EXIT_SUCCESS);
	}
	while (!Quit) {
		struct Kernel_Notification KNot;
		int Rd;
		memset(&KNot, 0, sizeof(KNot));
		
		/* Wait until there is data to read */
		{
			//LOGGER_DEBUG << "POLL" << std::endl;
			struct pollfd PollFD;
			PollFD.fd = fd;
			PollFD.events = POLLIN;
			PollFD.revents = 0;
			int Result = poll(&PollFD, 1, -1);
			if (Result < 0) {
				perror("poll");
				continue;
			} else if (Result == 0) {
				perror("No FD at poll()");
				continue;
			}
		}

		Rd = read(fd, &KNot, sizeof(struct Kernel_Notification));
		if (Rd != sizeof(struct Kernel_Notification)) {
			continue;
		}
	
		DisplayRequest(KNot);
		
		struct Userland_Response Rsp;
		Rsp.ID = KNot.ID;
		Request *CurrentRequest = GenerateRequest(KNot);
		if (CurrentRequest) {
			if (KNot.ConnectionType == CONNECT) {
				Rsp.Destiny = ConnectRuleset.CheckRequest(*CurrentRequest);
			} else if (KNot.ConnectionType == BIND) {
				Rsp.Destiny = BindRuleset.CheckRequest(*CurrentRequest);
			} else {
				throw GENERICEXCEPTION("Cannot handle unknown connection type from kernel.");
			}
			DestroyRequest(CurrentRequest);
		} else {
			LOGGER << "(unhandled)";
			Rsp.Destiny = REJECT;
		}

		if (Rsp.Destiny == ACCEPT) LOGGER << " => ACCEPT" << std::endl << std::flush;
			else if (Rsp.Destiny == REJECT) LOGGER << " => REJECT" << std::endl << std::flush;
			else LOGGER << " => INVALID" << std::endl << std::flush;

		int Wr;
		Wr = write(fd, &Rsp, sizeof(struct Userland_Response));
		if (Wr != sizeof(struct Userland_Response)){
			perror("write struct Userland_Response");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);
}

enum Destiny StringToPolicy(const std::string &String) {
	if (String == "ACCEPT") return ACCEPT;
	if (String == "REJECT") return REJECT;
	GENERICSTRMEXCEPTION("Unknown policy " << String);
}

void ParseConfig(const std::string &Filename, RuleSet &ConnectRuleset, RuleSet &BindRuleset, Config &Configuration) {
	bool HaveDefaultPolicy = false;
	ComplexParser Cmpl = ComplexParser(Filename);
	while (Cmpl.ParseNext()) {
		if (Cmpl.GetSectionName() == "[default]") {
			HaveDefaultPolicy = true;
			Cmpl.CheckPrerequisite("connect");
			Cmpl.CheckPrerequisite("bind");
			ConnectRuleset.SetDefaultPolicy(StringToPolicy(Cmpl.GetProperty("connect")(0)));
			BindRuleset.SetDefaultPolicy(StringToPolicy(Cmpl.GetProperty("bind")(0)));
		} else if ((Cmpl.GetSectionName() == "[connect]") || (Cmpl.GetSectionName() == "[bind]")) {
			Cmpl.CheckPrerequisite("policy");
			enum Destiny Policy = StringToPolicy(Cmpl.GetProperty("policy")(0));
			
			Rule *NewRule;
			if (Cmpl.HasProperty("protocol")) {
				if (Cmpl.GetProperty("protocol")(0) == "ip") {
					const char *Recognized[] = { "policy", "protocol", "reltime", "pid", "uid", "gid", "application", "dest", "destport", NULL };
					Cmpl.CheckUnrecognizedKeywords(Recognized);
					NewRule = new Rule_IP(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "tcp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "tcp") {
					const char *Recognized[] = { "policy", "protocol", "reltime", "pid", "uid", "gid", "application", "dest", "destport", NULL };
					Cmpl.CheckUnrecognizedKeywords(Recognized);
					NewRule = new Rule_TCP(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "tcp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "udp") {
					const char *Recognized[] = { "policy", "protocol", "reltime", "pid", "uid", "gid", "application", "dest", "destport", NULL };
					Cmpl.CheckUnrecognizedKeywords(Recognized);
					NewRule = new Rule_UDP(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "udp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "ip6") {
					NewRule = new Rule_IP6(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "tcp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "tcp6") {
					NewRule = new Rule_TCP6(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "tcp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "udp6") {
					NewRule = new Rule_UDP6(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("dest"),
										SystemResolver::ResolvePortStr(Cmpl.ForceGetPropertyString("destport"), "udp")
							);
				} else if (Cmpl.GetProperty("protocol")(0) == "netlink") {
					const char *Recognized[] = { "policy", "protocol", "reltime", "pid", "uid", "gid", "application", "nlpid", "nlgroup", NULL };
					Cmpl.CheckUnrecognizedKeywords(Recognized);
					NewRule = new Rule_Netlink(Policy,
										Cmpl.ForceGetPropertyString("reltime"),
										Cmpl.ForceGetPropertyString("pid"),
										SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
										SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
										Cmpl.ForceGetPropertyString("application"),
										Cmpl.ForceGetPropertyString("nlpid"),
										Cmpl.ForceGetPropertyString("nlgroup")
							);
				} else {
					GENERICSTRMEXCEPTION("Unknown protocol " << Cmpl.GetProperty("protocol")(0));
				}

			} else {
				const char *Recognized[] = { "policy", "reltime", "pid", "uid", "gid", "application", NULL };
				Cmpl.CheckUnrecognizedKeywords(Recognized);
				NewRule = new Rule(Policy,
									Cmpl.ForceGetPropertyString("reltime"),
									Cmpl.ForceGetPropertyString("pid"),
									SystemResolver::ResolveUIDStr(Cmpl.ForceGetPropertyString("uid")),
									SystemResolver::ResolveGIDStr(Cmpl.ForceGetPropertyString("gid")),
									Cmpl.ForceGetPropertyString("application")
						);
			}
			LOGGER << (*NewRule) << std::endl << std::flush;

			if (Cmpl.GetSectionName() == "[connect]") ConnectRuleset.Append(NewRule);
				else BindRuleset.Append(NewRule);
		} else if (Cmpl.GetSectionName() == "[options]") {
			const char *Recognized[] = { "debuglog", "log", "criticallog", "dropuid", "dropgid", "daemonize", NULL };
			Cmpl.CheckUnrecognizedKeywords(Recognized);
			Cmpl.CheckPrerequisite("dropuid");
			Cmpl.CheckPrerequisite("dropgid");
			Cmpl.CheckPrerequisite("daemonize");
			Configuration = Config(SystemResolver::ResolveUID(Cmpl.GetProperty("dropuid")(0)),
						SystemResolver::ResolveGID(Cmpl.GetProperty("dropgid")(0)),
						Cmpl.GetProperty("daemonize").GetBoolValue(0));

			Logger::Get().Init(Cmpl.ForceGetPropertyString("criticallog"),
						Cmpl.ForceGetPropertyString("log"),
						Cmpl.ForceGetPropertyString("debuglog"));
		} else {
			GENERICSTRMEXCEPTION("Unknown section '" << Cmpl.GetSectionName() << "' at line " << Cmpl.GetLineNumber() << ", must be one of '[default]', '[connect]' or '[bind]'.");
		}
	}
	if (!HaveDefaultPolicy) GENERICEXCEPTION("Do not have a default policy, please specify one in a '[default]' section.");
}

#define FW_DEVICE				"kernel/firewall"
#define FW_SYSCTRL				"/sys/module/acheron/parameters/Firewall_Active"
#define FW_SYSCTRL_AUTOSHTDN	"/sys/module/acheron/parameters/Firewall_Autoshutdown"

void Signalhandler(int Signal) {
	Quit = true;
}

void Install_Sighandler() {
	struct sigaction Action;
	Action.sa_handler = Signalhandler;
	sigemptyset(&Action.sa_mask);
	Action.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &Action, NULL) != 0) {
		LOGGER_CRIT << "Cannot install SIGINT handler." << std::endl << std::flush;
	}
	if (sigaction(SIGTERM, &Action, NULL) != 0) {
		LOGGER_CRIT << "Cannot install SIGTERM handler." << std::endl << std::flush;
	}
}

int Activate_FW() {
	FILE *f;
	f = fopen(FW_SYSCTRL_AUTOSHTDN, "w");
	if (!f) {
		LOGGER_CRIT << "Error: Could not open " << FW_SYSCTRL_AUTOSHTDN << " for writing: " << strerror(errno) << std::endl << std::flush;
		return 0;
	}
	if (fwrite("Y\n", 1, 2, f) != 2) {
		LOGGER_CRIT << "Error: Could not write to " << FW_SYSCTRL_AUTOSHTDN << " for writing: " << strerror(errno) << std::endl << std::flush;
		return 0;
	}
	fclose(f);

	f = fopen(FW_SYSCTRL, "w");
	if (!f) {
		LOGGER_CRIT << "Error: Could not open " << FW_SYSCTRL << " for writing: " << strerror(errno) << std::endl << std::flush;
		return 0;
	}
	if (fwrite("Y\n", 1, 2, f) != 2) {
		LOGGER_CRIT << "Error: Could not write to " << FW_SYSCTRL << " for writing: " << strerror(errno) << std::endl << std::flush;
		return 0;
	}
	fclose(f);
	return 1;
}

int WrapMain(int argc, char **argv) {
	bool Emergency = false;
	
	if ((argc == 2) && !strcmp(argv[1], "EMERGENCY")) {
		Emergency = true;
		LOGGER_CRIT << "RUNNING IN EMERGENCY ACCEPT MODE! THIS IS DANGEOUS!" << std::endl << std::flush;
	}

	RuleSet ConnectRuleset(REJECT);
	RuleSet BindRuleset(REJECT);
	Config Configuration;
	if (!Emergency) {
		ParseConfig("acherond.conf", ConnectRuleset, BindRuleset, Configuration);
		Configuration.AssertInitialized();
		Install_Sighandler();
		if (!Activate_FW()) {
			LOGGER_CRIT << "Cannot activate firewall." << std::endl << std::flush;
			exit(EXIT_FAILURE);
		}
	}
	int FWDevDescriptor = GetFWDescriptor(FW_DEVICE);
	if (!Emergency) {
		Configuration.DropPrivileges();
	} else {
		ConnectRuleset.Flush(ACCEPT);
		BindRuleset.Flush(ACCEPT);
	}
	Firewall(FWDevDescriptor, ConnectRuleset, BindRuleset, Configuration.GetDaemonize());
	close(FWDevDescriptor);
	LOGGER_CRIT << "Gracefully shut down." << std::endl << std::flush;
	return 0;
}

int main(int argc, char **argv) {
	int Result = 1;
	try {
		Result = WrapMain(argc, argv);
	} catch (TypeException &E) {
		E.Terminate();
	} catch (GenericException &E) {
		E.Terminate();
	}

	return Result;
}

