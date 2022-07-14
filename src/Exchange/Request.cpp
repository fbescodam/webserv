/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:12 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/14 14:33:41 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//all valid header field characters
// "!#$%&'*+-.^_`|~0123456789abdefghijklmnopqrstuvwxyzABCDEFGHJIKLMNOPQRSTUVWVXYZ";

void parseLineOne(ft::Request &req, std::string lineOne)
{
	std::vector<std::string> fields;

	ft::split(lineOne, " ", fields);
	if (fields.size() != 3)
		throw ft::BadRequest();

	req.setMethod(fields[0]);
	req.path = fields[1];
	req.version = fields[2];
}

ft::Request::Request(std::string buffer)
{
	std::vector<std::string> splitBuffer;
	ft::split(buffer, "\n\n", splitBuffer);
	if (splitBuffer.size() == 0)
		throw ft::BadRequest();
	std::istringstream iss(splitBuffer[0]);

	std::vector<std::string> tempFields;
	ft::trim(splitBuffer[0]);
	ft::split(splitBuffer[0], "\n", tempFields);

	parseLineOne(*this, tempFields[0]);
	tempFields.erase(tempFields.begin());

	std::pair<std::string, std::string> output;
	for (std::string &val : tempFields)
	{
		ft::trim(val);
		val.erase(remove_if(val.begin(), val.end(), [](char c){return !(c>=32);}), val.end());
		if (!val.empty())
		{
			if (val.find(':') == std::string::npos)
				throw ft::BadRequest();
			ft::slice(val, ':', output);
			ft::trim(output.second);
			if (output.second.empty())
				throw ft::BadRequest();
			this->fields[output.first] = output.second;
		}
	}

	if (splitBuffer.size() > 1)
		this->body = splitBuffer[1];
}

//////////////////////////////////////////

void ft::Request::display(void) const
{
	std::cout << static_cast<int>(method) << "-";
	std::cout << path << "-";
	std::cout << version << std::endl;

	for (const auto [key, value] : fields)
		std::cout << key << ":" << value << std::endl;
}

void ft::Request::setMethod(const std::string& inMethod)
{
	if (inMethod == "GET")
		method = ft::Method::GET;
	else if (inMethod == "POST")
		method = ft::Method::POST;
	else if (inMethod == "DELETE")
		method = ft::Method::DELETE;
	else
		method = ft::Method::ERR;
}

//////////////////////////////////////////
