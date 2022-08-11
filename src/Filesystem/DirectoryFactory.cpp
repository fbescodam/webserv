/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryFactory.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/12 16:02:40 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/11 16:28:07 by fbes          ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryFactory.hpp"

// Fuck macos 14
void ft::DirectoryFactory::buildContentFromDir(const std::string &actualPath, const std::string &requestedPath, std::string &outContent)
{
	outContent.clear();

	// Title and header
	outContent += "<html><head><title>Index of " + requestedPath + "</title></head>";
	outContent += "<body><h1>Index of " + requestedPath + "</h1><hr><pre>";

	DIR* dir;

	if ((dir = opendir(actualPath.c_str())) != NULL)
	{
		dirent* ent;
		while ((ent = readdir(dir)) != NULL)
		{
			const bool isDir = ent->d_type == DT_DIR;

			outContent += "<a href=\"";
			outContent += ent->d_name; // href where link points to
			if (isDir) outContent += "/";
			outContent += "\">";
			outContent += ent->d_name; // actual visible printed name
			if (isDir) outContent += "/";
			outContent += "</a>\n";
		}
		closedir(dir);
		outContent += "</pre><hr></body></html>";
		return;
	}
	throw ft::IOException();
}
