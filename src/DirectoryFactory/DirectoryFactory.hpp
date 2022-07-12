/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryFactory.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/12 16:02:40 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/12 17:27:46 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#ifndef DIRECTORY_FACTORY_HPP
# define DIRECTORY_FACTORY_HPP
# include "Common.hpp"
# include "Exceptions.hpp"
# include <dirent.h>
FT_BEGIN

namespace DirectoryFactory
{
	/**
	 * @brief 
	 * 
	 * @param path 
	 * @param outContent 
	 */
	void buildContentFromDir(const std::string& path, std::string& outContent);
}

FT_END
#endif