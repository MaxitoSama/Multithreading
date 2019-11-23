#pragma once
#include "ReplicationCommand.h"

#include <list>
#include <map>
#include <vector>

#define DELIVERY_TIME_OUT              0.5f

class DeliveryManager;
class ReplicationManagerServer;

class DeliveryDelegate
{
public:

	virtual void onDeliverySuccess(DeliveryManager *deliveryManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager *deliveryManager, OutputMemoryStream& packet) = 0;

public:

	std::map<uint32, ReplicationAction> deliveryReplicationCommands;	// To copy the commands if we need to recreate the packet.
	bool used = false;													// To know that this delivery has a packet saved to be recreated
	ReplicationManagerServer* replicationManager = nullptr;				// To acces functions: Create, Write and Destroy.
	uint32 deliverySequence = 0;										// To recreate the delivery sequence.

};

class DeliveryDelegateReplication : public DeliveryDelegate
{
	void onDeliverySuccess(DeliveryManager* deliveryManager);

	void onDeliveryFailure(DeliveryManager* deliveryManager, OutputMemoryStream& packet);

	void recreateCommands(OutputMemoryStream &packet);

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
	Delivery* writeSequenceNumber(OutputMemoryStream &packet, uint32 forcedSequence);

	//For receivers to process the seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream &packet);

	//For receivers to write ack'ed seq. numbers into a packet
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream &packet);

	//For senders to process ack'ed seq. numbers from a packet
	void processAckdSequenceNumbers(const InputMemoryStream &packet);
	bool processTimedOutPackets(OutputMemoryStream& packet);

	void clear();

private:

	uint32 nextSequenceNumber = 0;
	std::list<Delivery*> currDeliveries;

	uint32 nextSequenceExpected=0;
	std::list<uint32> pendingACK;

public:

	std::vector<OutputMemoryStream*> packetsToResend;

};