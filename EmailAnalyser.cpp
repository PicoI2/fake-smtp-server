#include "EmailAnalyser.h"

#include <iostream>

// On receiving an email
void CEmailAnalyser::NewEmail (const std::string& aNewEmail)
{
	std::cout << "Email received :" << std::endl;
	std::cout << aNewEmail << std::endl;
	// TODO
}
