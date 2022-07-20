/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 10:13:49 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/20 19:25:46 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CGI_HPP
# define CGI_HPP
# include "Request.hpp"
# include "Response.hpp"
# include "CommonUnix.hpp"
# include <strstream>
# include <unordered_map>
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
	 * Executes whatever the input file is.
	 * 
	 * @param request The incoming request.
	 * @param path The file path to execute.
	 */
	bool runCGI(const ft::Response& request, const std::string& path, std::string &out, const std::string &cgiBin);
}

FT_END
#endif
