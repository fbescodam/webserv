/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fileupload.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <main@w2wizard.dev>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/08/17 10:35:10 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/18 22:09:59 by pvan-dij      ########   odam.nl         */
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
	
	std::replace(fileName.begin(), fileName.end(), ' ', '+');
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

void GenerateResponse(const std::string &uploadPath, const std::vector<std::string> &fileNames)
{
	std::string out;

	out += "<html><head><title>Uploaded files</title></head>";
	out += "<body><h1>Uploaded Files</h1><hr><pre>";

	for (const std::string &val : fileNames)
	{
		out += "<a href=\"" + uploadPath + "/" + val + "\">";
		out += val;
		out += "</a>\n";
	}

	out += "</pre><hr></body></html>";

	std::cout << "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(out.size()) + "\n\n" + out;
}


void GenerateResponse(const std::string &uploadPath, const std::vector<std::string> &fileNames, const std::vector<std::string> &formData)
{
	std::string out;

	out += "<html><head><title>Uploaded files</title></head>";
	out += "<body><h1>Uploaded Files</h1><hr><pre>";

	// Filenames + links
	for (const std::string &val : fileNames)
	{
		out += "<a href=\"" + uploadPath + "/" + val + "\">";
		out += val;
		out += "</a>\n";
	}
	out += "</pre><hr>";

	if (!formData.empty())
	{
		out += "<pre>";
		// Formdata in a table
		out += "<table><tr><th>field</th><th>value</th></tr>";
		for (const std::string &val : formData)
		{
			out += "<tr>";
			size_t delimPos = val.find('=');
			out += "<td>" + val.substr(0, delimPos) + "</td>";
			out += "<td>" + val .substr(delimPos + 1) + "</td>";
			out += "</tr>"; //TODO: actually parse val
		}
		out += "</table></pre><hr>";
	}
	
	out += "</body></html>";
	std::cout << "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(out.size()) + "\n\n" + out;
}

// Parse multipart file data, handles multipe files
void parseMultipart(const std::string &data, const std::string &cType, const std::string &uploadDir, const std::string &uploadPath)
{
	std::vector<std::string> fileNames;
	std::vector<std::string> formData;

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
		{
			formData.push_back(val);
			continue;
		}
		else
			fname = getFileName(namePos + 9, val);
		
		if (fname.empty())
			continue;
		fileNames.push_back(fname);
	
		// Erase all content info and write the data to file
		val.erase(0, val.find("\r\n\r\n"));
		val.erase(0, val.find_first_not_of(" \t\r\n\t\f\v"));
		writeDataToFile(fname, val, uploadDir);
	}

	GenerateResponse(uploadPath, fileNames, formData);

}

// If content type is not multipart just create file and write data to it
void makeFile(const std::string &data, const std::string &uploadDir, const std::string &uploadPath)
{
	std::fstream file;
	std::vector<std::string> fileNames;

	std::string body = data.substr(data.find("\r\n\r\n"));
	body.erase(0, body.find_first_not_of("\r\n"));

	std::string fName = std::to_string(std::time(0));
	writeDataToFile(fName, body, uploadDir);
	fileNames.push_back(fName);

	GenerateResponse(uploadPath, fileNames);

}


// application/x-www-form-urlencoded
void parseFormData(const std::string &data)
{
    std::string body = data.substr(data.find("\r\n\r\n"));
    body.erase(0, body.find("\r\n\r\n"));
    body.erase(0, body.find_first_not_of(" \t\r\n\t\f\v"));

    std::vector<std::string> formData;
    cppSplit(body, formData, "&");

    std::string out = "<table><tr><th>field</th><th>value</th></tr>";
    for (const std::string &val : formData)
    {
        out += "<tr>";
        size_t delimPos = val.find('=');
        out += "<td>" + val.substr(0, delimPos) + "</td>";
        out += "<td>" + val .substr(delimPos + 1) + "</td>";
        out += "</tr>";
    }
    out += "</table>";

    std::cout << "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(out.size()) + "\n\n" + out;
}

int main(int ac, char **av, char **envp)
{
	(void)ac;
	(void)av;

	// Get data from cin
	std::istreambuf_iterator<char> begin(std::cin), end;
	std::string data(begin, end);

	// Get upload_dir from envp
	std::string uploadDir;
	std::string uploadPath;
	while (envp)
	{
		if (!*envp)
			break;
		std::string var(*envp);
		if (var.find("UPLOAD_DIR") != std::string::npos)
			uploadDir = var.substr(var.find_first_of("=") + 1);
		if (var.find("UPLOAD_PATH") != std::string::npos)
			uploadPath = var.substr(var.find_first_of("=") + 1);
		envp++;
	}
	if (!doesPathExist(uploadDir))
		mkdir(uploadDir.c_str(), 0700);
	
	// vector for storinga all the uploaded filenames
	std::vector<std::string> fileNames;

	// Get the content type from data
	auto cTypeIt = data.find("Content-Type");
	std::string cType = data.substr(cTypeIt, data.find("\r\n", cTypeIt) - cTypeIt);
    if (cType.find("multipart/form-data") != std::string::npos)
        parseMultipart(data, cType, uploadDir, uploadPath);
    else if (cType.find("application/x-www-form-urlencoded") != std::string::npos)
        parseFormData(data);
    else
        makeFile(data, uploadDir, uploadPath);
}
