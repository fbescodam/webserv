/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/17 03:47:57 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SECTION_HPP
# define SECTION_HPP
# include "Common.hpp"
FT_BEGIN

// A section consists of a name along with a map of key-value pairs (called fields) that can be retrieved.
class Section
{
public: // Ctor ~ Dtor
	Section(const std::string& name);
	~Section() = default;

public: // Functions

	/**
	 * @brief check if a key exists in the configuration
	 *
	 * @param key the key to check for
	 * @return true if the key exists
	 */
	bool keyExists(const std::string& key) const;

	/**
	 * @brief get the value of a configuration field
	 *
	 * @param key the key of a configuration field to get the value of
	 * @return the value
	 */
	const std::string* getValue(const std::string& key) const;

	/**
	 * @brief get the value of a configuration field, as an integer
	 * @warning can throw whatever std::stoi decides to throw at you
	 *
	 * @param key the key of a configuration field to get the value of
	 * @param output a reference to where you want the integer value of the configuration field
	 * @return true if the key exists, false if it does not (if false, output will not be modified)
	 */
	bool getValueAsInt(const std::string& key, int& output) const;

	/**
	 * @brief get the value of a configuration field, as a string array
	 *
	 * @param key the key of a configuration field to get the value of
	 * @param list a reference to the list where you want the output to be stored. This list always gets cleared.
	 * @return true if the key exists, false if it does not
	 */
	bool getValueAsList(const std::string& key, std::list<std::string>& list) const;

	/**
	 * @brief configure a configuration field
	 *
	 * @param key the key of a configuration field to set
	 * @param value the value to set the field to
	 */
	void setValue(const std::string& key, const std::string& value);

	/**
	 * @brief get the name of this configuration section
	 *
	 * @return the name
	 */
	const std::string& getName() const;

	/**
	 * @brief get the amount of fields defined in this section
	 *
	 */
	uint32_t getAmountOfFields() const;

	/**
	 * @brief used for debugging: print an entire section
	 *
	 * @param prefix a prefix prepended to every printed line
	 */
	void print(std::string prefix) const;

public: // Attributes

private:
	// Name of the section.
	std::string name;

	// Configurations in a key-value pair
	std::map<std::string, std::string> fields;
};

FT_END
#endif
