#include <iostream>
#include <boost/asio.hpp>

#include "TcpServer.h"

using boost::asio::ip::tcp;

// argv[0] must be listen port number
int main (int argc, char** argv)
{
	try
	{
		if (argc != 2)
		{
		    std::cerr << "Usage: " << argv[0] << " <port>\n";
		    return 1;
		}
		boost::asio::io_service io_service;
		
		// Start the server
		CTcpServer server(io_service, atoi(argv[1]));
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
			
	return 0;
}