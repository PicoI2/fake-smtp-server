#pragma once

#include <list>
#include <string>

// Analyse emails and execute associated script
class CEmailAnalyser
{
public:
	void NewEmail (const std::string& aNewEmail);
private:
	void GetAttachments (const std::string& aNewEmail, std::string& aNewEmailWithoutAttachments, std::list<std::string>& aAttachments);
};