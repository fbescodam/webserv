/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 13:50:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/13 16:27:36 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Custom exception implementations to rely less on the garbage stdlib of C++

//= All exceptions =//

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <errno.h>
# include "Common.hpp"

namespace ft
{
    // An exception is a runtime error.
    struct Exception
    {
        Exception() noexcept {}
        virtual ~Exception() noexcept { };

        // What went wrong.
        virtual const char* What() const noexcept = 0;
    };

    // Simple strerror exception wrapper.
    struct GenericErrnoExecption : public ft::Exception
    {
        virtual const char* What() const throw() {
            return (strerror(errno));
        }
    };

    /**
     * Print the exception and exit the application.
     * 
     * @param Exception The exception to print.
     * @param Code The exit code.
     */
    [[noreturn]] void ExceptionExit(const ft::Exception& Exception, int32_t Code = EXIT_FAILURE)
    {
        std::cerr << "Webserv: " << Exception.What() << std::endl;
        exit(Code);
    }
}
#endif
