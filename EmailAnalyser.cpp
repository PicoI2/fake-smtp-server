#include "EmailAnalyser.h"
#include "Base64.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <thread>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#define SCRIPT_EXTENSION ".sh"
		
// On receiving an email
void CEmailAnalyser::NewEmail (const std::string& aNewEmail)
{
	std::string NewEmailWithoutAttachments;
	std::list<std::string> Attachments;
	GetAttachments(aNewEmail, NewEmailWithoutAttachments, Attachments);

	std::cout << "Email received :" << std::endl;
	std::cout << NewEmailWithoutAttachments << std::endl;

	if (Attachments.size())
	{
		std::cout << "Attachments:" << std::endl;
		for (std::string Attachment : Attachments)
		{
			std::cout << Attachment << std::endl;
		}
	}

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
						if (std::regex_search(EmailLine, RegexFromScript))
						{
							// Start script
							std::cout << "Regex from script match." << std::endl;
							std::string Command = entry.path().string();
							for (std::string Attachment : Attachments)
							{
								Command.append(" \"" + Attachment + "\"");
							}

							std::thread ([Command](){
								std::cout << "start : " << Command << std::endl;
								system(Command.c_str());
								std::cout << "end : " << Command << std::endl;
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

// Search for attachment
void CEmailAnalyser::GetAttachments (const std::string& aNewEmail, std::string& aNewEmailWithoutAttachments, std::list<std::string>& aAttachments)
{
	// The large email string must be converted in a istringstream to be read line by line
	std::istringstream EmailLines (aNewEmail);
	std::string EmailLine;
	std::ofstream AttachmentFile;
	
	// Try regex on each line from email
	while (std::getline(EmailLines, EmailLine))
	{
		if (AttachmentFile.is_open())
		{
			if (std::string::npos != EmailLine.find(' '))
			{
				// Drop blank lines
			}
			else if (std::string::npos != EmailLine.find('-'))
			{
				AttachmentFile.close();
			}
			else
			{
			   AttachmentFile << base64_decode(EmailLine);
			}
		}
		else 
		{
			if (boost::iequals(EmailLine, "Content-Disposition: attachment;"))
			{
				std::getline(EmailLines, EmailLine);
				std::regex FilenameRegex("filename=\"([^/\\:*?\"<>|]*)\"");	// chars / \ : * ? " < > | are forbids in filename
				std::smatch matches;
				if (std::regex_search(EmailLine, matches, FilenameRegex) && 1 < matches.size() )
				{
					std::string Filename = std::string("attachments/") + std::string(matches[1]);
					AttachmentFile.open (Filename, std::ofstream::binary);
					if (AttachmentFile.is_open())
					{
						aAttachments.push_back(matches[1]);
					}
				}
			}
			aNewEmailWithoutAttachments.append(EmailLine + "\n");
		}
	}

	if (AttachmentFile.is_open())
	{
		AttachmentFile.close();
	}
}
