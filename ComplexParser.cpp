#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>

#include "GenericException.hpp"
#include "StringParser.hpp"
#include "ComplexParser.hpp"

ComplexParser::ComplexParser(const std::string &Filename) : ParseSource(*(new std::ifstream(Filename.c_str()))) {
	if (!ParseSource) GENERICSTRMEXCEPTION("Couldn't open file '" << Filename << "' for reading.");
	ManuallyOpened = true;
	CurrentLineNumber = 0;
}

ComplexParser::ComplexParser(std::ifstream &ParseSource, unsigned int CurrentLineNumber) : ParseSource(ParseSource), CurrentLineNumber(CurrentLineNumber) {
	ManuallyOpened = false;
}

bool ComplexParser::ParseNext() {
	if (ParseSource.eof()) return false;

	SectionName = "";
	Properties.clear();

	const unsigned int BufferSize = 256;
	char LineBuffer[256];
	bool HaveBlock = false;
	bool InBlock = false;
	while (ParseSource.getline(LineBuffer, BufferSize), CurrentLineNumber++, !ParseSource.eof()) {
		std::string Line(LineBuffer);
		StringParser Parser(Line);
		if (!Parser.IsValid()) {
			if (InBlock && (Line == "}")) {
				InBlock = false;
				break;
			}
			if (!InBlock && (Line == "-")) {
				return false;
			}
			continue;
		}

		if (!InBlock) {
			if (Parser(0) == "{") {
				InBlock = true;
				HaveBlock = true;
				SectionName = Parser.GetKey();
			} else {
				GENERICSTRMEXCEPTION("Expected start of a block in line number " << CurrentLineNumber << ", but block was not initiated by '{' charachter.");
			}
		} else {
			Properties[Parser.GetKey()] = Parser;
		}
	}
	if (InBlock) GENERICSTRMEXCEPTION("Hit end of file in line number " << CurrentLineNumber << " although last block named " << SectionName << " hasn't been closed yet.");
	return HaveBlock;
}

bool ComplexParser::HasProperty(const std::string &Property) const {
	std::map<std::string, StringParser>::const_iterator i;
	i = Properties.find(Property);
	return (i != Properties.end());
}

const StringParser& ComplexParser::GetProperty(const std::string &Property) const {
	std::map<std::string, StringParser>::const_iterator i;
	i = Properties.find(Property);
	EXCEPTIONASSERTION(i != Properties.end(), "No such property '" << Property << "' in StringParser.");
	return i->second;
}

std::string ComplexParser::ForceGetPropertyString(const std::string &Property) const {
	std::map<std::string, StringParser>::const_iterator i;
	i = Properties.find(Property);
	if (i == Properties.end()) return "";
	return (i->second)(0);
}

void ComplexParser::CheckPrerequisite(const std::string &Property) const {
	if (!HasProperty(Property)) {
		GENERICSTRMEXCEPTION("Section '" << SectionName << "' has required attribute '" << Property << "' as a prerequisite - but it is missing.");
	}
}

void ComplexParser::CheckUnrecognizedKeywords(const char **ValidKeywords) const {
	std::set<std::string> Recognized;
	while (*ValidKeywords) {
		Recognized.insert(*ValidKeywords);
		ValidKeywords++;
	}

	std::map<std::string, StringParser>::const_iterator i;
	for (i = Properties.begin(); i != Properties.end(); i++) {
		if (Recognized.find(i->first) == Recognized.end()) {
			GENERICSTRMEXCEPTION("Section '" << SectionName << "' contains the unrecognized keyword '" << i->first << "', block ends in line " << CurrentLineNumber << ".");
		}
	}
}

ComplexParser::~ComplexParser() {
	if (ManuallyOpened) delete &ParseSource;
}

