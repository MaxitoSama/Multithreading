#include "Networks.h"
#include "ReplicationManagerClient.h"

void ReplicationManagerClient::read(const InputMemoryStream & packet)
{
	if (packet.GetSize() > 0)
	{
		uint32 networkId;
		packet >> networkId;

		ReplicationAction action;
		packet >> action;

		if (action == ReplicationAction::Create || action == ReplicationAction::CreateClients)
		{
			GameObject* object = nullptr;
			object = Instantiate();

			if (object)
			{
				if (action == ReplicationAction::Create)
				{
					App->modLinkingContext->registerNetworkGameObject(object);
				}
				else
				{
					App->modLinkingContext->registerNetworkGameObjectWithNetworkId(object,networkId);
				}

				std::string texture;

				packet >> object->position.x;
				packet >> object->position.y;
				packet >> object->angle;
				packet >> object->size.x;
				packet >> object->size.y;
				packet >> texture;

				object->texture = App->modTextures->loadTexture(texture.c_str());
			}
		}
		else if (action == ReplicationAction::Update)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(networkId);

			if (object)
			{
				packet >> object->position.x;
				packet >> object->position.y;
				packet >> object->angle;
			}
		}
		else
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(networkId);
			
			App->modLinkingContext->unregisterNetworkGameObject(object);

			Destroy(object);			
		}
	}
}
