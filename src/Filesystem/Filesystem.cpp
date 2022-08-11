/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Filesystem.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 12:31:21 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 14:46:37 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Filesystem.hpp"

bool ft::filesystem::fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

size_t ft::filesystem::getFileSize(std::ifstream& f)
{
	f.seekg(0, f.end);
	size_t length = f.tellg();
	f.seekg(0, f.beg);

	return (length);
}

size_t ft::filesystem::getFileSize(FILE *f)
{
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	return (size);
}
