#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	int ret = 0;
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (socketClient == INVALID_SOCKET)
	{
		reportError("Client Error Creating the Socket");
	}

	// - Create the remote address object
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);

	// - Connect to the remote address
	ret = connect(socketClient, (const sockaddr*)&serverAddress, sizeof(serverAddress));
	if (ret == SOCKET_ERROR)
	{
		reportError("Client Error Conecting to the Server");
	}
	else
	{
		// - Add the created socket to the managed list of sockets using addSocket()
		addSocket(socketClient);

		// If everything was ok... change the state
		state = ClientState::Start;
	}

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	OutputMemoryStream packet;

	if (state == ClientState::Start)
	{
		// TODO(jesus): Send the player name to the server
		//int ret = send(socketClient, playerName.c_str(), playerName.size(), 0); --------> Updated

		packet << ClientMessage::Hello;
		packet << playerName;

		if (sendPacket(packet, socketClient))
		{
			state = ClientState::Logging;
		}
		else
		{
			disconnect();
			state = ClientState::Stopped;
		}
	}

	if (state == ClientState::Logging)
	{
		if (send)
		{
			if (message[0] == '/')
			{
				DLOG("Executing comand %s", message.c_str());
				packet << ClientMessage::Command;
			}
			else
			{
				packet << ClientMessage::Send;
			}

			packet << message;

			if (!sendPacket(packet, socketClient))
			{
				disconnect();
				state = ClientState::Stopped;
			}

			message.clear();
			send = false;
		}
	}

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", playerName.c_str());
		if (ImGui::Button("Disconect"))
		{
			onSocketDisconnected(socketClient);
			shutdown(socketClient, 2);
		}
		
		ImGui::BeginChild("Xat",ImVec2(375,400),true);

		for (int i = 0; i < Messages.size(); ++i)
		{
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, Messages[i].color);
			ImGui::Text("%s", Messages[i].message.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();

		char buff[1024]="\0";

		if (ImGui::InputText("write", buff, 1024, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			send = true;
			std::string mymessage(buff);
			message = mymessage;
		}

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream &packet)
{
	ServerMessage serverMessage;
	packet >> serverMessage;

	if (serverMessage == ServerMessage::Welcome)
	{
		std::string welcomeMessage;
		packet >> welcomeMessage;

		Message welcome;
		welcome.color = { 0.0f,1.0f,0.0f,1.0f };
		welcome.message = welcomeMessage;

		Messages.push_back(welcome);
	}
	else if (serverMessage == ServerMessage::Unwelcome)
	{
		std::string unwelcomeMessage;
		packet >> unwelcomeMessage;

		disconnect();
		state = ClientState::Stopped;

		WLOG("%s", unwelcomeMessage.c_str());

		//Messages.push_back(unwelcomeMessage);
	}
	else if (serverMessage == ServerMessage::Newuser)
	{
		std::string newusermessage;
		packet >> newusermessage;

		Message newuser;
		newuser.color = { 0.0f,0.0f,1.0f,1.0f };
		newuser.message = newusermessage;

		Messages.push_back(newuser);
	}
	else if (serverMessage == ServerMessage::Newmessage)
	{
		std::string newmessage;
		packet >> newmessage;

		Message new_message;
		new_message.color = { 1.0f,1.0f,1.0f,1.0f };
		new_message.message = newmessage;

		Messages.push_back(new_message);
	}
	else if (serverMessage == ServerMessage::Userdisconnected)
	{
		std::string newmessage;
		packet >> newmessage;

		Message new_message;
		new_message.color = { 1.0f,0.0f,0.0f,1.0f };
		new_message.message = newmessage;

		Messages.push_back(new_message);
	}
	else if (serverMessage == ServerMessage::Command)
	{
		std::string newmessage;
		packet >> newmessage;

		Message new_message;
		new_message.color = { 1.0f,1.0f,0.0f,1.0f };
		new_message.message = newmessage;

		Messages.push_back(new_message);
	}
	else if (serverMessage == ServerMessage::ComDisconnect)
	{
		onSocketDisconnected(socketClient);
		shutdown(socketClient, 2);
		disconnect();
	}
	else if (serverMessage == ServerMessage::NewName)
	{
		std::string newname;
		packet >> newname;

		playerName = newname;
	}

}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{

	state = ClientState::Stopped;
	Messages.clear();
}



