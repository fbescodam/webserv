/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Entry.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/01 15:50:28 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef ENTRY_HPP
# define ENTRY_HPP
# include "Common.hpp"
FT_BEGIN

// An entry consistes of a name along with a map of key and values that can be retrieved.
class Entry
{
public: // Ctor ~ Dtor
	Entry() = default;
	~Entry() = default;

public: // Functions

	/**
	 * @brief 
	 * 
	 * @param key 
	 * @return true 
	 * @return false 
	 */
	bool keyExists(const std::string& key) const;

	/**
	 * @brief 
	 * 
	 * @param key 
	 * @return const std::string* 
	 */
	const std::string* getValue(const std::string& key) const;

	/**
	 * @brief 
	 * 
	 * @param key 
	 * @param value 
	 * @return true 
	 * @return false 
	 */
	void setValue(const std::string& key, const std::string& value);

public: // Attributes
	// Name of the tag entry.
	std::string entryName;

private:
	std::map<std::string, std::string> fields;
};

FT_END
#endif
