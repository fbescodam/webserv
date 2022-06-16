/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/16 21:08:54 by fbes          ########   odam.nl         */
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

struct UnknownSectionTypeException : public std::exception
{
	const char* what() const throw() override {
		return ("Unknown (sub)section type");
	}
};

struct UnknownFieldKeyException : public std::exception
{
	const char* what() const throw() override {
		return ("Unknown field key");
	}
};

struct InvalidCharException : public std::exception
{
	const char* what() const throw() override {
		return ("Non-ascii character found");
	}
};

// File cannot be located.
struct FileNotFoundExecption : public std::exception
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