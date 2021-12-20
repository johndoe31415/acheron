#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <set>
#include <iostream>
#include <sstream>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "kernel/sharedinfo.h"

#define TYPEEXCEPTION(stream)				{ std::stringstream Strm; Strm << stream; throw TypeException(Strm.str()); }

class TypeException {
	private:
		std::string Reason;
	public:
		TypeException(const std::string &Reason) {
			this->Reason = Reason;
		};
		void Terminate() {
			std::cerr << "Type exception: " << Reason << std::endl;
			abort();
		}
		const std::string& GetReason() const {
			return Reason;
		}
};

class Type { };
class Type_Range { };

class Type_Int : public Type {
	private:
		unsigned int Value;
	public:
		unsigned int Get_Value() const {
			return Value;
		}
		Type_Int() {
			Value = 0;
		}
		Type_Int(unsigned int i) {
			Value = i;
		}
		Type_Int(const std::string &Integer) {
			Value = std::atoi(Integer.c_str());
		}
};

#define TYPE_RANGE_INT_VALIDCHARS		"01234567890-!"
class Type_Range_Int : public Type_Range {
	private:
		bool MinValid, MaxValid;
		bool Negated;
		unsigned int Min, Max;
	public:
		Type_Range_Int() {
			MinValid = false;
			MaxValid = false;
			Negated = false;
		}
		Type_Range_Int(Type_Int SingleTime) {
			MinValid = true;
			Min = SingleTime.Get_Value();
			MaxValid = true;
			Max = SingleTime.Get_Value();
			Negated = false;
		}
		Type_Range_Int(unsigned int From, bool FromValid, unsigned int To, bool ToValid, bool Negated) {
			MinValid = FromValid;
			Min = From;
			MaxValid = ToValid;
			Max = To;
			this->Negated = Negated;
		}
		Type_Range_Int(const std::string &Integer) {
			if (Integer.length() == 0) {
				MinValid = false;
				MaxValid = false;
				Negated = false;
				return;
			}
			if (Integer.find_first_not_of(TYPE_RANGE_INT_VALIDCHARS) != std::string::npos) {
				TYPEEXCEPTION("Illegal character for integer range at position " << (Integer.find_first_not_of(TYPE_RANGE_INT_VALIDCHARS) + 1));
			}
			std::string Copy;
			Negated = (Integer.at(0) == '!');
			if (Negated) {
				Copy = Integer.substr(1);
			} else {
				Copy = Integer;
			}

			if (Copy.find('-') == std::string::npos) {
				/* No range, a single integer */
				MinValid = true;
				MaxValid = true;
				Min = std::atoi(Copy.c_str());
				Max = std::atoi(Copy.c_str());
			} else {
				std::size_t Pos = Copy.find('-');
				MinValid = true;
				MaxValid = true;
				Min = std::atoi(Copy.substr(0, Pos).c_str());
				Max = std::atoi(Copy.substr(Pos + 1).c_str());
			}
			
		}
		bool Is_Satisfied(const Type_Int &SingleVal) const {
			bool Satisfied = true;
			if (MinValid) Satisfied = Satisfied && (SingleVal.Get_Value() >= Min);
			if (MaxValid) Satisfied = Satisfied && (SingleVal.Get_Value() <= Max);
			if (Negated) Satisfied = !Satisfied;
			return Satisfied;
		}
		bool Is_Valid() const {
			return MinValid || MaxValid || Negated;
		}
		void Get_Value(unsigned int &From, bool &FromValid, unsigned int &To, bool &ToValid, bool &Negated) const {
			From = this->Min;
			FromValid = this->MinValid;
			To = this->Max;
			ToValid = this->MaxValid;
			Negated = this->Negated;
		}
};

class Type_String : public Type {
	public:
		bool operator<(const Type_String &Other) const {
			return Value < Other.Value;
		}
		bool operator==(const Type_String &Other) const {
			return Other.Value == Value;
		}
	private:
		std::string Value;
	public:
		const std::string& Get_Value() const {
			return Value;
		}
		Type_String() {
		}
		Type_String(const std::string &String) {
			Value = String;
		}
};

class Type_Range_String : public Type_Range {
	private:
		std::set<Type_String> Values;
	public:
		Type_Range_String() {
		}
		Type_Range_String(const std::string &Strings) {
			if (Strings.length() == 0) return;
			std::size_t OldPos = 0;
			std::size_t Pos;
			while ((Pos = Strings.find('|', OldPos)) != std::string::npos) {
				Values.insert(Strings.substr(OldPos, Pos - OldPos));
				OldPos = Pos + 1;
			}
			Values.insert(Strings.substr(OldPos, Pos - OldPos));
		}
		bool Is_Satisfied(const Type_String &SingleString) const {
			if (Values.size() == 0) return true;
			return Values.find(SingleString) != Values.end();
		}
		bool Is_Valid() const {
			return (Values.size() > 0);
		}
		const std::set<Type_String>& Get_Values() const {
			return Values;
		}
};

class Type_RelTime : public Type {
	private:
		Type_Int TimeValue;
	public:
		Type_RelTime() {
			time_t Time;
			time(&Time);
			
			struct tm TimeStruct;
			localtime_r(&Time, &TimeStruct);

			TimeValue = (TimeStruct.tm_hour * 60) + TimeStruct.tm_min;
		}
		Type_RelTime(const std::string &TimeString) {
			if ((TimeString.length() != 5)
				|| (TimeString.at(0) < '0') || (TimeString.at(0) > '2')
				|| (TimeString.at(1) < '0') || (TimeString.at(1) > '9')
				|| (TimeString.at(2) != ':')
				|| (TimeString.at(3) < '0') || (TimeString.at(3) > '5')
				|| (TimeString.at(4) < '0') || (TimeString.at(4) > '9')
			) TYPEEXCEPTION("A relative time must be of the form hh:mm and contain exactly 5 characters - '" << TimeString << "' doesn't");
			TimeValue = (std::atoi(TimeString.substr(0, 2).c_str()) * 60) + (std::atoi(TimeString.substr(3, 2).c_str()));
			if ((TimeValue.Get_Value() < 0) || (TimeValue.Get_Value() >= 1440)) {
				TYPEEXCEPTION("A relative time must be of the form hh:mm and be in the range from 00:00 to 23:59 '" << TimeString << "' isn't");
			}
		}
		const Type_Int& Get_Value() const {
			return TimeValue;
		}
};

class Type_Range_RelTime : public Type_Range {
	private:
		Type_Range_Int TimeRange;
	public:
		Type_Range_RelTime() {
		}
		Type_Range_RelTime(const std::string &TimeRngString) {
			if (TimeRngString.length() == 0) return;

			if (TimeRngString.length() == 5) {
				/* "hh:mm" */
				Type_RelTime SingleTime = Type_RelTime(TimeRngString);
				TimeRange = Type_Range_Int(SingleTime.Get_Value());
			} else if (TimeRngString.length() == 6) {
				/* "hh:mm-" || "-hh:mm" */
				if (TimeRngString.at(0) == '-') {
					/* "-hh:mm" */
					Type_RelTime SingleTime = Type_RelTime(TimeRngString.substr(1, 5));
					TimeRange = Type_Range_Int(0, false, SingleTime.Get_Value().Get_Value(), true, false);
				} else if (TimeRngString.at(5) == '-') {
					/* "hh:mm-" */
					Type_RelTime SingleTime = Type_RelTime(TimeRngString.substr(0, 5));
					TimeRange = Type_Range_Int(SingleTime.Get_Value().Get_Value(), true, 0, false, false);
				} else {
					TYPEEXCEPTION("A realative time range must be of the form hh:mm, -hh:mm, hh:mm- or hh:mm-hh:mm and contain exactly 5, 6 or 11 characters - '" << TimeRngString << "' doesn't");
				}
			} else if (TimeRngString.length() == 11) {
				/* "hh:mm-hh:mm" */
				if (TimeRngString.at(5) == '-') {
					Type_RelTime StartTime = Type_RelTime(TimeRngString.substr(0, 5));
					Type_RelTime EndTime = Type_RelTime(TimeRngString.substr(6, 5));
					TimeRange = Type_Range_Int(StartTime.Get_Value().Get_Value(), true, EndTime.Get_Value().Get_Value(), true, false);
				} else {
					TYPEEXCEPTION("A realative time range must be of the form hh:mm, -hh:mm, hh:mm- or hh:mm-hh:mm and contain exactly 5, 6 or 11 characters - '" << TimeRngString << "' doesn't");
				}
			} else {
				TYPEEXCEPTION("A realative time range must be of the form hh:mm, -hh:mm, hh:mm- or hh:mm-hh:mm and contain exactly 5, 6 or 11 characters - '" << TimeRngString << "' doesn't");
			}
		}
		const Type_Range_Int& Get_Value() const {
			return TimeRange;
		}
		bool Is_Valid() const {
			return TimeRange.Is_Valid();
		}
		bool Is_Satisfied(const Type_RelTime &Time) const {
			return TimeRange.Is_Satisfied(Time.Get_Value());
		}
};

#define TYPE_RANGE_IP_VALIDCHARS		"01234567890."
class Type_IPAddress : public Type {
	private:
		Type_Int IPValue;
	public:
		Type_IPAddress() {
			IPValue = (127 * 256 * 256 * 256) + (0 * 256 * 256) + (0 * 256) + 0;
		}
		Type_IPAddress(const std::string &IPString) {
			if (IPString.find_first_not_of(TYPE_RANGE_IP_VALIDCHARS) != std::string::npos) {
				TYPEEXCEPTION("Illegal character for IP address at position " << (IPString.find_first_not_of(TYPE_RANGE_IP_VALIDCHARS) + 1) << " in '" << IPString << "'");
			}
			unsigned int IP = 0;
			std::size_t OldPos = 0;
			std::size_t Pos;
			for (int i = 1; i <= 4; i++) {
				Pos = IPString.find('.', OldPos);
				if ((Pos == std::string::npos) && (i != 4)) TYPEEXCEPTION(IPString << " is not a valid IP address");
				std::string SubIP = IPString.substr(OldPos, Pos - OldPos);
				unsigned int SubIP_Int = std::atoi(SubIP.c_str());
				if (SubIP_Int > 255) TYPEEXCEPTION(IPString << " is not a valid IP address");
				IP = (IP * 256) + SubIP_Int;
				OldPos = Pos + 1;
			}
			IPValue = IP;
		}
		Type_IPAddress(unsigned int IP) {
			IPValue = IP;
		}
		const Type_Int& Get_Value() const {
			return IPValue;
		}
};

class Type_Range_IPAddress : public Type_Range {
	private:
		Type_Range_Int IPRange;
	public:
		Type_Range_IPAddress() {
		}

		Type_Range_IPAddress(const std::string &IPRngString) {
			if (IPRngString.length() == 0) return;

			std::string Copy;
			bool Negated;
			Negated = IPRngString.at(0) == '!';
			if (Negated) {
				Copy = IPRngString.substr(1);
			} else {
				Copy = IPRngString;
			}

			std::size_t Pos = Copy.find('-');
			if (Pos == std::string::npos) {
				/* No range, single IP */
				if (Copy.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos) {
					IPRange = Type_Range_Int(Type_IPAddress(Copy).Get_Value().Get_Value(), true, Type_IPAddress(Copy).Get_Value().Get_Value(), true, Negated);
				} else {
					/* Resolve DNS */
					struct hostent *Hostname = gethostbyname(Copy.c_str());
					if (!Hostname) {
						TYPEEXCEPTION("Failure in DNS resolution for hostname '" << Copy << "'.");
					}
					if (Hostname->h_length != 4) {
						TYPEEXCEPTION("DNS resolution for hostname '" << Copy << "' yielded non-IPv4-address of length " << Hostname->h_length << ".");
					}
					uint32_t IPValue;
					IPValue = ntohl(*((uint32_t*)Hostname->h_addr));
					IPRange = Type_Range_Int(IPValue, true, IPValue, true, Negated);
				}
			} else {
				std::string From, To;
				From = Copy.substr(0, Pos);
				To = Copy.substr(Pos + 1);
				IPRange = Type_Range_Int(Type_IPAddress(From).Get_Value().Get_Value(), true, Type_IPAddress(To).Get_Value().Get_Value(), true, Negated);
			}
		}
		bool Is_Valid() const {
			return IPRange.Is_Valid();
		}
		const Type_Range_Int& Get_Value() const {
			return IPRange;
		}
		bool Is_Satisfied(const Type_IPAddress &IP) const {
			return IPRange.Is_Satisfied(IP.Get_Value());
		}
};


class Type_BigInt {
	private:
		static const int size = 16;
		unsigned char chunks[size];
	public:
		Type_BigInt() {
			for(int i = 0; i < size; ++i)
				this->chunks[i] = 0;
		}
		
		Type_BigInt(unsigned char* chunks, int count) {
			if(size != count)
				TYPEEXCEPTION("Failed to create Type_BigInt due to mismatch in size.");
			for(int i = count-1; i >= 0; --i)
				this->chunks[size-count+i] = chunks[i];
			
			for(int i = 0; i < size - count; ++i)
				this->chunks[i] = 0;
		}
		
		Type_BigInt& operator=(const Type_BigInt& that) {
			if(size != that.size)
				TYPEEXCEPTION("Assignment of Type_BigInt failed due to mismatch in size.");
			for(int i = 0; i < size; ++i)
				chunks[i] = that.chunks[i];
			
			return *this;
		} 

		bool operator<(const Type_BigInt &Other) const {
			if(size != Other.size)
				TYPEEXCEPTION("Failed to compare Type_BigInts due to mismatch in size.");
			
			for(int i = 0; i < size; ++i) {
				if(chunks[i] > Other.chunks[i])
					return false;
				else if(chunks[i] < Other.chunks[i])
					return true;
			}
			// equal
			return false;
		}

		bool operator>(const Type_BigInt &Other) const {
			if(size != Other.size)
				TYPEEXCEPTION("Failed to compare Type_BigInts due to mismatch in size.");
			
			for(int i = 0; i < size; ++i) {
				if(chunks[i] > Other.chunks[i])
					return true;
				else if(chunks[i] < Other.chunks[i])
					return false;
			}
			// equal
			return false;
		}

		bool operator==(const Type_BigInt &Other) const {
			if(size != Other.size)
				return false;

			for(int i = 0; i < size; ++i)
				if(chunks[i] != Other.chunks[i])
					return false;

			return true;
		}

	friend std::ostream& operator<<(std::ostream &Strm, const Type_BigInt &bigInt);
};

class Type_IP6Address {
	private:
		Type_BigInt IP6Value;

	public:
		Type_IP6Address() {
		}

		Type_IP6Address(Type_BigInt bigInt) {
			IP6Value = bigInt;
		}

		Type_IP6Address(const std::string &IP6String) {
			struct in6_addr addr;
			if(!inet_pton(AF_INET6, IP6String.c_str(), &addr))
				TYPEEXCEPTION(IP6String << " is not a valid IP6 address");
		
			IP6Value = Type_BigInt((unsigned char*)&addr.in6_u, 16);
		}

		Type_IP6Address(const ipv6_addr_t& IP6Addr) {
			IP6Value = Type_BigInt((unsigned char*)&IP6Addr.u8, 16);
		}
		
		Type_IP6Address& operator=(const Type_IP6Address& that) {
			IP6Value = that.IP6Value;

			return *this;
		}

		bool operator<(const Type_IP6Address &Other) const {
			return IP6Value < Other.IP6Value;
		}
		
		bool operator<=(const Type_IP6Address &Other) const {
			return !(IP6Value > Other.IP6Value);
		}

		bool operator>(const Type_IP6Address &Other) const {
			return IP6Value > Other.IP6Value;
		}
		
		bool operator>=(const Type_IP6Address &Other) const {
			return !(IP6Value < Other.IP6Value);
		}
		
		bool operator==(const Type_IP6Address &Other) const {
			return IP6Value == Other.IP6Value;
		}

		const Type_BigInt Get_Value() const {
			return IP6Value;
		}
};

#define TYPE_RANGE_IP6ADDRESS_VALIDCHARS		"01234567890abcdefABCDEF-!:"

class Type_Range_IP6Address : public Type_Range {
	private:
		bool MinValid, MaxValid;
		bool Negated;
		Type_IP6Address Min, Max;
	public:
		Type_Range_IP6Address() {
		}

		Type_Range_IP6Address(const std::string &IP6RngString) {
			if (IP6RngString.length() == 0) {
				MinValid = false;
				MaxValid = false;
				Negated = false;
				return;
			}
			if (IP6RngString.find_first_not_of(TYPE_RANGE_IP6ADDRESS_VALIDCHARS) != std::string::npos) {
				TYPEEXCEPTION("Illegal character for IPv6 range at position " << (IP6RngString.find_first_not_of(TYPE_RANGE_IP6ADDRESS_VALIDCHARS) + 1));
			}

			std::string Copy;
			Negated = (IP6RngString.at(0) == '!');
			if (Negated) {
				Copy = IP6RngString.substr(1);
			} else {
				Copy = IP6RngString;
			}

			if (IP6RngString.find('-') == std::string::npos) {
				/* No range, a single integer */
				MinValid = true;
				MaxValid = true;
				Min = Type_IP6Address(Copy);
				Max = Type_IP6Address(Copy);
			} else {
				std::size_t Pos = Copy.find('-');
				MinValid = true;
				MaxValid = true;
				Min = Type_IP6Address(Copy.substr(0, Pos).c_str());
				Max = Type_IP6Address(Copy.substr(Pos + 1).c_str());
			}
		}
		
		Type_Range_IP6Address(const Type_IP6Address& IP6Addr) {
			MinValid = true;
			Min = Type_IP6Address(IP6Addr);
			Max = Type_IP6Address(IP6Addr);
			MaxValid = true;
			Negated = false;
		}
		
		Type_Range_IP6Address(const ipv6_addr_t& IP6Addr) {
			MinValid = true;
			Min = Type_IP6Address(IP6Addr);
			Max = Type_IP6Address(IP6Addr);
			MaxValid = true;
			Negated = false;
		}

		Type_Range_IP6Address(const Type_IP6Address& From, bool FromValid, const Type_IP6Address& To, bool ToValid, bool Negated) {
			MinValid = FromValid;
			Min = From;
			MaxValid = ToValid;
			Max = To;
			this->Negated = Negated;
		}

		bool Is_Satisfied(const Type_IP6Address &SingleVal) const {
			bool Satisfied = true;
			if (MinValid) Satisfied = Satisfied && (SingleVal >= Min);
			if (MaxValid) Satisfied = Satisfied && (SingleVal <= Max);
			if (Negated) Satisfied = !Satisfied;
			return Satisfied;
		}
	
		bool Is_Valid() const {
			return MinValid || MaxValid || Negated;
		}

		void Get_Value(Type_IP6Address& From, bool& FromValid, Type_IP6Address& To, bool& ToValid, bool& Negated) const {
			From = this->Min;
			FromValid = this->MinValid;
			To = this->Max;
			ToValid = this->MaxValid;
			Negated = this->Negated;
		}
};

#endif
