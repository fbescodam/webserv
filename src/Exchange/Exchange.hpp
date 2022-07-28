/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/26 17:42:38 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/28 19:57:48 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCHANGE_HPP
# define EXCHANGE_HPP
# include <map>
# include <string>

namespace ft
{

// An exchange is an either incoming or outgoing http reqeuest.
class Exchange
{
public: // Ctor ~ Dtor
	virtual ~Exchange() { }

public: // Types
    enum class Method : uint8_t
    {
        GET,
        POST,
        DELETE,
        MAX
    };

    enum class Status : uint8_t
    {
        DONE,
        NOT_DONE,
    };

public: // Functions

	/**
	 * @brief Get the value of a header field.
	 *
	 * @param[in] key The key of a header field to get the value of.
	 * @return A pointer to the value, null if field does not exist.
	 */
	const std::string* getHeaderValue(const std::string& key) const;

public:

    // The exchange make-up and parameters.
    std::map<std::string, std::string> headers;

    // The raw data of the exchange method, e.g: file, text, script, ...
    std::string data;

    // The either incoming or outgoing socket of the exchange.
    int32_t socketFD;
};

}
#endif
