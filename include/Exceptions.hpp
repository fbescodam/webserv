/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/13 13:50:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/05/13 14:18:01 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//= All exceptions =//

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <exception>
# include <errno.h>
# include <string>

// Simplificationt typedef
typedef std::exception Exception;

namespace ft
{
    // Simple strerror exception wrapper.
    struct GenericErrnoExecption : public Exception
    {
        virtual const char* what() const throw() {
            return (strerror(errno));
        }
    };
}
#endif
