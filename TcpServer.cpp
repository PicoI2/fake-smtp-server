#include "TcpServer.h"

// Constructor
CTcpServer::CTcpServer(boost::asio::io_service& aIoService, int aPort)
	: mIoService(aIoService)
	, mAcceptor(aIoService, tcp::endpoint(tcp::v4(), aPort))
{
	StartAccept();
}
			
// Start or restart listen on server socket
void CTcpServer::StartAccept()
{
	CTcpConnection::pointer NewConnection = CTcpConnection::pointer(new CTcpConnection (mIoService, mEmailAnalyser));
				
	mAcceptor.async_accept(NewConnection->socket(),
		[this, NewConnection] (auto error) {this->HandleAccept(NewConnection, error);}
	);

	std::cout << "Listening " << mAcceptor.local_endpoint().address().to_string() << ":" << mAcceptor.local_endpoint().port() << std::endl;
}

// Callback on new client connection			
void CTcpServer::HandleAccept(CTcpConnection::pointer aNewConnection, const boost::system::error_code& error)
{
	if (!error)
	{
		aNewConnection->Init();
		std::cout << "Connection of client: " << aNewConnection->ToString() << std::endl;
	}
	StartAccept();	// Accept next client
}