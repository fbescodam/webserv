#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>

void trimQoutes(std::string &body)
{
	body.erase(0, body.find_first_not_of("\""));
	body.erase(body.find_last_not_of("\""));
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

	std::string body = data.substr(data.find("\r\n\r\n"));
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
		fname = line.substr(name + 9, line.find("\n"));
		trimQoutes(fname);	
		if (fname.back()== '\"')
			fname.pop_back();
	}

	std::fstream file;
	file.open("examples/www/delete/" + fname, std::fstream::out);
	if (!file.good())
	{
		std::string fuck("example/www/delete/" + fname);
		std::cerr << "HTTP/1.1 200 OK\nContent-type: text/html\nContent-length: "<<fuck.size()<<"\n\n"<<fuck;
		exit(1);
	}
	std::getline(bodyss, line);
writeloop:
	std::getline(bodyss, line);
	if (line.find(boundary) != std::string::npos)
	{
		line.erase(line.find_last_not_of("\n"));
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