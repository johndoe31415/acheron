#ifndef STRINGPARSER_HPP
#define STRINGPARSER_HPP

#include <string>
#include <vector>

class StringParser {
	private:
		std::string Key;
		std::vector <std::string> Values;
		void RemoveWhiteSpaces(std::string &String);
		void RemoveQuotationMarks(std::string &String);
		bool Valid;
	public:
		StringParser();
		StringParser(const std::string &ParseString);
		const std::string& GetKey() const;
		unsigned int GetValueCount() const;
		
		const std::string& operator()(unsigned int Index) const;
		int GetValue(unsigned int Index) const;
		float GetFloatValue(unsigned int Index) const;
		bool GetBoolValue(unsigned int Index) const;

		bool IsValid() const;
		~StringParser();
};

#endif
