#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>

void trimQoutes(std::string &body)
{
	body.erase(0, body.find_first_not_of("\""));
	body.erase(body.find_last_not_of("\"") + 1);
}

bool fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

void parseMultipart(std::string data, std::string cType)
{
	time_t lol = std::time(0);
	auto boundaryIt = cType.find_first_of("=");
	std::string boundary = "--" + cType.substr(boundaryIt + 1, cType.find("\n") - boundaryIt -1);

	std::string body = data.substr(data.find("\n\n"));
	body.erase(0, body.find_first_not_of("\r\n"));
	std::stringstream bodyss;
	bodyss.str(body);
	std::string line;
	std::getline(bodyss, line);

lbl:
	std::getline(bodyss, line);
	auto name = line.find("filename=");
	std::string fname;
	if (name == std::string::npos)
		fname = std::to_string(lol++);
	else
	{
		fname = line.substr(name + 10, line.find("\n"));
		trimQoutes(fname);	
	}
	std::fstream file;
	file.open("../delete/" + fname, std::fstream::out);
	if (!file.good())
	{
		std::cerr << "shits fucked"<<std::endl;
		exit(1);
	}
	std::getline(bodyss, line);
writeloop:
	std::getline(bodyss, line);
	if (line.find(boundary) != std::string::npos)
	{
		file.close();
		if (line.back() == '-')
			return;
		goto lbl;
	}
	file.write(line.data(), line.size());
	goto writeloop;
}

void makeFile(std::string data)
{
	std::fstream file;

	file.open("../delete/" + std::to_string(std::time(0)), std::fstream::out);
	if (!file.good())
	{
		std::cerr << "shits fucked"<<std::endl;
		exit(1);
	}
	std::string body = data.substr(data.find("\r\n\r\n"));
	body.erase(0, body.find_first_not_of("\r\n"));
	file.write(body.data(), body.size());
	file.close();
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "shits fucked"<<std::endl;
		exit(1);
	}

	std::string data(av[1]);
	auto cTypeIt = data.find("Content-Type");
	std::string cType = data.substr(cTypeIt, data.find("\r\n", cTypeIt) - cTypeIt);
	if (cType.find("multipart/form-data") == std::string::npos)
		makeFile(data);
	else
		parseMultipart(data, cType);
}