/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/23 18:05:00 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/26 16:06:06 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Custom exception implementations to rely less on the garbage stdlib of C++

#pragma once

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include "Common.hpp"
FT_BEGIN

// An exception is a runtime error.
struct Exception
{
    Exception() noexcept { }
    virtual ~Exception() noexcept { };
	
    // What went wrong.
    virtual const char* what() const noexcept = 0;
};

// Simple strerror exception wrapper.
struct GenericErrnoExecption : public ft::Exception
{
    virtual const char* what() const throw() {
        return (strerror(errno));
    }
};

/**
 * Print the exception and exit the application.
 * 
 * @param[in] Exception The exception to print.
 * @param[in] Code The exit code.
 */
[[noreturn]] void exceptionExit(const ft::Exception& exception, int32_t code);

FT_END
#endif