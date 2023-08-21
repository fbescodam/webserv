/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SocketAddress.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/27 11:22:23 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 21:12:24 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SOCKETADDR_HPP
# define SOCKETADDR_HPP
# include <cstdint>
# include <memory>
# include <netinet/in.h>

namespace ft {

// C Network function & type wrappers
//////////////////////////////////////////

// Socket address, internet style. Wrapper for struct sockaddr_in.
struct SocketAddress
{
public: // Ctor
	SocketAddress() = default;
	SocketAddress(uint8_t inFamily, uint16_t inPort, uint32_t inAddress)
	{
		addressFamily = inFamily;
		port = inPort;
		address = inAddress;
	}

public: // Functions

	// Converts the wrapper to the C Style type.
	struct sockaddr_in getCStyle(void) const
	{
		struct sockaddr_in out;
		// out.sin_len = length;
		out.sin_family = addressFamily;
		out.sin_port = port;
		out.sin_addr.s_addr = address;

		std::memset(out.sin_zero, 0, sizeof(out.sin_zero));
		return (out);
	}

	// Gets the size of the Socket Address.
	constexpr size_t getSize(void) const
	{
		return (sizeof(SocketAddress));
	}

public: // Attributes
	// uint8_t		length;
	uint8_t		addressFamily;
	uint16_t	port;
	uint32_t	address;
protected:
	char		zero[8]; // Literally useless ?
};
}
#endif
