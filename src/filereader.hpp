/*
 * filereader.h
 *
 *  Created on: May 12, 2018
 *      Author: soyo
 */

#ifndef FILEREADER_HPP_
#define FILEREADER_HPP_

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <exception>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cwctype>

// Small exception class. We throw this exception if an error occurs
// while we read the file.
class ParsingException : public std::exception {
public:
	ParsingException(const char *msg, int line = -1) throw();
	virtual ~ParsingException() throw();
	virtual const char *what() const throw();
	static std::string getMsg(const char *msg, int line);

private:
	const std::string m_msg;
};

// Type for storing the data we read
typedef std::vector< std::vector<int> > Data;

int readIntsFromFile(char* filenameIn, Data& dataBucket,int (&arr)[2],char delimiter= ',');

typedef std::vector< std::vector<double> > DataDouble;

int readDoublesFromFile(char* filenameIn, DataDouble& dataBucket,double (&arr)[2],char delimiter= ',');



#endif /* FILEREADER_HPP_ */
