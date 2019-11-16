#include "Networks.h"
#include "DeliveryManager.h"

Delivery * DeliveryManager::writeSequenceNumber(OutputMemoryStream & packet)
{
	return nullptr;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream & packet)
{
	return true;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return true;
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream & packet)
{
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
