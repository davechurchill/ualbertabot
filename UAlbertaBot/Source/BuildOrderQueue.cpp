#include "BuildOrderQueue.h"

using namespace UAlbertaBot;

BuildOrderQueue::BuildOrderQueue() 
	: highestPriority(0), 
	  lowestPriority(0),
	  defaultPrioritySpacing(10),
	  numSkippedItems(0)
{
	
}

void BuildOrderQueue::clearAll() 
{
	// clear the queue
	queue.clear();

	// reset the priorities
	highestPriority = 0;
	lowestPriority = 0;
}

BuildOrderItem & BuildOrderQueue::getHighestPriorityItem() 
{
	// reset the number of skipped items to zero
	numSkippedItems = 0;

	// the queue will be sorted with the highest priority at the back
	return queue.back();
}

BuildOrderItem & BuildOrderQueue::getNextHighestPriorityItem() 
{
	assert(queue.size() - 1 - numSkippedItems >= 0);

	// the queue will be sorted with the highest priority at the back
	return queue[queue.size() - 1 - numSkippedItems];
}

void BuildOrderQueue::skipItem()
{
	// make sure we can skip
	assert(canSkipItem());

	// skip it
	numSkippedItems++;
}

bool BuildOrderQueue::canSkipItem() 
{
	// does the queue have more elements
	bool bigEnough = queue.size() > (size_t)(1 + numSkippedItems);

	if (!bigEnough) 
	{
		return false;
	}

	// is the current highest priority item not blocking a skip
	bool highestNotBlocking = !queue[queue.size() - 1 - numSkippedItems].blocking;

	// this tells us if we can skip
	return highestNotBlocking;
}

void BuildOrderQueue::queueItem(BuildOrderItem b) 
{
	// if the queue is empty, set the highest and lowest priorities
	if (queue.empty()) 
	{
		highestPriority = b.priority;
		lowestPriority = b.priority;
	}

	// push the item into the queue
	if (b.priority <= lowestPriority) 
	{
		queue.push_front(b);
	}
	else
	{
		queue.push_back(b);
	}

	// if the item is somewhere in the middle, we have to sort again
	if ((queue.size() > 1) && (b.priority < highestPriority) && (b.priority > lowestPriority)) 
	{
		// sort the list in ascending order, putting highest priority at the top
		std::sort(queue.begin(), queue.end());
	}

	// update the highest or lowest if it is beaten
	highestPriority = (b.priority > highestPriority) ? b.priority : highestPriority;
	lowestPriority  = (b.priority < lowestPriority)  ? b.priority : lowestPriority;
}

void BuildOrderQueue::queueAsHighestPriority(MetaType m, bool blocking) 
{
	// the new priority will be higher
	int newPriority = highestPriority + defaultPrioritySpacing;

	// queue the item
	queueItem(BuildOrderItem(m, newPriority, blocking));
}

void BuildOrderQueue::queueAsLowestPriority(MetaType m, bool blocking) 
{
	// the new priority will be higher
	int newPriority = lowestPriority - defaultPrioritySpacing;

	// queue the item
	queueItem(BuildOrderItem(m, newPriority, blocking));
}

void BuildOrderQueue::removeHighestPriorityItem() 
{
	// remove the back element of the vector
	queue.pop_back();

	// if the list is not empty, set the highest accordingly
	highestPriority = queue.empty() ? 0 : queue.back().priority;
	lowestPriority  = queue.empty() ? 0 : lowestPriority;
}

void BuildOrderQueue::removeCurrentHighestPriorityItem() 
{
	// remove the back element of the vector
	queue.erase(queue.begin() + queue.size() - 1 - numSkippedItems);

	//assert((int)(queue.size()) < size);

	// if the list is not empty, set the highest accordingly
	highestPriority = queue.empty() ? 0 : queue.back().priority;
	lowestPriority  = queue.empty() ? 0 : lowestPriority;
}

size_t BuildOrderQueue::size() 
{
	return queue.size();
}

bool BuildOrderQueue::isEmpty()
{
	return (queue.size() == 0);
}

BuildOrderItem BuildOrderQueue::operator [] (int i)
{
	return queue[i];
}

void BuildOrderQueue::drawQueueInformation(int x, int y) 
{
	//x = x + 25;

    if (!Config::Debug::DrawProductionInfo)
    {
        return;
    }
	
	std::string prefix = "\x04";

	size_t reps = queue.size() < 12 ? queue.size() : 12;

	// for each unit in the queue
	for (size_t i(0); i<reps; i++) {

		prefix = "\x04";

        const MetaType & type = queue[queue.size() - 1 - i].metaType;

        if (type.isUnit())
        {
            if (type.getUnitType().isWorker())
            {
                prefix = "\x1F";
            }
            else if (type.getUnitType().supplyProvided() > 0)
            {
                prefix = "\x03";
            }
            else if (type.getUnitType().isRefinery())
            {
                prefix = "\x1E";
            }
            else if (type.isBuilding())
            {
                prefix = "\x11";
            }
            else if (type.getUnitType().groundWeapon() != BWAPI::WeaponTypes::None || type.getUnitType().airWeapon() != BWAPI::WeaponTypes::None)
            {
                prefix = "\x06";
            }
        }

		BWAPI::Broodwar->drawTextScreen(x, y+(i*10), " %s%s", prefix.c_str(), type.getName().c_str());
	}
}
