/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   KeyValueVerifier.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/20 20:09:48 by fbes          #+#    #+#                 */
/*   Updated: 2022/07/20 21:32:50 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "KeyValueVerifier.hpp"
#include <unordered_map>

// no ructors or destructors as this is basically just a holder class...

//////////////////////////////////////////

void ft::KeyValueVerifier::parseInt(VERIFIER_PARAMS)
{

}

void ft::KeyValueVerifier::parseUShort(VERIFIER_PARAMS)
{

}

void ft::KeyValueVerifier::parseNonEmptyString(VERIFIER_PARAMS)
{

}

void ft::KeyValueVerifier::parseYesNo(VERIFIER_PARAMS)
{

}

void ft::KeyValueVerifier::parseRedir(VERIFIER_PARAMS)
{

}

void ft::KeyValueVerifier::parseCGIBin(VERIFIER_PARAMS)
{

}

//////////////////////////////////////////

void ft::KeyValueVerifier::run(VERIFIER_PARAMS)
{
	static std::unordered_map<std::string, ft::KeyValueVerifier::parserFunc> fieldParseFunctions;
	if (fieldParseFunctions.empty())
	{
		// fieldParseFunctions.emplace("limit_body_size", &ft::KeyValueVerifier::parseInt);
		fieldParseFunctions["limit_body_size"] = &ft::KeyValueVerifier::parseInt;
	}
}
