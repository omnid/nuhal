#include "nuhal/queue.h"
#include "nuhal/utilities.h"
#include "nuhal/time.h"
#include "nuhal/catch.hpp"
#include <thread>

// some basic tests of the queue
TEST_CASE("queue_fifo", "[queue]")
{
    int items[4];
    struct queue cue = queue_init(ARRAY_LEN(items), sizeof(items[0]), items);
    int item = 78;
    // queue starts off empty
    CHECK(queue_is_empty(&cue));
    CHECK(!queue_pop_nonblock(&cue, &item));
    CHECK(78 == item); // queue was empty so item should not be modified

    // add some items to the queue
    CHECK(queue_push_nonblock(&cue, &item));
    ++item;
    CHECK(queue_push_nonblock(&cue, &item));
    ++item;
    CHECK(queue_push_nonblock(&cue, &item));

    // queue has one empty space so it should be full now
    CHECK(queue_is_full(&cue));

    // should not be able to push an item at this moment
    CHECK(!queue_push_nonblock(&cue, &item));

    int read = 0;
    CHECK(queue_pop_nonblock(&cue, &read));
    CHECK(78 == read);
    CHECK(queue_pop_nonblock(&cue, &read));
    CHECK(79 == read);
    CHECK(queue_pop_nonblock(&cue, &read));
    CHECK(80 == read);
    CHECK(queue_is_empty(&cue));

    // try pushing more items onto the queue
    item = 195;
    CHECK(queue_push_nonblock(&cue, &item));
    item = 178;
    CHECK(queue_push_nonblock(&cue, &item));
    CHECK(queue_pop_nonblock(&cue, &read));
    CHECK(195 == read);
    CHECK(queue_pop_nonblock(&cue, &read));
    CHECK(178 == read);
}

