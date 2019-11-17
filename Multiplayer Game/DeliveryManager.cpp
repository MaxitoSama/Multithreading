#include "Networks.h"
#include "DeliveryManager.h"

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

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	bool ret = false;

	if (pendingACK.size() > 0)
	{
		ret = true;
	}

	return ret;
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream & packet)
{
	for (std::list<uint32>::iterator ACK_it = pendingACK.begin(); ACK_it != pendingACK.end(); ACK_it++)
	{
		packet << (*ACK_it);
	}
}

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream & packet)
{
}

void DeliveryManager::processTimedOutPackets()
{
}

void DeliveryManager::clear()
{
}
