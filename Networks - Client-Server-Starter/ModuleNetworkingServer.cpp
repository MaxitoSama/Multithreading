#include "ModuleNetworkingServer.h"

//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	int ret = 0;
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		reportError("Server Error Creating the Socket");
	}

	// - Set address reuse
	int enable = 1;
	ret = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (ret == SOCKET_ERROR)
	{
		reportError("Server Error Enabling the Socket");
	}

	// - Bind the socket to a local interface
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	ret = bind(listenSocket, (const sockaddr*)&bindAddr, sizeof(bindAddr));
	if (ret == SOCKET_ERROR)
	{
		reportError("Server Error Binding the Socket");
	}
	   
	// - Enter in listen mode
	ret = listen(listenSocket, 5);
	if (ret == SOCKET_ERROR)
	{
		reportError("Server Error Listening");
	}
	else
	{
		// - Add the listenSocket to the managed list of sockets using addSocket()
		addSocket(listenSocket);

		state = ServerState::Listening;
	}

	return true;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		if (ImGui::Button("Close Server"))
		{
			disconnect();
			state = ServerState::Stopped;
		}

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}

		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream &packet)
{
	// Set the player name of the corresponding connected socket proxy         --------------> Updated
	/*for (auto &connectedSocket : connectedSockets)
	{
		if (connectedSocket.socket == socket)
		{
			connectedSocket.playerName = (const char *)data;
		}
	}*/
	
	ClientMessage clientMessage;
	packet >> clientMessage;

	if (clientMessage == ClientMessage::Hello)
	{
		std::string playername;
		packet >> playername;

		bool notify = false;

		for (int i = 0; i < connectedSockets.size(); ++i)
		{
			if (connectedSockets[i].socket == socket)
			{
				OutputMemoryStream _packet;
				std::string welcom_message;

				if (checkUserName(playername))
				{
					connectedSockets[i].playerName = playername;
					welcom_message = "Welcome To The best server!";
					_packet << ServerMessage::Welcome;
					notify = true;
				}
				else
				{
					connectedSockets[i].playerName = playername;
					welcom_message = "Your name is already used! \n Please logout!";
					_packet << ServerMessage::Unwelcome;
				}
								
				_packet << welcom_message;

				int ret = sendPacket(_packet, socket);
				if (ret == SOCKET_ERROR)
				{
					reportError("Error Sending Welcom Packet");
				}
			}
		}
		if (notify)
		{
			for (int i = 0; i < connectedSockets.size(); ++i)
			{
				if (connectedSockets[i].socket != socket)
				{
					OutputMemoryStream _packet;
					_packet << ServerMessage::Newuser;

					std::string newuser_message = playername + " joined";
					_packet << newuser_message;

					int ret = sendPacket(_packet, connectedSockets[i].socket);
					if (ret == SOCKET_ERROR)
					{
						reportError("Error Sending Welcom Packet");
					}
				}
			}
		}
	}

	if (clientMessage == ClientMessage::Send)
	{
		std::string playername;

		for (int i = 0; i < connectedSockets.size(); ++i)
		{
			if (connectedSockets[i].socket == socket)
			{
				playername = connectedSockets[i].playerName;
			}
		}

		std::string message;
		packet >> message;

		OutputMemoryStream _packet;
		_packet << ServerMessage::Newmessage;

		std::string text = playername + ": " + message;
		_packet << text;
		
		for (int i = 0; i < connectedSockets.size(); ++i)
		{
			int ret = sendPacket(_packet, connectedSockets[i].socket);

			if (ret == SOCKET_ERROR)
			{
				reportError("Error Sending Welcom Packet");
			}
		}
	}

	if (clientMessage == ClientMessage::Command)
	{
		for (int i = 0; i < connectedSockets.size(); ++i)
		{
			if (connectedSockets[i].socket == socket)
			{
				std::string command;
				packet >> command;

				executeCommand(command, connectedSockets[i].socket);
			}
		}
	}
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	std::string playername;

	// Remove the connected socket from the list
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			playername = connectedSocket.playerName;
			connectedSockets.erase(it);
			break;
		}
	}

	OutputMemoryStream _packet;
	_packet << ServerMessage::Userdisconnected;

	std::string text = playername + " left";
	_packet << text;

	for (int i = 0; i < connectedSockets.size(); ++i)
	{
		int ret = sendPacket(_packet, connectedSockets[i].socket);

		if (ret == SOCKET_ERROR)
		{
			reportError("Error Sending Welcom Packet");
		}
	}
}

bool ModuleNetworkingServer::checkUserName(std::string name)
{
	for (int i = 0; i < connectedSockets.size(); ++i)
	{
		if (name == connectedSockets[i].playerName)
		{
			return false;
		}
	}
	return true;
}

void ModuleNetworkingServer::executeCommand(std::string command, SOCKET socket)
{
	if (command.find("help")!= std::string::npos)
	{
		OutputMemoryStream _packet;
		_packet << ServerMessage::Command;

		std::string newuser_message = "Command list:\n /help \n /list \n /kick [name] \n /whisper [name] \n /change_name [name]";
		_packet << newuser_message;

		int ret = sendPacket(_packet, socket);
		if (ret == SOCKET_ERROR)
		{
			reportError("Error Sending Welcom Packet");
		}
	}
	else if (command.find("list") != std::string::npos)
	{
		OutputMemoryStream _packet;
		_packet << ServerMessage::Command;
		
		std::string newuser_message = "Users connected: \n";
		
		for (int i = 0; i < connectedSockets.size(); i++)
		{
			newuser_message = newuser_message + "- "+connectedSockets[i].playerName;
			if (i < connectedSockets.size() - 1)
			{
				newuser_message = newuser_message + "\n";
			}
		}
		
		_packet << newuser_message;

		int ret = sendPacket(_packet, socket);
		if (ret == SOCKET_ERROR)
		{
			reportError("Error Sending Welcom Packet");
		}
	}
	else if (command.find("kick") != std::string::npos)
	{
		for (int i = 0; i < connectedSockets.size(); i++)
		{
			if (command.find(connectedSockets[i].playerName) != std::string::npos)
			{
				OutputMemoryStream _packet;
				_packet << ServerMessage::ComDisconnect;

				int ret = sendPacket(_packet, connectedSockets[i].socket);
				if (ret == SOCKET_ERROR)
				{
					reportError("Error Sending Welcom Packet");
				}
			}
		}
	}
	else if (command.find("whisper") != std::string::npos)
	{
		std::string remove_1 = "/whisper ";
		command.erase(0,remove_1.size());

		for (int i = 0; i < connectedSockets.size(); i++)
		{
			if (command.find(connectedSockets[i].playerName) != std::string::npos)
			{
				std::string remove_2 = connectedSockets[i].playerName+" ";
				command.erase(0, remove_2.size());

				OutputMemoryStream _packet;
				_packet << ServerMessage::Newmessage;
				_packet << command;

				int ret = sendPacket(_packet, connectedSockets[i].socket);
				if (ret == SOCKET_ERROR)
				{
					reportError("Error Sending Welcom Packet");
				}
			}
		}
	}
	else if (command.find("change_name"))
	{
		std::string remove_1 = "/change_name ";
		command.erase(0, remove_1.size());

		for (int i = 0; i < connectedSockets.size(); i++)
		{
			if (connectedSockets[i].socket == socket)
			{
				connectedSockets[i].playerName = command;

				OutputMemoryStream _packet;
				_packet << ServerMessage::NewName;
				_packet << command;

				int ret = sendPacket(_packet, connectedSockets[i].socket);
				if (ret == SOCKET_ERROR)
				{
					reportError("Error Sending Welcom Packet");
				}
			}
		}

	}
}

