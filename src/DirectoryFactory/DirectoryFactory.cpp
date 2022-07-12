/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryFactory.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: lde-la-h <lde-la-h@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/12 16:02:40 by lde-la-h      #+#    #+#                 */
/*   Updated: 2022/07/12 20:29:10 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryFactory.hpp"

// Fuck macos 14
void ft::DirectoryFactory::buildContentFromDir(const std::string &path, std::string &outContent)
{
	outContent.clear();
	
	// Title and header
	outContent += "<html><head><title>Balls</title></head>";
	outContent += "<body><h1>Index</h1><hr><pre>";

	DIR* dir;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		dirent* ent;
		while ((ent = readdir(dir)) != NULL)
		{
			bool isDir = ent->d_type == DT_DIR;

			outContent += "<a href=\"";
			outContent += ent->d_name;
			outContent += "\">";
			outContent += ent->d_name;
			outContent += "</a>\n";
		}
		closedir(dir);
	}
	else throw ft::FileNotFoundException();

	outContent += "</pre><hr></body></html>";
}