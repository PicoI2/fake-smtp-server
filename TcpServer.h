#include <iostream>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "EmailAnalyser.h"
#include "TcpConnection.h"

using boost::asio::ip::tcp;

class CTcpServer
{
public:
	CTcpServer(boost::asio::io_service& aIoService, int aPort);

private:
	void StartAccept();
	void HandleAccept(CTcpConnection::pointer aNewConnection, const boost::system::error_code& error);

// Members values
	boost::asio::io_service& mIoService;
	tcp::acceptor mAcceptor;
	CEmailAnalyser mEmailAnalyser;
};
