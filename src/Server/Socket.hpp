/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:22:23 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/31 16:20:41 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SOCKET_HPP
# define SOCKET_HPP
# include "Utils.hpp"
# include "SocketAddress.hpp"

namespace ft {

struct Socket
{
	ft::SocketAddress	addr;	// The socket's address
	ft::fd_t			fd;		// The socket's file descriptor
};

}
#endif
