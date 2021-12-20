#ifndef __OUTPUT_HPP__
#define __OUTPUT_HPP__

#include <ostream>
#include <set>

#include "Rule_TCP.hpp"
#include "Rule_UDP.hpp"
#include "Rule_Netlink.hpp"
#include "Types.hpp"
#include "Output.hpp"

std::ostream& operator<<(std::ostream &Strm, enum Destiny Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_Range_Int &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_String &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_Range_String &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_BigInt &bigInt);
std::ostream& operator<<(std::ostream &Strm, const Type_Range_IPAddress &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_IP6Address &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_Range_IP6Address &Obj);
std::ostream& operator<<(std::ostream &Strm, const Type_Range_RelTime &Obj);
std::ostream& operator<<(std::ostream &Strm, const Rule &Obj);

#endif
