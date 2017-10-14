#include "BuildOrderQueue.h"
#include <BWAPI/WeaponType.h>
#include <BWAPI/Game.h>
#include <assert.h>

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

size_t BuildOrderQueue::size() const
{
	return queue.size();
}

bool BuildOrderQueue::isEmpty()
{
	return (queue.size() == 0);
}

BuildOrderItem BuildOrderQueue::operator [] (int i) const
{
	return queue[i];
}

BuildOrderItem BuildOrderQueue::operator [] (size_t i) const
{
	return queue[i];
}
