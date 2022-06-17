/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:36:35 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <exception>
# include "Common.hpp"

FT_BEGIN

// Simple strerror exception wrapper.
struct GenericErrnoExecption : public std::exception
{
	const char* what() const throw() override {
		return (strerror(errno));
	}
};

struct DelimiterNotFoundException : public std::exception
{
	const char* what() const throw() override {
		return ("Delimiter not found");
	}
};

struct ConfigParserSyntaxException : public std::exception
{
	const char* what() const throw() override {
		return ("Syntax error in config");
	}
};

struct UnknownSectionTypeException : public std::exception
{
	const char* what() const throw() override {
		return ("Unknown (sub)section type in config");
	}
};

struct InvalidSubSectionPosition : public std::exception
{
	const char* what() const throw() override {
		return ("Subsection is defined at an invalid position in config");
	}
};

struct UnknownFieldKeyException : public std::exception
{
	const char* what() const throw() override {
		return ("Unknown field key in config");
	}
};

struct NoServersException : public std::exception
{
	const char* what() const throw() override {
		return ("No servers defined in config");
	}
};

struct InvalidCharException : public std::exception
{
	const char* what() const throw() override {
		return ("Non-ascii character found");
	}
};

// File cannot be located.
struct FileNotFoundExecption : public std::exception	// TODO: change to more generic File could not be located or read exception, IOException?
{
	const char* what() const throw() override {
		return ("File could not be located");
	}
};

/**
 * Print the exception and exit the application.
 *
 * @param[in] Exception The exception to print.
 * @param[in] Code The exit code.
 */
[[noreturn]] void exceptionExit(const std::exception& e, int32_t code);

FT_END
#endif