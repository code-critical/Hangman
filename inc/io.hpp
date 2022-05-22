#ifndef IO_HPP
#define IO_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::stringstream getFileData(std::string filename);

void setFileData(std::string filename, std::stringstream& data);

#endif