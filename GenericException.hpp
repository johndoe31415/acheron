#ifndef GENERICEXCEPTION_HPP
#define GENERICEXCEPTION_HPP

#define GENERICEXCEPTION(text)						GenericException(__FILE__, __LINE__, __PRETTY_FUNCTION__, text)
#define GENERICSTRMEXCEPTION(stream)				{ std::stringstream Strm; Strm << stream; throw GENERICEXCEPTION(Strm.str()); }

#ifdef DEBUG
#define EXCEPTIONASSERTION(assertion, stream)		if (!(assertion)) { std::stringstream Strm; Strm << stream; throw GENERICEXCEPTION(Strm.str()); }
#else
#define EXCEPTIONASSERTION(assertion, stream)
#endif

class GenericException {
	private:
		std::string Location;
		int Line;
		std::string Function;
		std::string Text;
	public:
		GenericException(const std::string &Location, int Line, const std::string &Function, const std::string &Text);
		void Terminate();
		~GenericException();

	friend std::ostream& operator<<(std::ostream &Strm, GenericException &Obj);
};

#endif
