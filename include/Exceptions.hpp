/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/29 19:17:53 by fbes          ########   odam.nl         */
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
	ConfigParserSyntaxException(int32_t on) {
		this->err = ft::format("Syntax error on line %d of your configuration file", on);
	}
};

struct UnknownSectionTypeException : public ft::ConfigException
{
public:
	UnknownSectionTypeException(int32_t on) {
		this->err = ft::format("Unknown (sub)section type on line %d of your configuration file", on);
	}
};

struct InvalidSubSectionPosition : public ft::ConfigException
{
public:
	InvalidSubSectionPosition(int32_t on) {
		this->err = ft::format("Invalid position for subsection definition on line %d of your configuration file", on);
	}
};

struct UnknownFieldKeyException : public ft::ConfigException
{
public:
	UnknownFieldKeyException(int32_t on) {
		this->err = ft::format("Unknown field key on line %d of your configuration file", on);
	}
};

struct InvalidFieldValueException : public ft::ConfigException
{
public:
	InvalidFieldValueException(int32_t on) {
		this->err = ft::format("Invalid field value on line %d of your configuration file", on);
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

struct NoServersException : public std::exception
{
	const char* what() const throw() override {
		return ("No servers defined in config");
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
