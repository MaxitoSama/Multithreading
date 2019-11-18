#include "Networks.h"
#include "DeliveryManager.h"

//Server
Delivery * DeliveryManager::writeSequenceNumber(OutputMemoryStream & packet)
{
	Delivery* ret = new Delivery();
	
	packet << nextSequenceNumber;
	
	ret->sequenceNumber = nextSequenceNumber;
	ret->dispatchTime = Time.time;

	nextSequenceNumber++;

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
				DLOG("Deleting Delivery");
				to_erase.push_back((*dv_it));
				//(*dv_it)->delegate->onDeliverySuccess();
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
			to_erase.push_back((*dv_it));
			//(*dv_it)->delegate->onDeliveryFailure();
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
