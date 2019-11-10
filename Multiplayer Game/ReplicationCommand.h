#pragma once

enum class ReplicationAction {
	None,
	Create,
	CreateClients,
	Update,
	Destroy
};

struct ReplicationCommand
{
	ReplicationAction action;
	uint32 networkId;
};

