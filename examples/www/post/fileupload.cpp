#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>

void trimQoutes(std::string &body)
{
	if (body.empty())
		return;
	body.erase(0, body.find_first_not_of("\""));
	body.erase(body.find_last_not_of("\"") + 1);
}

bool fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

void cppSplit(std::string string, std::vector<std::string> &outVector, const std::string &delim)
{
	size_t pos;
	string.erase(0, string.find_first_not_of(" \t\r\n\t\f\v"));
	string.erase(string.find_last_not_of(" \t\r\n\t\f\v"));

	while ((pos = string.find(delim)) != std::string::npos) 
	{
		std::string temp = string.substr(0, pos);
		if (!temp.empty())
			outVector.push_back(string.substr(0, pos));
		string.erase(0, pos + delim.length());
	}
}

void parseMultipart(std::string data, std::string cType)
{
	time_t lol = std::time(0);
	auto boundaryIt = cType.find_first_of("=");
	std::string boundary = "--" + cType.substr(boundaryIt + 1, cType.find("\n") - boundaryIt -1);
	std::string body = data.substr(data.find("\r\n\r\n"));

	std::vector<std::string> bodyContent;
	cppSplit(body, bodyContent, boundary);

	for (std::string &val: bodyContent)
	{
		if (val.empty())
			continue;
		std::string fname;
		auto name = val.find("filename=");
		if (name == std::string::npos)
			fname = std::to_string(lol++);
		else
		{
			fname = val.substr(name + 9, val.find_first_of("\t\r\n\t\f\v", name + 9) - (name + 9));
			trimQoutes(fname);	
			if (fname.back()== '\"')
				fname.pop_back();
		}	
		val.erase(0, val.find("\r\n\r\n"));
		val.erase(0, val.find_first_not_of(" \t\r\n\t\f\v"));

		std::fstream file;
		file.open("examples/www/delete/" + fname, std::fstream::out);
		if (!file.good())
		{
			std::string fuck("example/www/delete/" + fname);
			std::cerr << "HTTP/1.1 200 OK\nContent-type: text/html\nContent-length: "<<fuck.size()<<"\n\n"<<fuck;
			exit(1);
		}
		file.write(val.data(), val.size());
		file.close();
	}

}

void makeFile(std::string data)
{
	std::fstream file;

	file.open("../delete/" + std::to_string(std::time(0)), std::fstream::out);
	if (!file.good())
	{
		std::cerr << "HTTP/1.1 200 OK\nContent-type: text/html\nContent-length: 4\n\nfuck";
		exit(1);
	}
	std::string body = data.substr(data.find("\r\n\r\n"));
	body.erase(0, body.find_first_not_of("\r\n"));
	file.write(body.data(), body.size());
	file.close();
}

int main(int ac, char **av)
{
	std::istreambuf_iterator<char> begin(std::cin), end;
	std::string data(begin, end);

	auto cTypeIt = data.find("Content-Type");
	std::string cType = data.substr(cTypeIt, data.find("\r\n", cTypeIt) - cTypeIt);
	if (cType.find("multipart/form-data") == std::string::npos)
		makeFile(data);
	else
		parseMultipart(data, cType);

	std::cout << "HTTP/1.1 302 Found\nLocation: /delete\n\n";


}