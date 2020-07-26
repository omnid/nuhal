#include "nuhal/queue.h"
#include "nuhal/error.h"
#include "nuhal/time.h"

#if defined(__GNUC__)

/// atomic increment for gcc compiler
#define ATOMIC_INC(val) (void)__sync_fetch_and_add(&val, 1)

#elif defined(__TI_ARM__)

#define ATOMIC_INC(val) ++val

#endif

static inline uint32_t queue_index(const struct queue * queue, uint32_t curr)
{
    return curr & queue->mask;
}

struct queue queue_init(uint32_t capacity,
                        uint32_t item_size,
                        volatile void * data)
{
    // if capacity is not a power of 2
    if(0 != ((capacity - 1) & capacity) || capacity <= 1)
    {
        error(FILE_LINE, "capacity must be a power of 2 > 1");
    }

    struct queue out = {capacity - 1, item_size, data, 0, 0};
    return out;
}

bool queue_is_full(const struct queue * queue)
{
    if(!queue)
    {
        error(FILE_LINE, "queue is NULL");
    }
    return queue_index(queue, queue->write_index + 1)
        == queue_index(queue, queue->read_index);
}

bool queue_is_empty(const struct queue * queue)
{
    if(!queue)
    {
        error(FILE_LINE, "queue is NULL");
    }
    return queue_index(queue, queue->read_index)
        == queue_index(queue, queue->write_index);
}

bool queue_push_nonblock(struct queue * queue, const void * data)
{
    if(!queue || !data)
    {
        error(FILE_LINE, "NULL pointer");
    }

    if(queue_is_full(queue))
    {
        return false;
    }
    // it is important that we add the data to the queue prior to
    // updating the write_index. since queue->write_index is volatile
    // and we are copying to a volatile array, and each expression
    // is a sequence point (in the parlance of the C standard)
    // the write operation and updating the write_index pointer cannot be
    // re-ordered relative to each other
    const uint32_t index = queue_index(queue, queue->write_index);
    for(uint32_t i = 0; i != queue->item_size; ++i)
    {
        queue->data[index * queue->item_size + i] = ((uint8_t *)data)[i];
    }

    ATOMIC_INC(queue->write_index);
    return true;
}


void queue_push_error(struct queue * queue, const void * data)
{
    if(!queue_push_nonblock(queue, data))
    {
        error(FILE_LINE, "queue is full");
    }
}

void queue_push_block(struct queue * queue, const void * data, uint32_t timeout)
{
    struct time_elapsed_ms stamp = time_elapsed_ms_init();

    while(timeout == 0 || time_elapsed_ms(&stamp) < timeout)
    {
        if(queue_push_nonblock(queue, data))
        {
            return;
        }
    }
    // if we get here, we have timed out
    error(FILE_LINE, "timeout");
}


bool queue_pop_nonblock(struct queue * queue, void * out)
{
    if(!queue)
    {
        error(FILE_LINE, "NULL pointer");
    }

    if(queue_is_empty(queue))
    {
        return false;
    }

    if(out)
    {
        // copy data from the queue to the out variable.
        // this copy must happen prior to advancing the read_index pointer.
        // because queue->data is volatile and read_index pointer is volatile
        // the operations cannot be re-ordered
        const uint32_t index = queue_index(queue, queue->read_index);
        for(uint32_t i = 0; i != queue->item_size; ++i)
        {
            ((uint8_t*)out)[i] = queue->data[index * queue->item_size + i];
        }
    }

    // this line ensures that read_index is written in one go and after
    // the data has been written
    ATOMIC_INC(queue->read_index);
    return true;
}

void queue_pop_error(struct queue * queue, void * out)
{
    if(!queue_pop_nonblock(queue, out))
    {
        error(FILE_LINE, "queue is empty");
    }
}

void queue_pop_block(struct queue * queue, void * out, uint32_t timeout)
{
    struct time_elapsed_ms stamp = time_elapsed_ms_init();

    while(timeout == 0 || time_elapsed_ms(&stamp) < timeout)
    {
        if(queue_pop_nonblock(queue, out))
        {
            return;
        }
    }
    error(FILE_LINE, "timeout");
}
