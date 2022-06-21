#ifndef WEBX_QUEUE_H_
#define WEBX_QUEUE_H_

#include <deque>
#include <string>
#include <iostream>
#include <mutex>
#include <condition_variable>


/**
 * Template class defining a parallel queue. The access is sequentialised to avoid concurrent access problems. We put or get elements from
 * the queue. The queue is initialised with a size. The call to put is blocking when the queue is full. The call to get is also blocking when 
 * the queue is empty. The queue can be stopped. Any call to put or get lead to the UnavalaibleException.  
 */
template<typename ItemType>
class WebXQueue {
	
public:

	/**
	 * Constructor.
	 */
	WebXQueue();

	/**
	 * Virtual destructor. Deletes all the items.
	 */
	virtual ~WebXQueue();

	/**
	 * Puts the item in the correct postion corresponding to its priority. Is blocking when the queue is full. Waits for the queue to be not full. 
	 */
	void put(ItemType item, int priority = 0);

	/**
	 * Gets the item. Is blocking when the queue is empty. Waits for the queue to be not empty.
	 */
	ItemType get();

	/**
	 * Gets the size.
	 */
	long getNumberOfItems();
	
	/**
	 * Stops the queue. Becomes unavailable.
	 */
	void stop();

	int size() {
		return this->_queue.size();
	}

private:

	struct Item {
		ItemType content;
		int priority;
	};

	std::deque<Item> _queue;
	std::mutex _queueMutex;
	std::condition_variable _waitForItemDelivery;
	bool _unavailable;
};

template<typename ItemType> 
WebXQueue<ItemType>::WebXQueue() :
	_unavailable(false) {
}

template<typename ItemType> 
WebXQueue<ItemType>::~WebXQueue() {
}


template<typename ItemType> 
void WebXQueue<ItemType>::put(ItemType item, int priority) {
	
	std::unique_lock<std::mutex> lock(_queueMutex);
	  
	// Throwing exception if stopped
	if (_unavailable) {
		return;
	}
	
	// item to insert
	Item newItem;
	newItem.content = item;
	newItem.priority = priority;
			
	// Searching for the correct position
	typename std::deque<Item>::iterator i;
	for (i = _queue.begin(); i != _queue.end(); i++) {

		if (priority < i->priority) {
			_queue.insert(i, newItem);
			break;
		}
	}
	
	// A greater priority has not been found in the queue, inserting at the end.
	if (i == _queue.end()) {
		_queue.push_back(newItem);
	}
			
	_waitForItemDelivery.notify_one();
}

template<typename ItemType> 
ItemType WebXQueue<ItemType>::get() {
	
	std::unique_lock<std::mutex> lock(_queueMutex);
	
	// waiting until the wait for item delivery condition is fulfilled
	while (_queue.empty() && !_unavailable) {
		_waitForItemDelivery.wait(lock);
	}
	
	// Throwing exception if stopped
	if (_unavailable) {
		return NULL;
	}

	// getting the first element
	Item item = _queue.front();
	_queue.pop_front();
  
	return item.content;
	
}

template<typename ItemType>
long WebXQueue<ItemType>::getNumberOfItems() {
	
	const std::unique_lock<std::mutex> lock(_queueMutex);
	
	return _queue.size();
}

template<typename ItemType>
void WebXQueue<ItemType>::stop() {
	
	const std::unique_lock<std::mutex> lock(_queueMutex);
	_unavailable = true;
	
	_waitForItemDelivery.notify_all();	
}

#endif /*WEBX_QUEUE_H_*/
