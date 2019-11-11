#include "Networks.h"
#include "ReplicationManagerServer.h"

void ReplicationManagerServer::create(uint32 networkId)
{
	ReplicationCommand createCommand;
	createCommand.action = ReplicationAction::Create;
	createCommand.networkId = networkId;

	replicationCommands.push_back(createCommand);
}

void ReplicationManagerServer::update(uint32 networkId)
{
	ReplicationCommand updateCommand;
	updateCommand.action = ReplicationAction::Update;
	updateCommand.networkId = networkId;

	replicationCommands.push_back(updateCommand);
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	ReplicationCommand destroyCommand;
	destroyCommand.action = ReplicationAction::Destroy;
	destroyCommand.networkId = networkId;

	replicationCommands.push_back(destroyCommand);
}

void ReplicationManagerServer::write(OutputMemoryStream & packet)
{
	for (int i = 0; i < replicationCommands.size(); ++i)
	{
		if (replicationCommands[i].action == ReplicationAction::Create)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(replicationCommands[i].networkId);

			if (object)
			{
				packet << replicationCommands[i].networkId;
				packet << replicationCommands[i].action;

				packet << object->position.x;
				packet << object->position.y;
				packet << object->angle;
				packet << object->size.x;
				packet << object->size.y;

				std::string texture(object->texture->filename);
				packet << texture;
			}			
		}
		else if (replicationCommands[i].action == ReplicationAction::Update)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(replicationCommands[i].networkId);

			if (object)
			{
				packet << replicationCommands[i].networkId;
				packet << replicationCommands[i].action;

				packet << object->position.x;
				packet << object->position.y;
				packet << object->angle;
			}
		}
		else if (replicationCommands[i].action == ReplicationAction::Destroy)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(replicationCommands[i].networkId);

			if (object)
			{
				packet << replicationCommands[i].networkId;
				packet << replicationCommands[i].action;
			}
		}
	}

	replicationCommands.clear();
}
