#include "Networks.h"
#include "DeliveryManager.h"

//Server
Delivery * DeliveryManager::writeSequenceNumber(OutputMemoryStream & packet, uint32 forcedSequence)
{
	Delivery* ret = new Delivery();

	if (forcedSequence == -1)
	{
		packet << nextSequenceNumber;

		ret->sequenceNumber = nextSequenceNumber;
		ret->dispatchTime = Time.time;

		nextSequenceNumber++;
	}
	else
	{
		packet << forcedSequence;

		ret->sequenceNumber = forcedSequence;
		ret->dispatchTime = Time.time;
	}

	currDeliveries.push_back(ret);

	return ret;
}

//Client
bool DeliveryManager::processSequenceNumber(const InputMemoryStream & packet)
{
	bool ret = false;

	uint32 sequence;
	packet >> sequence;

	if (sequence == nextSequenceExpected)
	{
		nextSequenceExpected++;
		pendingACK.push_back(sequence);
		ret = true;
	}

	return ret;
}

//Client
bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	bool ret = false;

	if (pendingACK.size() > 0)
	{
		ret = true;
	}

	return ret;
}

//Client
void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream & packet)
{
	for (std::list<uint32>::iterator ACK_it = pendingACK.begin(); ACK_it != pendingACK.end(); ACK_it++)
	{
		packet << (*ACK_it);
	}

	pendingACK.clear();
}

//Server
void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream & packet)
{
	std::vector<Delivery*> to_erase;

	while (packet.RemainingByteCount() > 0)
	{
		uint32 sequenceNumber = -1;
		packet >> sequenceNumber;

		for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
		{
			if ((*dv_it)->sequenceNumber == sequenceNumber)
			{
				(*dv_it)->delegate->onDeliverySuccess(this);
				to_erase.push_back((*dv_it));
				break;
			}
		}
	}

	for (int i = 0; i < to_erase.size(); ++i)
	{
		currDeliveries.remove(to_erase[i]);
	}

	to_erase.clear();
}

//Server
void DeliveryManager::processTimedOutPackets()
{
	std::vector<Delivery*> to_erase;

	for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
	{
		if ((*dv_it)->dispatchTime + DELIVERY_TIME_OUT < Time.time)
		{
			//(*dv_it)->delegate->onDeliveryFailure(this);
			to_erase.push_back((*dv_it));
			break;
		}
	}

	for (int i = 0; i < to_erase.size(); ++i)
	{
		currDeliveries.remove(to_erase[i]);
	}

	to_erase.clear();
}

//Server & Client
void DeliveryManager::clear()
{
	for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
	{
		(*dv_it) = nullptr;
	}

	currDeliveries.clear();
	pendingACK.clear();
}


//------------------------------------------------------ Delivery Delegates ----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

void DeliveryDelegateReplication::onDeliverySuccess(DeliveryManager * deliveryManager)
{
	DLOG("Deleting Delivery");
}

void DeliveryDelegateReplication::onDeliveryFailure(DeliveryManager * deliveryManager)
{
	if (used)
	{
		OutputMemoryStream packet;
		packet << ServerMessage::Replication;

		Delivery* recreatedDelivery = nullptr;

		recreatedDelivery = deliveryManager->writeSequenceNumber(packet,deliverySequence);
		recreatedDelivery->delegate = new DeliveryDelegateReplication();

		recreateCommands(packet);

		replicationManager->write(packet,recreatedDelivery);
	}
	
}

void DeliveryDelegateReplication::recreateCommands(OutputMemoryStream & packet)
{
	for (std::map<uint32, ReplicationAction>::iterator it_rc = deliveryReplicationCommands.begin(); it_rc != deliveryReplicationCommands.end(); ++it_rc)
	{
		switch (it_rc->second)
		{
		case ReplicationAction::Create:
			replicationManager->create(it_rc->first);
			break;
		case ReplicationAction::Update:
			replicationManager->update(it_rc->first);
			break;
		case ReplicationAction::Destroy:
			replicationManager->destroy(it_rc->first);
			break;
		default:
			break;
		}

	}
}
