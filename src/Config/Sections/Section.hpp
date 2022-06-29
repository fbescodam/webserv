/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Section.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 13:54:52 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/06/29 20:11:30 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SECTION_HPP
# define SECTION_HPP
# include "Common.hpp"
# include "Exceptions.hpp"
FT_BEGIN

// A section consists of a name along with a map of key-value pairs (called fields) that can be retrieved.
class Section
{
public: // Ctor ~ Dtor
	Section() = default;
	Section(const std::string& cwd, const std::string& name);
	Section(const char* cwd, const std::string& name);
	Section(const std::string&, const std::string& name, const std::string& appliesToPath);
	Section(const std::string& cwd, const std::string& name, Section& inherit);
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
	 * @return a pointer to the value (NULL if field does not exist)
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
	 * @brief get the value of a configuration field, as an integer
	 * @warning can throw whatever std::stoi decides to throw at you
	 *
	 * @param key the key of a configuration field to get the value of
	 * @return the direct value of the configuration
	 */
	int returnValueAsInt(const std::string& key) const;

	/**
	 * @brief get the value of a configuration field, as a string array
	 *
	 * @param key the key of a configuration field to get the value of
	 * @param list a reference to the list where you want the output to be stored. This list always gets cleared.
	 * @return true if the key exists, false if it does not
	 */
	bool getValueAsList(const std::string& key, std::list<std::string>& list) const;

	/**
	 * @brief set the value of a configuration field
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
	 * @brief check if this section applies to the requested path
	 *
	 * @return the name
	 */
	bool appliesForPath(const std::string& requestedPath) const;

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

	/**
	 * @brief checks if a value is acceptable for the use with a certain key. Throws an error if something's wrong.
	 *
	 * @param lineNum the line number in the config file where the field is defined
	 * @param key a key
	 * @param value a value
	 */
	void verifyKeyValue(uint32_t lineNum, std::string& key, std::string& value) const;

	std::map<std::string, std::string> exportFields(void) const;
	void importFields(std::map<std::string, std::string> fields);

	/**
	 * @brief get the current working directory for this section
	 *
	 */
	const std::string& getcwd() const;


public: // Attributes

private:
	// Name of the section.
	std::string name;

	// Path this section applies to (only used for .location)
	std::string appliesToPath;

	// Configurations in a key-value pair
	std::map<std::string, std::string> fields;

	// Current working directory, used for appending paths in configs in subsections
	std::string cwd;

};

FT_END
#endif
