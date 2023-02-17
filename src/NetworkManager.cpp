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
	//SocketAddressPtr sock = SocketAddressFactory::CreateIPv4FromString(INADDR_ANY);
	SocketAddress addr = SocketAddress();

	listenSocket.get()->Bind(addr);
	listenSocket.get()->SetNonBlockingMode(true);
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
	listenSocket.get()->Listen();

	SocketAddress addr = SocketAddress();
	TCPSocketPtr ptr = listenSocket.get()->Accept(addr);

	if (ptr != NULL)
		openConnections.emplace(addr, ptr);
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
		iter->second.get()->Send(message.c_str(), sizeof(message));
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
		iter->second.get()->Receive(data, sizeof(data));
		messageLog.AddMessage(data);
	}
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{
	listenSocket.get()->Connect(*targetAddress.get());
}
