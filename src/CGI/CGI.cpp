/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 11:51:45 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/02 12:34:39 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//////////////////////////////////////////

ft::Response ft::CGI::getResponse(const ft::Request& request)
{
	// If request is an image, response is the image data
	// with correct header
	
	// If request is a script that is also allowed to execute under the
	// config file, we call exec on it, and store the output which is the
	// response header

	return ft::Response();
}

//////////////////////////////////////////
