/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerSection.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/13 17:30:53 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVERSECTION_HPP
# define SERVERSECTION_HPP
# include "Common.hpp"
# include "Section.hpp"
FT_BEGIN

class ServerSection final : public ft::Section
{
public: // Ctor ~ Dtor
	ServerSection(const std::string& name);
	~ServerSection() = default;

public: // Attributes

	// Server locations
	std::vector<ft::Section> locations;

	//TODO: que
	std::string root = "examples/www";
};

FT_END
#endif