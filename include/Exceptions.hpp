/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/27 21:01:59 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <exception>
# include "Common.hpp"

FT_BEGIN

// Simple strerror exception wrapper.
struct GenericErrnoException : public std::exception
{
	const char* what() const throw() override {
		return (strerror(errno));
	}
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Exception used for config parsing, lets you define a prefix error and a line number.
 */
struct ConfigException : public std::exception
{
	
public:
	ConfigException() = default;

public:
	const char* what() const throw() override {
		return (err.c_str());
	}

protected:
	std::string err;
};

struct DelimiterNotFoundException : public std::exception
{
	const char* what() const throw() override {
		return ("Delimiter not found");
	}
};

struct ConfigParserSyntaxException : public ft::ConfigException
{
public:
	ConfigParserSyntaxException(int32_t on)
	{
		this->err = ft::format("Error on line %d", on);
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

struct InvalidFieldValueException : public std::exception
{
	const char* what() const throw() override {
		return ("Invalid field value in config");
	}
};

struct NoServersException : public std::exception
{
	const char* what() const throw() override {
		return ("No servers defined in config");
	}
};

struct EmptySectionException : public std::exception
{
	const char* what() const throw() override {
		return ("Empty section found in config");
	}
};

struct InvalidCharException : public std::exception
{
	const char* what() const throw() override {
		return ("Non-ascii character found");
	}
};

// File cannot be located.
struct FileNotFoundException : public std::exception	// TODO: change to more generic File could not be located or read exception, IOException?
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