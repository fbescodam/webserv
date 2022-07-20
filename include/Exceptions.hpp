/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/20 20:23:40 by pvan-dij      ########   odam.nl         */
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

struct NoSubSectionLocation : public ft::ConfigException
{
public:
	NoSubSectionLocation(int32_t on) {
		this->err = ft::format("No path defined for a location on line %d of your configuration file", on);
	}
};

struct DuplicateSubSectionLocation : public ft::ConfigException
{
public:
	DuplicateSubSectionLocation(int32_t on) {
		this->err = ft::format("Duplicate path for a location definition on line %d of your configuration file", on);
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

struct EmptySectionException : public ft::ConfigException
{
public:
	EmptySectionException(int32_t on) {
		this->err = ft::format("Empty section found before line %d of your configuration file", on);
	}
};

struct MissingFieldException : public ft::ConfigException
{
public:
	MissingFieldException(int32_t on) {
		this->err = ft::format("Missing field found in config before line %d of your configuration file (every server needs a path, server_names and listen field)", on);
	}
};

struct BadRequest : public std::exception
{
	const char* what() const throw() override {
		return ("Bad request");
	}
};

struct PayloadTooLarge : public std::exception
{
	const char* what() const throw() override {
		return ("Payload too large");
	}
};

struct NoServersException : public std::exception
{
	const char* what() const throw() override {
		return ("No servers defined in config");
	}
};

struct IOException : public std::exception
{
	const char* what() const throw() override {
		return ("IO Error on file. Is it actually a file? And is it reabable? Who knows...");
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
