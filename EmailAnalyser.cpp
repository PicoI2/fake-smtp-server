#include "EmailAnalyser.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#define SCRIPT_EXTENSION ".sh"
		
// On receiving an email
void CEmailAnalyser::NewEmail (const std::string& aNewEmail)
{
	std::cout << "Email received :" << std::endl;
	std::cout << aNewEmail << std::endl;

	// The large email string must be converted in a istringstream to be read line by line
	// (regex on multiple line is not working)
	std::istringstream EmailLines (aNewEmail);

	// Search for scripts in execution directory
	boost::filesystem::path p = "./";
    for (boost::filesystem::directory_entry& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
	{
		if (SCRIPT_EXTENSION == entry.path().extension())
		{
			std::cout << "Opening " << entry.path().string() << std::endl;
			std::ifstream ScriptFile(entry.path().string());
			std::string RegexLine;
			// Read the two first lines
			if (std::getline(ScriptFile, RegexLine) && std::getline(ScriptFile, RegexLine))
			{
				RegexLine.erase(0, 1); // Remove the first char (it should be a '#')
				std::cout << "The regex of this script is: '" << RegexLine << "'" << std::endl;

				// Catch exception (if regex is not valid)
				try
				{
					std::regex RegexFromScript(RegexLine);
					std::string EmailLine;

					// Try regex on each line from email
					while (std::getline(EmailLines, EmailLine))
					{
						if (std::regex_match(EmailLine, RegexFromScript))
						{
							// Start script
							std::cout << "Regex from script match." << std::endl;
							std::thread ([Script = entry.path().string()](){
								std::cout << "start : " << Script << std::endl;
								system(Script.c_str());
								std::cout << "end : " << Script << std::endl;
							}).detach();			
						}
						break;	// no need to read more
					}
				}
				catch (std::exception& e)
				{
					// Most commonly a regex error.
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}
}
