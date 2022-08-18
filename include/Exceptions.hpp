/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/18 16:34:14 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <exception>
# include <string>
# include <errno.h>

namespace ft {

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
		this->err = std::string("Syntax error on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct UnknownSectionTypeException : public ft::ConfigException
{
public:
	UnknownSectionTypeException(int32_t on) {
		this->err = std::string("Unknown (sub)section type on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct InvalidSubSectionPosition : public ft::ConfigException
{
public:
	InvalidSubSectionPosition(int32_t on) {
		this->err = std::string("Invalid position for subsection definition on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct NoSubSectionLocation : public ft::ConfigException
{
public:
	NoSubSectionLocation(int32_t on) {
		this->err = std::string("No path defined for a location on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct DuplicateSubSectionLocation : public ft::ConfigException
{
public:
	DuplicateSubSectionLocation(int32_t on) {
		this->err = std::string("Duplicate path for a location definition on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct UnknownFieldKeyException : public ft::ConfigException
{
public:
	UnknownFieldKeyException(int32_t on) {
		this->err = std::string("Unknown field key on line ") + std::to_string(on);
	}
};

struct InvalidFieldValueException : public ft::ConfigException
{
public:
	InvalidFieldValueException(int32_t on) {
		this->err = std::string("Invalid field value on line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct EmptySectionException : public ft::ConfigException
{
public:
	EmptySectionException(int32_t on) {
		this->err = std::string("Empty section found before line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct MissingFieldException : public ft::ConfigException
{
public:
	MissingFieldException(int32_t on, std::string fieldKey, std::string sectionName) {
		this->err = std::string("Missing field ") + fieldKey + std::string(" in section ") + sectionName + std::string(" before line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct ForbiddenFieldException : public ft::ConfigException
{
public:
	ForbiddenFieldException(int32_t on, std::string fieldKey, std::string sectionName) {
		this->err = std::string("Field ") + fieldKey + std::string(" cannot be used in section ") + sectionName + std::string(" before line ") + std::to_string(on) + std::string(" of your configuration file");
	}
};

struct BadRequest : public std::exception
{
	const char* what() const throw() override {
		return ("Bad request");
	}
};

struct HTTPInvalid : public std::exception
{
	const char* what() const throw() override {
		return ("Invalid http version");
	}
};

struct NotImplemented : public std::exception
{
	const char* what() const throw() override {
		return ("Not implemented");
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
		return ("IO Error on file. Is it actually a file?");
	}
};
}
#endif
