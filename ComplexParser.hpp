#ifndef __COMPLEXPARSER_HPP__
#define __COMPLEXPARSER_HPP__

#include <string>
#include <map>
#include <fstream>

#include "StringParser.hpp"

class ComplexParser {
	private:
		std::string SectionName;
		std::map <std::string, StringParser> Properties;
		std::ifstream &ParseSource;
		unsigned int CurrentLineNumber;
		bool ManuallyOpened;
	public:
		ComplexParser(const std::string &Filename);
		ComplexParser(std::ifstream &ParseSource, unsigned int CurrentLineNumber = 0);
		bool ParseNext();
		bool HasProperty(const std::string &Property) const;
		const StringParser& GetProperty(const std::string &Property) const;
		std::string ForceGetPropertyString(const std::string &Property) const;
		// {{{ inline const std::string& GetSectionName() const;
		inline const std::string& GetSectionName() const {
			return SectionName;
		}
		// }}}
		void CheckPrerequisite(const std::string &Property) const;
		// {{{ inline unsigned int GetLineNumber() const;
		inline unsigned int GetLineNumber() const {
			return CurrentLineNumber;
		}
		// }}}
		// {{{ inline const std::map<std::string, StringParser>& GetProperties() const;
		inline const std::map<std::string, StringParser>& GetProperties() const {
			return Properties;
		}
		// }}}
		void CheckUnrecognizedKeywords(const char **ValidKeywords) const;
		~ComplexParser();
};
#endif

