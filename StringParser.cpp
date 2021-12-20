#include <string>
#include <vector>
#include <sstream>

#include "GenericException.hpp"
#include "StringParser.hpp"

StringParser::StringParser() {
	Valid = false;
}

StringParser::StringParser(const std::string &ParseString) {
	int SepLen = 1;
	std::string::size_type EqualPos;
	EqualPos = ParseString.find(":=");
	if (EqualPos == std::string::npos) {
		EqualPos = ParseString.find("=");
		if (EqualPos == std::string::npos) {
			/* Doesn't contain equal sign */
			Valid = false;
			return;
		}
	} else {
		SepLen = 2;
	}

	Valid = true;
	Key = ParseString.substr(0, EqualPos);
	RemoveWhiteSpaces(Key);

	if (Key.at(0) == '#') {
		/* Comment line */
		Valid = false;
		return;
	}

	std::string Remainder = ParseString.substr(EqualPos + SepLen);
	std::string::size_type Location;
	while ((Location = Remainder.find(",")) != std::string::npos) {
		std::string Token = Remainder.substr(0, Location);
		RemoveWhiteSpaces(Token);
		RemoveQuotationMarks(Token);
		Values.push_back(Token);
		Remainder = Remainder.substr(Location + SepLen);
	}

	RemoveWhiteSpaces(Remainder);
	RemoveQuotationMarks(Remainder);
	Values.push_back(Remainder);
}

void StringParser::RemoveWhiteSpaces(std::string &String) {
	std::string::size_type First = String.find_first_not_of(std::string(" \t\n"));
	if (First == std::string::npos) {
		/* String consists only of whitespace */
		String = "";
		return;
	}
	
	std::string::size_type Last = String.find_last_not_of(std::string(" \t\n"));
	String = String.substr(First, Last - First + 1);
}

void StringParser::RemoveQuotationMarks(std::string &String) {
	unsigned int Length = String.length();
	if ((String.at(0) == '"') && (String.at(Length - 1) == '"')) {
		String = String.substr(1, Length - 2);
	}
}

const std::string& StringParser::GetKey() const {
	return Key;
}

unsigned int StringParser::GetValueCount() const {
	return Values.size();
}

const std::string& StringParser::operator()(unsigned int Index) const {
	if (Index >= Values.size()) {
		std::stringstream Strm;
		Strm << "Access to element index " << Index << " requested, when StringParser only holds " << Values.size() << " elements.";
		throw GENERICEXCEPTION(Strm.str());
	}
	return Values[Index];
}

int StringParser::GetValue(unsigned int Index) const {
	if (Index >= Values.size()) {
		std::stringstream Strm;
		Strm << "Access to element index " << Index << " requested, when StringParser only holds " << Values.size() << " elements.";
		throw GENERICEXCEPTION(Strm.str());
	}
	return std::atoi(Values[Index].c_str());
}

float StringParser::GetFloatValue(unsigned int Index) const {
	if (Index >= Values.size()) {
		std::stringstream Strm;
		Strm << "Access to element index " << Index << " requested, when StringParser only holds " << Values.size() << " elements.";
		throw GENERICEXCEPTION(Strm.str());
	}
	return std::atof(Values[Index].c_str());
}

bool StringParser::GetBoolValue(unsigned int Index) const {
	return ((GetValue(Index) == 1) || ((*this)(Index) == "true"));
}

bool StringParser::IsValid() const {
	return Valid;
}

StringParser::~StringParser() {
}
