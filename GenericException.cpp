#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>

#include <execinfo.h>
#include <signal.h>

#include "GenericException.hpp"
#include "Logger.hpp"

GenericException::GenericException(const std::string &Location, int Line, const std::string &Function, const std::string &Text) {
	this->Location = Location;
	this->Line = Line;
	this->Function = Function;
	this->Text = Text;
}

std::ostream& operator<<(std::ostream &Strm, GenericException &Obj) {
	Strm << "Exception at " << Obj.Location << ", line " << Obj.Line << std::endl;
	Strm << "     At: " << Obj.Function << std::endl;
	Strm << "     Reason: " << Obj.Text << std::endl;
	return Strm;
}

void GenericException::Terminate() {
	LOGGER_CRIT << (*this);
	Logger::Get().Shutdown();
	abort();
}

GenericException::~GenericException() {
}

