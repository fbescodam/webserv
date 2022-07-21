/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryFactory.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/12 16:02:40 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/21 17:57:24 by pvan-dij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryFactory.hpp"

// Fuck macos 14
void ft::DirectoryFactory::buildContentFromDir(const std::string &path, std::string &outContent)
{
	outContent.clear();

	// Title and header
	outContent += "<html><head><title>Index of " + path + "</title></head>";
	outContent += "<body><h1>Index</h1><hr><pre>";

	DIR* dir;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		dirent* ent;
		while ((ent = readdir(dir)) != NULL)
		{
			bool isDir = ent->d_type == DT_DIR;

			outContent += "<a href=\"";
			outContent += ent->d_name; // href where link points to
			if (ent->d_type == DT_DIR)
				outContent += "/";
			outContent += "\">";
			outContent += ent->d_name; // actual visible printed name
			if (ent->d_type == DT_DIR)
				outContent += "/";
			outContent += "</a>\n";
		}
		closedir(dir);
	}
	else throw ft::IOException();

	outContent += "</pre><hr></body></html>";
}
