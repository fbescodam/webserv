/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerEntry.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/01 15:47:44 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVERENTRY_HPP
# define SERVERENTRY_HPP
# include "Common.hpp"
# include "Entry.hpp"
FT_BEGIN

class ServerEntry final : public ft::Entry
{
public: // Attributes

	// Server locations
	std::vector<ft::Entry> Locations;
};

FT_END
#endif