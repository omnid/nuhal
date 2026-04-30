/// \file
/// \brief test the queue under a concurrent environment with producer and consumer threads
#define CATCH_CONFIG_MAIN
#include "nuhal/queue.h"
#include "nuhal/utilities.h"
#include "nuhal/time.h"
#include "nuhal/catch.hpp"
#include <thread>


/// test the queue using a single producer and consumer thread
TEST_CASE("queue_single_prod_cons", "[queue]")
{
    using std::vector;
    struct item
    {
        int x, y;
        uint32_t delay;
        bool exit;

        bool operator==(const item & rhs) const
        {
            return x == rhs.x && y == rhs.y && delay == rhs.delay
                && exit == rhs.exit;
        }
    };

    struct item items[4];
    struct queue cue = queue_init(ARRAY_LEN(items), sizeof(items[0]), items);

    const vector<item> to_produce
        { {1,1,1, false}, {2,2,20,false}, {3,3,3,false}, {4,4,40, false},
          {5,5,50, false}, {6,6,60,false}, {7,7,7,false}, {8,8,80, false},
          {9,9,9, false}, {10,10,1,false}, {11,11,110,false}, {12,12,12, true}};
    vector<item> consumed;
  
    // start the consumer
    auto consumer = std::thread([&cue, &consumed]()
                {
                    bool exit = false;
                    while(!exit)
                    {
                        struct item curr;
                        queue_pop_block(&cue, &curr, 0);
                        consumed.push_back(curr);
                        if(curr.exit)
                        {
                            break;
                        }
                    }
                });

    /// the producer adds items to the queue and delays by some itnerval
    /// inbetween
    auto producer = std::thread([&cue, &to_produce]()
                {
                    for(const auto & p : to_produce)
                    {
                        if(p.x % 2 == 0)
                        {
                            time_delay_us(p.delay);
                        }
                        else
                        {
                            time_delay_ms(p.delay);
                        }
                        queue_push_block(&cue, &p, 0);
                    }
                });

        // wait for the threads to finish
        consumer.join();
        producer.join();

        using std::equal;
        using std::end;
        using std::begin;
        CHECK(equal(begin(to_produce), end(to_produce), begin(consumed), end(consumed)));
}
