/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exchange.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/26 17:42:38 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/26 17:52:53 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef EXCHANGE_HPP
# define EXCHANGE_HPP
# include <map>
# include <string>

namespace ft
{

class Exchange
{
public: // Ctor ~ Dtor
	virtual ~Exchange() { }

    enum class Method : uint8_t
    {
        GET,
        POST,
        DELETE,
        MAX
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
    std::string body;

    // The either incoming or outgoing socket of the exchange.
    int32_t socketFD;
};

}
#endif
