/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryFactory.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/12 16:02:40 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/08/09 15:32:16 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryFactory.hpp"

// Fuck macos 14
void ft::DirectoryFactory::buildContentFromDir(const std::string &path, std::string &outContent)
{
	outContent.clear();

	// Title and header
	outContent += "<html><head><title>Index of " + path + "</title></head>";
	outContent += "<body><h1>Index of " + path + "</h1><hr><pre>";

	DIR* dir;

	if ((dir = opendir(path.c_str())) != NULL)
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
