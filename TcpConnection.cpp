#include "TcpConnection.h"

const long TIMEOUT = 60; // seconds
static constexpr size_t MAX_EMAIL_SIZE = 1024*1024; // 1 Mo

// Constructor
CTcpConnection::CTcpConnection(boost::asio::io_service& aIoService, CEmailAnalyser& aEmailAnalyser) 
	: mSocket(aIoService)
	, mTimer(aIoService, boost::posix_time::seconds(TIMEOUT))	// Start timer at construction
{
	mConnection = NewMail.connect(
		[&aEmailAnalyser] (auto aMessage) {aEmailAnalyser.NewEmail(aMessage);}
	);
}

// Init connection (start listening)
void CTcpConnection::Init ()
{
	mClientAsString = mSocket.remote_endpoint().address().to_string() + ":" + std::to_string(mSocket.remote_endpoint().port());

	// Reset timeout
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
		else
		{
			Send("250 Ok\r\n");
		}
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
	mTimer.async_wait(	// calling async_wait cancel previous async_wait
		[self = shared_from_this()] (auto error_code)
		{
			std::cout << "Connection timeout" << std::endl;
			self->CloseConnection();
		}
	);
}

// Close connection
void CTcpConnection::CloseConnection()
{
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
