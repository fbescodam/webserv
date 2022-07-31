/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   KeyValueVerifier.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: fbes <fbes@student.codam.nl>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/20 20:13:53 by fbes          #+#    #+#                 */
/*   Updated: 2022/07/31 14:20:21 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SECTION_HPP
# define SECTION_HPP
# include "Utils.hpp"
# include "Exceptions.hpp"
# define VERIFIER_PARAMS  uint32_t lineNum,  std::string& key,  std::string& value

namespace ft {

// A KeyValueVerifier verifies that a key-value pair is set up correctly
class KeyValueVerifier
{
public: // Ctor ~ Dtor
	KeyValueVerifier() = default;
	~KeyValueVerifier() = default;

public: // Functions
	/**
	 * @brief Run a check against a key-value pair. Based on the key, a specific check will be run.
	 */
	void run(VERIFIER_PARAMS) ;


public: // Attributes
	typedef void (*parserFunc)(VERIFIER_PARAMS);

private: // Functions
	/**
	 * @brief Parse an integer (as a signed long) and check if it's > 0 and < INT_MAX
	 */
	void parseInt(VERIFIER_PARAMS) ;

	/**
	 * @brief Parse an unsigned short (as a signed long) and check if it's > 0 and < USHRT_MAX
	 */
	void parseUShort(VERIFIER_PARAMS) ;

	/**
	 * @brief Parse a non-empty string
	 */
	void parseNonEmptyString(VERIFIER_PARAMS) ;

	/**
	 * @brief Parse a boolean: should be a string saying "yes" or "no"
	 */
	void parseYesNo(VERIFIER_PARAMS) ;

	/**
	 * @brief Parse a redirection field. 1st param should be 301, 302, 307 or 308 as string, 2nd a full path to a file to redirect to
	 */
	void parseRedir(VERIFIER_PARAMS) ;

	/**
	 * @brief Parse a CGI bin field. 1st param should be a file extension, 2nd a path to an executable
	 */
	void parseCGIBin(VERIFIER_PARAMS) ;
};

}
#endif
