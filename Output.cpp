#include <ostream>
#include <set>
#include <iomanip>

#include "Rule_TCP.hpp"
#include "Rule_UDP.hpp"
#include "Rule_TCP6.hpp"
#include "Rule_UDP6.hpp"
#include "Rule_Netlink.hpp"
#include "Types.hpp"
#include "Output.hpp"

std::ostream& operator<<(std::ostream &Strm, enum Destiny Obj) {
	if (Obj == ACCEPT) Strm << "ACCEPT";
		else if (Obj == REJECT) Strm << "REJECT";
		else Strm << "INVALID";
	return Strm;
}

std::ostream& operator<<(std::ostream &Strm, const Type_Range_Int &Obj) {
	unsigned int Min, Max;
	bool MinValid, MaxValid;
	bool Negated;
	Obj.Get_Value(Min, MinValid, Max, MaxValid, Negated);
	if (Negated) Strm << "!";
	if (MinValid && MaxValid) {
		if (Min == Max) Strm << Min;
			else Strm << Min << "-" << Max;
	} else if (MinValid) {
		Strm << Min << "-";
	} else if (MaxValid) {
		Strm << "-" << Max;
	} else {
		Strm << "N/A";
	}
	return Strm;
}

std::ostream& operator<<(std::ostream &Strm, const Type_String &Obj) {
	Strm << Obj.Get_Value();
	return Strm;
}

std::ostream& operator<<(std::ostream &Strm, const Type_Range_String &Obj) {
	if (!Obj.Is_Valid()) {
		Strm << "()";
		return Strm;
	}

	Strm << "(";
	const std::set<Type_String> &Values = Obj.Get_Values();
	std::set<Type_String>::const_iterator i;
	
	for (i = Values.begin(); i != Values.end(); i++) {
		if (i != Values.begin()) {
			Strm << " || ";
		}
		Strm << (*i);
	}
	Strm << ")";
	return Strm;
}

#define STRMIPv4(IP)		((IP & 0xff000000) >> 24) << "." << ((IP & 0x00ff0000) >> 16) << "." << ((IP & 0x0000ff00) >> 8) << "." << ((IP & 0x000000ff) >> 0)

std::ostream& operator<<(std::ostream &Strm, const Type_Range_IPAddress &Obj) {
	unsigned int Min, Max;
	bool MinValid, MaxValid;
	bool Negated;
	Obj.Get_Value().Get_Value(Min, MinValid, Max, MaxValid, Negated);
	if (Negated) Strm << "!";
	if (MinValid && MaxValid) {
		if (Min == Max) Strm << STRMIPv4(Min);
			else Strm << STRMIPv4(Min) << "-" << STRMIPv4(Max);
	} else if (MinValid) {
		Strm << STRMIPv4(Min) << "-";
	} else if (MaxValid) {
		Strm << "-" << STRMIPv4(Max);
	} else {
		Strm << "N/A";
	}
	return Strm;
}

#define BIGINT_BLOCKSIZE 2

std::ostream& operator<<(std::ostream &Strm, const Type_BigInt &bigInt) {
	Strm.setf(std::ios::hex, std::ios::basefield);
	for(int i = 0; i < bigInt.size; ++i) {
		Strm  << std::setfill('0') << std::setw(2) << int(bigInt.chunks[i]);
		if(!((i+1) % BIGINT_BLOCKSIZE) && i < bigInt.size-1)
			Strm << ":";
	}
	Strm.setf(std::ios::dec, std::ios::basefield);

	return Strm;
}

std::ostream& operator<<(std::ostream &Strm, const Type_IP6Address &Obj) {
	Strm << Obj.Get_Value();
	return Strm;
}

std::ostream& operator<<(std::ostream &Strm, const Type_Range_IP6Address &Obj) {
	Type_IP6Address Min, Max;
	bool MinValid, MaxValid;
	bool Negated;
	Obj.Get_Value(Min, MinValid, Max, MaxValid, Negated);
	if (Negated) Strm << "!";
	if (MinValid && MaxValid) {
		if (Min == Max) Strm << Min;
			else Strm << Min << "-" << Max;
	} else if (MinValid) {
		Strm << Min << "-";
	} else if (MaxValid) {
		Strm << "-" << Max;
	} else {
		Strm << "N/A";
	}
	return Strm;
}


#define STRM_TIME(x)		std::setfill('0') << std::setw(2) << ((x)/60) << ":" << std::setw(2) << ((x)%60)

std::ostream& operator<<(std::ostream &Strm, const Type_Range_RelTime &Obj) {
	unsigned int Min, Max;
	bool MinValid, MaxValid;
	bool Negated;
	Obj.Get_Value().Get_Value(Min, MinValid, Max, MaxValid, Negated);
	if (Negated) Strm << "!";
	if (MinValid && MaxValid) {
		if (Min == Max) Strm << STRM_TIME(Min);
			else Strm << STRM_TIME(Min) << "-" << STRM_TIME(Max);
	} else if (MinValid) {
		Strm << STRM_TIME(Min) << "-";
	} else if (MaxValid) {
		Strm << "-" << STRM_TIME(Max);
	} else {
		Strm << "N/A";
	}
	return Strm;
}

#define Dump_SpcMember(Obj,x) 	if (Obj.Get_##x().Is_Valid()) { if (!First) Strm << " && "; Strm << #x << " == " << Obj.Get_##x(); First = false; }
#define Dump_Member(x) 			if (Obj.Get_##x().Is_Valid()) { if (!First) Strm << " && "; Strm << #x << " == " << Obj.Get_##x(); First = false; }

std::ostream& operator<<(std::ostream &Strm, const Rule &Obj) {
	bool First = true;
	if (typeid(Obj) == typeid(Rule_IP)) Strm << "Rule_IP";
		else if (typeid(Obj) == typeid(Rule_TCP)) Strm << "Rule_TCP";
		else if (typeid(Obj) == typeid(Rule_UDP)) Strm << "Rule_UDP";
		else if (typeid(Obj) == typeid(Rule_TCP6)) Strm << "Rule_TCP6";
		else if (typeid(Obj) == typeid(Rule_UDP6)) Strm << "Rule_UDP6";
		else if (typeid(Obj) == typeid(Rule_Netlink)) Strm << "Rule_Netlink";
		else if (typeid(Obj) == typeid(Rule)) Strm << "Rule";
		else Strm << "Other";
	Strm << ": [";
	Dump_Member(RelTime);
	Dump_Member(PID);
	Dump_Member(UID);
	Dump_Member(GID);
	Dump_Member(Application);
	if ((typeid(Obj) == typeid(Rule_IP)) || (typeid(Obj) == typeid(Rule_TCP)) || ((typeid(Obj) == typeid(Rule_UDP)))) {
		const Rule_IP &SObj = dynamic_cast<const Rule_IP&>(Obj);
		Dump_SpcMember(SObj, Destination);
		Dump_SpcMember(SObj, DestinationPort);
	} else if ((typeid(Obj) == typeid(Rule_IP6)) || (typeid(Obj) == typeid(Rule_TCP6)) || ((typeid(Obj) == typeid(Rule_UDP6)))) {
		const Rule_IP6 &SObj = dynamic_cast<const Rule_IP6&>(Obj);
		Dump_SpcMember(SObj, Destination);
		Dump_SpcMember(SObj, DestinationPort);
	} else if (typeid(Obj) == typeid(Rule_Netlink)) {
		const Rule_Netlink &SObj = dynamic_cast<const Rule_Netlink&>(Obj);
		Dump_SpcMember(SObj, NL_PID);
		Dump_SpcMember(SObj, NL_Group);
	}
	Strm << "] -> " << Obj.GetPolicy();
	return Strm;
}
