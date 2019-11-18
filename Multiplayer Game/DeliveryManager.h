#pragma once

#include <list>

#define DELIVERY_TIME_OUT              1.0f

class DeliveryManager;

class DeliveryDelegate
{
public:

	virtual void onDeliverySuccess(DeliveryManager *deliveryManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager *deliveryManager) = 0;

};

class DeliveryDelegateHello : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}
	
	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

class DeliveryDelegateWelcome : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}

	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

class DeliveryDelegateUnwelcome : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}

	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

class DeliveryDelegatePing : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}

	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

class DeliveryDelegateReplication : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}

	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

class DeliveryDelegateInput : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager)
	{

	}

	void onDeliveryFailure(DeliveryManager* deliveryManager)
	{

	}
};

struct Delivery
{
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* delegate = nullptr;
};

class DeliveryManager
{
public:

	//For senders to write a new seq. numbers into packet
	Delivery* writeSequenceNumber(OutputMemoryStream &packet);

	//For receivers to process the seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream &packet);

	//For receivers to write ack'ed seq. numbers into a packet
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream &packet);

	//For senders to process ack'ed seq. numbers from a packet
	void processAckdSequenceNumbers(const InputMemoryStream &packet);
	void processTimedOutPackets();

	void clear();

private:

	uint32 nextSequenceNumber = 0;
	std::list<Delivery*> currDeliveries;

	uint32 nextSequenceExpected=0;
	std::list<uint32> pendingACK;
		
};