/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 10:13:49 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/16 21:08:55 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CGI_HPP
# define CGI_HPP
# include <strstream>
# include "CommonUnix.hpp"
# include "Request.hpp"
# include "Response.hpp"
FT_BEGIN

/**
 * Static class that is responsible of executing incoming requests
 * and process an outgoing response.
 *
 * The CGI checks if the given input file is on any of the matching files
 * and applies the appropriate action on that file, say you give it a php
 * file it will check what to do with that file and applies a action on it
 */
namespace CGI
{
	/**
	 * @brief
	 *
	 * @param request
	 * @param path
	 */
	ft::Response runCGI(const ft::Request& request, const std::string& path);

}

FT_END
#endif
