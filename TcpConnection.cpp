#include <boost/algorithm/string.hpp>
#include "TcpConnection.h"

const long TIMEOUT = 60; // seconds

// Constructor
CTcpConnection::CTcpConnection(boost::asio::io_service& aIoService, CEmailAnalyser& aEmailAnalyser) 
	: mSocket(aIoService)
	, mTimer(aIoService)
{
	mConnection = NewMail.connect(
		[&aEmailAnalyser] (const std::string& aMessage) {aEmailAnalyser.NewEmail(aMessage);}
	);
}

// Init connection (start listening)
void CTcpConnection::Init ()
{
	mClientAsString = mSocket.remote_endpoint().address().to_string() + ":" + std::to_string(mSocket.remote_endpoint().port());

	// Configure timeout
	ResetTimeout();
	
	Send("220 SMTP Ready\r\n");	// Service ready

	StartListening();
}

// Start listening from client
void CTcpConnection::StartListening ()
{
	// Start listening for message.
	mSocket.async_read_some(boost::asio::buffer(mBuffer), 
		[self = shared_from_this()]	(auto error, size_t bytes_transferred){self->HandleRead(error, bytes_transferred);}
	);

}

// Callback on data received from client
void CTcpConnection::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
	bool bCloseConnexion = error;
	if (!error)
	{
		// Relauch timeout timer
		ResetTimeout();

		std::string message(mBuffer.begin(), mBuffer.begin()+bytes_transferred);

		// Display received message
		// std::cout << "\033[1;36m" << message << "\033[0m";

		// Append to email
		mEmail.append(message);

        boost::algorithm::to_upper(message);
		// Limit email size
		if (MAX_EMAIL_SIZE < mEmail.size())
		{
			bCloseConnexion = true;
		}
		else if (!mbDataSeen && std::string::npos != message.find("DATA"))
		{
			mbDataSeen = true;
			Send("354 Enter mail, end with \".\" on a line by itself\r\n");
		}
		else if (std::string::npos != message.find("QUIT"))
		{
			bCloseConnexion = true;
		}
		else if (!mbDataSeen || std::string::npos != message.find("\r\n.\r\n") || std::string::npos != message.rfind(".\r\n", 0))
		{
			Send("250 Ok\r\n");
		}
	}

	if (error)
	{
		// std::cout << "ERROR: " << error.message() << std::endl;
	}

	if (bCloseConnexion)
	{
		Send("221 Closing connection\r\n");
		CloseConnection();
	}
	else
	{
		// Start listening for message again
		StartListening();
	}
}

// Send message to client
void CTcpConnection::Send (std::string aMessage)
{
    // std::cout << "\033[1;35m" << aMessage << "\033[0m";
	if (mSocket.is_open())
	{
		// Uncomment to see server response
		// cout << "\033[1;31m" << aMessage << "\033[0m";
		boost::asio::async_write(mSocket, boost::asio::buffer(aMessage),
			[self = shared_from_this()] (auto error, auto bytes_transferred) {self->HandleWrite(error, bytes_transferred);}
		);
	}
}

// Callback at each write on socket
void CTcpConnection::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		std::cout << error.message() << std::endl;
		CloseConnection();
	}
}

// Reset timeout to initial value
void CTcpConnection::ResetTimeout ()
{
	mTimer.expires_from_now(boost::posix_time::seconds(TIMEOUT));
	// Changing expiration time cancel the timer, so we have to start a new asynchronous wait
	mTimer.async_wait(
		[self = shared_from_this()] (const boost::system::error_code& error_code)
		{
			if (boost::asio::error::operation_aborted != error_code)
			{
				std::cout << "Connection timeout" << std::endl;
				self->CloseConnection();
			}
		}
	);
}

// Close connection
void CTcpConnection::CloseConnection()
{
	mTimer.cancel();
	if (mSocket.is_open())
	{
		std::cout << "Close client connection with " << mClientAsString << std::endl;
		mSocket.close();
	}
	if (mEmail.size())
	{
		NewMail(mEmail);
	}
	mConnection.disconnect();
}
