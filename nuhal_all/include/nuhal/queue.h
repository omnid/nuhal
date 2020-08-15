#ifndef COMMON_QUEUE_H_INCLUDE_GUARD
#define COMMON_QUEUE_H_INCLUDE_GUARD
/// @file
/// @brief A basic circular buffer, suitable
///        for a single produce and single consumer
#include<stdint.h>
#include<stdbool.h>


/// @brief the queue data structure
struct queue
{
    /// bitmask for the size of the queue is all the bits
    /// in an index that can be a 1 while still being within
    /// the capacity of the queue. the queue capacity
    /// is the maximum number of items in the queue and
    /// must be a power of two
    uint32_t mask;
    uint32_t item_size;      /// the size (in bytes) of each item
    volatile uint8_t * data;    /// buffer for the queue data
    volatile uint32_t write_index; /// queue write location 
    volatile uint32_t read_index;  /// queue read location
};

#ifdef __cplusplus
extern "C" {
#endif 

/// @brief create a new queue
/// @param capacity - the maximum number of items in the queue
/// NOTE: one slot will always be unused to simplify detecting of full vs empty
/// @param item_size - the size in bytes of each item
/// @param data - buffer where the data should be stored. must
/// have capacity*item_size bytes available
/// @return the initialized queue
struct queue queue_init(uint32_t capacity,
                uint32_t item_size,
                volatile void * data);

/// @brief determine if there is space on the queue
/// @param queue - the queue to examine
/// @return true if there is room for another item on the queue,
///  false otherwise
bool queue_is_full(const struct queue * queue);

/// @brief determine if the queue is empty
/// @return true if the queue is empty, false otherwise
bool queue_is_empty(const struct queue * queue);

/// @brief add an item to the queue. if there is no space return immediately
/// @param queue - the structure describing the queue
/// @param data - data to push onto the queue
/// @return true if data was pushed onto the queue, false otherwise
bool queue_push_nonblock(struct queue * queue, const void * data);

/// @brief push data to the queue. if the queue is full issue an error
/// @param queue - the structure describing the queue
/// @param data - data to push onto the queue
/// @post a fatal error is triggered if the queue is full
void queue_push_error(struct queue * queue, const void * data);

/// @brief add an item to the queue. wait for space to be available
///  or for a timeout to occur
/// @param queue - the queue to manipulate
/// @param data - the data to add to the queue
/// @param timeout - time in ms to wait for space on queue.
///         if 0 timeout is ignored
void queue_push_block(struct queue * queue, const void * data, uint32_t timeout);

/// @brief retrieve an item from the queue. If there is nothing on the queue
/// return immediately
/// @param queue - the queue from which to retrieve an item
/// @param out - buffer in which to store the item.  If NULL then no item
/// is stored but an item is still removed from the queue.
/// @return true if there was an item to retrieve, false otherwise. if no item
/// is retrieved, out is not modified
bool queue_pop_nonblock(struct queue * queue, void * out);

/// @brief retrieve an item from the queue. if queue is empty, issue an error
/// @param queue - the queue data structure
/// @param out - buffer in which to store the item. if NULL no item is stored
/// @post a fatal error occurs if the queue is empty
void queue_pop_error(struct queue * queue, void * out);

/// @brief retrieve an item from the queue. Wait up to timout ms for an item
/// @param queue - the queue from which to retrieve the item
/// @param out - buffer in which to store the item. If NULL no item is stored
/// but an item is still removed from the queue
/// @param timeout - time in ms to wait for an item to be on the queue. if 0
/// than timeout is ignored
void queue_pop_block(struct queue * queue, void * out, uint32_t timeout);

#ifdef __cplusplus
}
#endif 
#endif
