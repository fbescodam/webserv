/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   KeyValueVerifier.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/20 20:13:53 by fbes          #+#    #+#                 */
/*   Updated: 2022/08/19 11:48:04 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef KEYVALUEVERIFIER_HPP
# define KEYVALUEVERIFIER_HPP
# include "Utils.hpp"
# include "Exceptions.hpp"
# include <unordered_map>
# define VERIFIER_PARAMS_DEF uint32_t lineNum, std::string& key, std::string& value
# define VERIFIER_PARAMS lineNum, key, value
# define VERIFIER_PARAMS_KEY key

namespace ft {

// A KeyValueVerifier verifies that a key-value pair is set up correctly
class KeyValueVerifier
{
public: // Ctor ~ Dtor
	KeyValueVerifier() = default;
	~KeyValueVerifier() = default;

public: // Functions
	/**
	 * @brief checks if a value is acceptable for the use with a certain key. Throws an error if something's wrong.
	 *
	 * @param lineNum the line number in the config file where the field is defined
	 * @param key a key
	 * @param value a value
	 */
	void run(VERIFIER_PARAMS_DEF);


public: // Attributes
	typedef void (ft::KeyValueVerifier::*parserFunc)(VERIFIER_PARAMS_DEF);

private: // Functions
	void setupParseFunctions(std::unordered_map<std::string, ft::KeyValueVerifier::parserFunc> &fieldParseFunctions);

	/**
	 * @brief Parse an integer (as a signed long) and check if it's > 0 and < INT_MAX
	 */
	void parseInt(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse an unsigned short (as a signed long) and check if it's > 0 and < USHRT_MAX
	 */
	void parseUShort(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse a non-empty string and check if it starts with a /
	 */
	void parsePath(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse a non-empty string
	 */
	void parseNonEmptyString(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse a boolean: should be a string saying "yes" or "no"
	 */
	void parseYesNo(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse a redirection field. 1st param should be 301, 302, 307 or 308 as string, 2nd a full path to a file to redirect to
	 */
	void parseRedir(VERIFIER_PARAMS_DEF);

	/**
	 * @brief Parse a CGI bin field. 1st param should be a file extension, 2nd a path to an executable
	 */
	void parseCGIBin(VERIFIER_PARAMS_DEF);
};

}
#endif
