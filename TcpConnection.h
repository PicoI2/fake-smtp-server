#pragma once

#include <iostream>
#include <memory>	// enable_shared_from_this
#include <array>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "EmailAnalyser.h"

using boost::asio::ip::tcp;

class CTcpConnection : public std::enable_shared_from_this<CTcpConnection>
{
public:
	typedef std::shared_ptr<CTcpConnection> pointer;

	// Constructor
	CTcpConnection(boost::asio::io_service& aIoService, CEmailAnalyser& aEmailAnalyser);

	void Send (std::string aMessage);
	void Init ();

	tcp::socket& socket() {return mSocket;}
	const std::string ToString() {return mClientAsString;}

private:	
	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);
	void StartListening ();
	void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);
	void CloseConnection();
	void ResetTimeout ();

private:
	boost::signals2::signal < void (const std::string&) > NewMail;	// Signal to send mail to EmailAnalyser
	boost::asio::deadline_timer mTimer;	// Timeout
	tcp::socket mSocket;
	std::array<char, 1024> mBuffer;
	std::string mClientAsString;	// somthing like 127.0.0.1:45677
	bool mbDataSeen = false;	// "DATA" has been received from client
	std::string mEmail;		// Complete email
	boost::signals2::connection mConnection;
};
