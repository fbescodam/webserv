/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   KeyValueVerifier.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/20 20:09:48 by fbes          #+#    #+#                 */
/*   Updated: 2022/08/19 11:48:00 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "KeyValueVerifier.hpp"

// no ructors or destructors as this is basically just a holder class...

//////////////////////////////////////////

void ft::KeyValueVerifier::parseInt(VERIFIER_PARAMS_DEF)
{
	long bignum = std::stol(value);
	if (bignum <= 0 || bignum > INT_MAX)
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parseUShort(VERIFIER_PARAMS_DEF)
{
	long bignum = std::stol(value);
	if (bignum <= 0 || bignum > USHRT_MAX)
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parsePath(VERIFIER_PARAMS_DEF)
{
	if (value.length() == 0)
		throw ft::InvalidFieldValueException(lineNum);
	if (value.front() != '/')
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parseNonEmptyString(VERIFIER_PARAMS_DEF)
{
	if (value.length() == 0)
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parseYesNo(VERIFIER_PARAMS_DEF)
{
	if (value != "yes" && value != "no")
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parseRedir(VERIFIER_PARAMS_DEF)
{
	// check if string is splittable on a space
	size_t space = value.find(' ');
	if (space == std::string::npos)
		throw ft::InvalidFieldValueException(lineNum);

	// check if status code is a redirect
	int32_t statusCode = std::stoi(value.substr(0, space));
	if (statusCode != 301 && statusCode != 302 && statusCode != 307 && statusCode != 308)
		throw ft::InvalidFieldValueException(lineNum);

	// count amount of spaces
	std::string::difference_type n = std::count(value.begin(), value.end(), ' ');
	if (n > 1)
		throw ft::InvalidFieldValueException(lineNum);
}

void ft::KeyValueVerifier::parseCGIBin(VERIFIER_PARAMS_DEF)
{
	// count amount of spaces
	std::string::difference_type n = std::count(value.begin(), value.end(), ' ');
	if (n != 1)
		throw ft::InvalidFieldValueException(lineNum);
}

//////////////////////////////////////////

void ft::KeyValueVerifier::setupParseFunctions(std::unordered_map<std::string, ft::KeyValueVerifier::parserFunc> &fieldParseFunctions)
{
	// Integers
	fieldParseFunctions.emplace("limit_body_size", &ft::KeyValueVerifier::parseInt);

	// Unsigned shorts
	fieldParseFunctions.emplace("listen", &ft::KeyValueVerifier::parseUShort);

	// Paths
	fieldParseFunctions.emplace("path", &ft::KeyValueVerifier::parsePath);
	fieldParseFunctions.emplace("upload_dir", &ft::KeyValueVerifier::parsePath);
	fieldParseFunctions.emplace("error_403", &ft::KeyValueVerifier::parsePath);
	fieldParseFunctions.emplace("error_404", &ft::KeyValueVerifier::parsePath);

	// Strings
	fieldParseFunctions.emplace("server_name", &ft::KeyValueVerifier::parseNonEmptyString);
	fieldParseFunctions.emplace("index", &ft::KeyValueVerifier::parseNonEmptyString);
	fieldParseFunctions.emplace("methods", &ft::KeyValueVerifier::parseNonEmptyString);

	// Booleans
	fieldParseFunctions.emplace("access", &ft::KeyValueVerifier::parseYesNo);
	fieldParseFunctions.emplace("dir_listing", &ft::KeyValueVerifier::parseYesNo);

	// Special cases
	fieldParseFunctions.emplace("redir", &ft::KeyValueVerifier::parseRedir);
	fieldParseFunctions.emplace("cgi_bin", &ft::KeyValueVerifier::parseCGIBin);
}

//////////////////////////////////////////

void ft::KeyValueVerifier::run(VERIFIER_PARAMS_DEF)
{
	static std::unordered_map<std::string, ft::KeyValueVerifier::parserFunc> fieldParseFunctions;
	if (fieldParseFunctions.empty())
		this->setupParseFunctions(fieldParseFunctions);

	// Run the function for the key to verify, which is defined in the map
	(this->*(fieldParseFunctions[VERIFIER_PARAMS_KEY]))(VERIFIER_PARAMS);
}
