/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 19:34:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/30 17:30:33 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//////////////////////////////////////////


//////////////////////////////////////////

/** As one giant shit strings
 * CODE - STATUS
 * <HEADER>
 * \n\n 
 * <BODY>
 */

// Note: Im a memory rapist
void ft::Response::send(int32_t socket, const std::vector<uint8_t>& buffer)
{
	std::vector<uint8_t> rawResponse;
	
	// Build Header //
	writeHeader(rawResponse);

	// Build data //
	rawResponse.insert(rawResponse.end(), buffer.begin(), buffer.end());
	ft::send(socket, rawResponse.data(), rawResponse.size(), 0);
}

void ft::Response::send(int32_t socket, const std::string& buffer)
{
	std::vector<uint8_t> rawResponse;
	
	// Build Header //
	writeHeader(rawResponse);

	// Build data //
	rawResponse.insert(rawResponse.end(), buffer.begin(), buffer.end());
	ft::send(socket, rawResponse.data(), rawResponse.size(), 0);
}

void ft::Response::writeHeader(std::vector<uint8_t>& buffer)
{
	std::string temp;
	for (const auto [key, value] : this->fields)
	{
		temp = key + " : " + value + '\n';

		for (size_t i = 0; i < temp.length(); i++)
			buffer.push_back(temp[i]);		
	}
	buffer.push_back('\n');
}