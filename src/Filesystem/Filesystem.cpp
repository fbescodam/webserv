/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Filesystem.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/28 12:31:21 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 19:50:26 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Filesystem.hpp"
#include <sys/stat.h>

bool ft::filesystem::fileExists(const std::string& path)
{
	std::ifstream fstream(path);

	return (fstream.good());
}

bool ft::filesystem::isDir(const std::string& path)
{
	struct stat stats;
	stat(path.c_str(), &stats);

	return (S_ISDIR(stats.st_mode));
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
