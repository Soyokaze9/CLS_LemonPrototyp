/* Sample code that reads comma separated integer values from a file
 *
 * Daniel Schmidt, 2018
 * modified for project
 * */

#include "filereader.hpp"
#include <string>
#include <limits.h>
#include <unistd.h>
#include <cstring>



std::string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}

// Cast a string to an arbitrary type (includes error checking)
template <typename T>
const T to(const std::string& sval);

// Remove all whitespace from the beginning of a string
std::string& ltrim(std::string& str);

// Remove all whitespace from the end of a string
std::string& rtrim(std::string& str);

// Remove all whitespace from the beginning and from the end of the string; return a copy
std::string trim_copy(std::string const & str);


// The actual code.
int readIntsFromFile(char* filenameIn,Data& data, int (&arr)[2], char delimiter /*= ','*/){

	if ((filenameIn != NULL) && (filenameIn == '\0')){
		std::cerr << "Missing input parameter: Filename." << std::endl;
		return 1;
	}

	//Data data = &data;

	// First parameter is the name of the input file
	std::string filename(filenameIn);

	//std::cout << getexepath() << std::endl;

	//std::cout << std::filesystem::current_path() <<std::endl;

	std::cout << "Using delimiter: '" << delimiter << "'" << std::endl;

	// We read the file into this array
	//Data data; now parameter

	/*
	std::ifstream f;
	// Set exceptions to be thrown on failure
	f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
	    f.open(filenameIn);
	} catch (std::system_error& e) {
	    std::cerr << e.code().message() << std::endl;
	}*/

	// Try to open the file
	std::ifstream input(filename);
	if(!input){
		std::cerr << "Error ifstream input: " << strerror(errno)<<std::endl;
		throw ParsingException("File is unreadable.");
	}

	// Read the file line by line. We count how many lines we have read
	// (mostly to produce useful error messages).
	int curLine = 0;
	std::string line;
	int max = 0;
	int min = INT_MAX;

	// Read a line from 'input' and store it in 'line'
	while(getline(input, line)){
		++curLine;

		// Remove starting and ending whitespace from line
		line = trim_copy(line);
		if(line == "") continue;

		std::cout << line << std::endl;

		std::size_t found = line.find("#");
		if (found!=std::string::npos){
			  std::cout << "ignored line comment" << std::endl;
			  continue;
		}

		// This vector will hold the data read from the current line
		std::vector<int> lineData;

		// Convert line into a stringstream so that we can call getline
		std::stringstream lineStream;
		lineStream << line;

		// Parse the current line with the help of getline:
		// getline can be called with a third parameter that specifies
		// the line separator. Effectively, this call reads from the
		// current position to the next occurence of ',' and then
		// continues after the ','.
		std::string valueString;

		while(getline(lineStream, valueString, delimiter)){
			// Try to read an integer and throw an exception on failure
			// first check if valueString is comment line, ignore it it is

			//std::cout << valueString << std::endl;

			found = valueString.find("#");
			  if (found!=std::string::npos){
				  std::cout << "ignored comment" << std::endl;
			  }else{
				  int value = -1;

				  			try{
				  				value = to<int>(valueString);
				  				//keep track of max and min
				  				max= (value>max)?value:max;
				  				min= (value<min)?value:min;
				  			}
				  			catch(std::runtime_error& e){
				  				throw ParsingException("Input value is not integer", curLine);
				  			}

				  			// Store the value we just read
				  			lineData.push_back(value);
			  }
		}

		// Shrink the vector to exactly fit the number of values we read
		lineData.shrink_to_fit();

		// Store the line we just read, but only if it is non-empty
		if(lineData.size()> 0){
			data.push_back(lineData);
		}
	}
	// Shrink data to the number of lines we have read.
	data.shrink_to_fit();

	/*
	// Output to see if everything was read correctly
	for(std::size_t i=0; i < data.size(); ++i){
		for(std::size_t j=0; j < data[i].size(); ++j){
			std::cout << std::setw(5) << data[i][j] << " ";
		}
		std::cout << std::endl;
	}*/

	arr[0]=min;
	arr[1]=max;

	return 0;
}

int readDoublesFromFile(char* filenameIn,DataDouble& data, double (&arr)[2], char delimiter /*= ','*/){

	if ((filenameIn != NULL) && (filenameIn == '\0')){
		std::cerr << "Missing input parameter: Filename." << std::endl;
		return 1;
	}

	//Data data = &data;

	// First parameter is the name of the input file
	std::string filename(filenameIn);

	std::cout << "Using delimiter: '" << delimiter << "'" << std::endl;

	// We read the file into this array
	//Data data; now parameter

	/*
	std::ifstream f;
	// Set exceptions to be thrown on failure
	f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
	    f.open(filenameIn);
	} catch (std::system_error& e) {
	    std::cerr << e.code().message() << std::endl;
	}*/

	// Try to open the file
	std::ifstream input(filename);
	if(!input){
		std::cerr << "Error ifstream input: " << strerror(errno)<<std::endl;
		throw ParsingException("File is unreadable.");
	}

	// Read the file line by line. We count how many lines we have read
	// (mostly to produce useful error messages).
	int curLine = 0;
	std::string line;
	double max = 0;
	double min =std::numeric_limits<double>::max();

	// Read a line from 'input' and store it in 'line'
	while(getline(input, line)){
		++curLine;

		// Remove starting and ending whitespace from line
		line = trim_copy(line);
		if(line == "") continue;

		//std::cout << line << std::endl;

		std::size_t found = line.find("#");
		if (found!=std::string::npos){
			  std::cout << "ignored line comment" << std::endl;
			  continue;
		}

		// This vector will hold the data read from the current line
		std::vector<double> lineData;

		// Convert line into a stringstream so that we can call getline
		std::stringstream lineStream;
		lineStream << line;

		// Parse the current line with the help of getline:
		// getline can be called with a third parameter that specifies
		// the line separator. Effectively, this call reads from the
		// current position to the next occurence of ',' and then
		// continues after the ','.
		std::string valueString;

		while(getline(lineStream, valueString, delimiter)){
			// Try to read an integer and throw an exception on failure
			// first check if valueString is comment line, ignore it it is
			std::size_t found = valueString.find("#");
			  if (found!=std::string::npos){
				  std::cout << "ignored comment" << std::endl;
			  }else{
				  double value = -1;
				  			try{
				  				value = to<double>(valueString);
				  				//keep track of max and min
				  				max= (value>max)?value:max;
				  				min= (value<min)?value:min;
				  			}
				  			catch(std::runtime_error& e){
				  				throw ParsingException("Input value is not integer", curLine);
				  			}

				  			// Store the value we just read
				  			lineData.push_back(value);
			  }
		}

		// Shrink the vector to exactly fit the number of values we read
		lineData.shrink_to_fit();

		// Store the line we just read, but only if it is non-empty
		if(lineData.size()> 0){
			data.push_back(lineData);
		}
	}
	// Shrink data to the number of lines we have read.
	data.shrink_to_fit();

	/*
	// Output to see if everything was read correctly
	for(std::size_t i=0; i < data.size(); ++i){
		for(std::size_t j=0; j < data[i].size(); ++j){
			std::cout << std::setw(5) << data[i][j] << " ";
		}
		std::cout << std::endl;
	}
*/
	arr[0]=min;
	arr[1]=max;


	return 0;
}

/* Implementations ************************************************** */
ParsingException::ParsingException(const char *msg, int line) throw() :
    		m_msg(getMsg(msg, line)) {
}

ParsingException::~ParsingException() throw() {
}

const char* ParsingException::what() const throw() {
	return m_msg.c_str();
}

std::string ParsingException::getMsg(const char *msg, int line) {
	std::stringstream msgStream;
	msgStream << "Parsing error in ";
	if (line > 0) {
		msgStream << "line " << line << ": ";
	}
	else {
		msgStream << "unknown line: ";
	}

	msgStream << msg;

	return msgStream.str();
}

// https://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
template <typename T>
const T to(const std::string& sval)
{
	T val;
	std::stringstream sstream;
	sstream << sval;
	sstream >> val;
	if(sstream.fail()){
		throw std::runtime_error((std::string)typeid(T).name() + " type wanted: " + sval);
	}
	return val;
}

// Is a given char a white space char?
bool isNotWhiteSpace(wchar_t c){
	return !iswspace(c);
}

// Remove all whitespace from the beginning of a string
std::string& ltrim(std::string& str)
{
	// Returns an iterator to the first element in str for which isNotWhiteSpace
	// returns true. If no such element exists, find_if returns str.end().
	std::string::iterator it = std::find_if( str.begin(), str.end(), isNotWhiteSpace );

	// Erases the elements in [str.begin, it) .
	str.erase( str.begin(), it);
	return str;
}

// Remove all whitespace from the end of a string
std::string& rtrim(std::string& str)
{
	std::string::reverse_iterator it = std::find_if( str.rbegin(), str.rend(), isNotWhiteSpace );
	str.erase(it.base(), str.end());
	return str;
}


// Remove all whitespace from the beginning and from the end of the string; return a copy
std::string trim_copy(std::string const & str)
{
	std::string s = str;
	return ltrim(rtrim(s));
}


