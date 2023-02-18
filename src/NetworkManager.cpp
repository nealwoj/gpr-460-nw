#include "RoboCatPCH.h"
#include "NetworkManager.h"
//#include "TCPSocket.h"
#include <iostream>

/// <summary>
/// Initialize listenSocket so it can receive incoming connections.
/// listenSocket must not block on any future calls.
/// </summary>
void NetworkManager::Init()
{
	listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	SocketAddressPtr sockAddr = SocketAddressFactory::CreateIPv4FromString("localhost" + listenSocket.get()->GetPortNumber());

	if (listenSocket.get()->Bind(*sockAddr.get()) == NOERROR)
		messageLog.AddMessage("Binded to " + sockAddr.get()->ToString());
	else
		messageLog.AddMessage("Failed to bind at " + sockAddr.get()->ToString());

	//blocking
	listenSocket.get()->SetNonBlockingMode(true);

	//listening
	listenSocket.get()->Listen();
}

/// <summary>
/// Called once per frame. Should check listenSocket for new connections.
/// If any come in, should add them to the list of openConnections, and
/// should send a message to each peer listing the addresses
/// of every other peer.
/// 
/// If there is a new connection, log some info about it with
/// messageLog.AddMessage().
/// </summary>
void NetworkManager::CheckForNewConnections()
{
	SocketAddress addr = *SocketAddressFactory::CreateIPv4FromString("127.0.0.1:0").get();
	TCPSocketPtr ptr = listenSocket.get()->Accept(addr);

	if (ptr != NULL)
	{
		messageLog.AddMessage("Accepted connection from " + addr.ToString());
		openConnections.emplace(addr, ptr);
	}
}

/// <summary>
/// Sends the provided message to every connected peer. Called
/// whenever the user presses enter.
/// </summary>
/// <param name="message">Message to send</param>
void NetworkManager::SendMessageToPeers(const std::string& message)
{
	std::unordered_map<SocketAddress, TCPSocketPtr>::iterator iter = openConnections.begin();
	while (iter != openConnections.end())
	{
		if (iter->second.get()->Send(message.c_str(), sizeof(message)) < 0 )
			messageLog.AddMessage("Failed to send message!");
	}
}

//PostMessagesFromPeers() is called once per fram
//Once implemented, it should try to receive messages from all connected peers
//If there is no data to receive, it should return without doing anything
//If there is data, it should post the message it received using the messageLog member variable
void NetworkManager::PostMessagesFromPeers()
{
	std::unordered_map<SocketAddress, TCPSocketPtr>::iterator iter = openConnections.begin();
	while (iter != openConnections.end())
	{
		char* data = new char;
		
		if (iter->second.get()->Receive(data, sizeof(data)) < 0)
			messageLog.AddMessage("Failed to receive data!");
		
		//messageLog.AddMessage(data);
		
		if (data != NULL)
		{
			string msg = static_cast<string>(data);
			messageLog.AddMessage(msg);
		}
		else
			messageLog.AddMessage("Received data was null!");
	}
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{
	TCPSocketPtr tcp = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	SocketAddressPtr sockAddr = SocketAddressFactory::CreateIPv4FromString("localhost" + listenSocket.get()->GetPortNumber());
	tcp.get()->Bind(*sockAddr.get());

	if (tcp.get()->Connect(*targetAddress.get()) == NOERROR)
	{
		messageLog.AddMessage("Connected to " + targetAddress.get()->ToString());
		openConnections.emplace(*targetAddress.get(), tcp);
	}
	else
		messageLog.AddMessage("Failed to connect to " + targetAddress.get()->ToString());
}
