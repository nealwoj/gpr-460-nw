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
	SocketAddress sockAddr = SocketAddress();
	//need to create a TCPSocketPtr/TCPSocket
	listenSocket.get()->Bind(sockAddr);
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
}

/// <summary>
/// Sends the provided message to every connected peer. Called
/// whenever the user presses enter.
/// </summary>
/// <param name="message">Message to send</param>
void NetworkManager::SendMessageToPeers(const std::string& message)
{
	for (int i = 0; i < openConnections.size(); i++)
	{
		listenSocket.get()->Send(message.c_str(), sizeof(message));
	}
}

void NetworkManager::PostMessagesFromPeers()
{
	/*if (listenSocket.get()->Receive())
	{
		messageLog.AddMessage();
	}*/
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{
	SocketAddress* addr = targetAddress.get();
	listenSocket.get()->Connect(*addr);
}
