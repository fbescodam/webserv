/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fileupload.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <main@w2wizard.dev>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/08/17 10:35:10 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/17 15:10:00 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>
#include <sys/stat.h>


bool doesPathExist(const std::string &s)
{
	struct stat buffer;
	return (stat (s. c_str(), &buffer) == 0);
}

void trimQoutes(std::string& body)
{
	if (body.empty())
		return;
	body.erase(0, body.find_first_not_of("\""));
	body.erase(body.find_last_not_of("\"") + 1);
}

void trimWhiteSpace(std::string &string)
{
	if (string.empty())
		return;
	string.erase(0, string.find_first_not_of(" \t\r\n\t\f\v"));
	string.erase(string.find_last_not_of(" \t\r\n\t\f\v"));
}

bool fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

void cppSplit(std::string string, std::vector<std::string> &outVector, const std::string &delim)
{
	size_t pos;

	trimWhiteSpace(string);
	while ((pos = string.find(delim)) != std::string::npos) 
	{
		std::string temp = string.substr(0, pos);
		if (!temp.empty())
			outVector.push_back(string.substr(0, pos));
		string.erase(0, pos + delim.length());
	}
}

std::string getFileName(size_t name, const std::string& data)
{
	std::string fileName = data.substr(name, data.find_first_of("\t\r\n\t\f\v", name) - (name));
	
	trimQoutes(fileName);	
	if (fileName.back()== '\"')
		fileName.pop_back();
	
	return fileName;
}

// Create a file based on fname and write val to it
void writeDataToFile(const std::string &fname, const std::string &val, const std::string &uploadDir)
{
	std::fstream file;

	file.open(uploadDir + "/" + fname, std::fstream::out);
	if (!file.good())
	{
		std::cerr << "HTTP/1.1 500 Internal Server Error\nContent-type: text/html\nContent-length: 21\r\n\r\nInternal Server Error";
		exit(1);
	}
	file.write(val.data(), val.size());
	file.close();
}

// Parse multipart file data, handles multipe files
void parseMultipart(std::string data, const std::string &cType, const std::string &uploadDir)
{
	// Set variables, substring magic to get all the data we need
	time_t randomFileName = std::time(0);
	size_t boundaryIt = cType.find_first_of("=");
	std::string boundary = "--" + cType.substr(boundaryIt + 1, cType.find("\n") - (boundaryIt - 1));
	std::string body = data.substr(data.find("\r\n\r\n"));

	// Split up the multipart data in single chunks
	std::vector<std::string> bodyContent;
	cppSplit(body, bodyContent, boundary);

	// Loop over the chunks and create files from them
	std::string fname;
	for (std::string &val: bodyContent)
	{
		if (val.empty())
			continue;

		size_t namePos = val.find("filename=");
		if (namePos == std::string::npos)
			fname = std::to_string(randomFileName++);
		else
			fname = getFileName(namePos + 9, val);
		
		// Erase all content info and write the data to file
		val.erase(0, val.find("\r\n\r\n"));
		val.erase(0, val.find_first_not_of(" \t\r\n\t\f\v"));
		writeDataToFile(fname, val, uploadDir);
	}

}

// If content type is not multipart just create file and write data to it
void makeFile(std::string data, const std::string &uploadDir)
{
	std::fstream file;

	std::string body = data.substr(data.find("\r\n\r\n"));
	body.erase(0, body.find_first_not_of("\r\n"));

	writeDataToFile(std::to_string(std::time(0)), body, uploadDir);
}

int main(int ac, char **av, char **envp)
{
	// Get data from cin
	std::istreambuf_iterator<char> begin(std::cin), end;
	std::string data(begin, end);

	// Get upload_dir from envp
	std::string uploadDir;
	while (envp)
	{
		std::string var(*envp);
		if (var.find("UPLOAD_DIR") != std::string::npos)
		{
			uploadDir = var.substr(var.find_first_of("=") + 1);
			break;
		}
		envp++;
	}
	if (!doesPathExist(uploadDir))
		mkdir(uploadDir.c_str(), 0700);


	// Get the content type from data
	auto cTypeIt = data.find("Content-Type");
	std::string cType = data.substr(cTypeIt, data.find("\r\n", cTypeIt) - cTypeIt);
	if (cType.find("multipart/form-data") == std::string::npos)
		makeFile(data, uploadDir);
	else
		parseMultipart(data, cType, uploadDir);

	//TODO: fix this
	std::cout << "HTTP/1.1 302 Found\nLocation: /delete\n\n";
}