#include <assert.h>
#include <stdio.h>

#include "rbuf.h"
#include "primitives.h"

int main() {
    printf("testing ring buffer\n");

    typedef struct Position {
        f64 x;
        f64 y;
        f64 z;
    } Position;

    Position data[3];

    rbuf buf = {
        .buf = &data,
        .length = 3,
        .obj_size = sizeof(Position),
        .overwrite_when_full = false
    };

    Position p = {
        .x = 1,
        .y = 2
    };

    RBUF_STATUS s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_OK); // should be ok

    p.z = 2;
    s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_OK); // should be ok

    p.z = 3;
    s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_OK); // should be ok

    p.z = 4;
    s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_BUFFER_FULL); // don't overwrite when full

    p.z = 100;
    s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_BUFFER_FULL); // still full


    // now try the overwrite when full behavior
    buf.overwrite_when_full = true;

    s = rbuf_push_back(&buf, &p);
    assert(s == RBUF_OK); // should allow overwrites now

    // check pops
    Position p2;
    assert(p2.x != p.x);
    s = rbuf_pop_front(&buf, &p2);
    assert(s == RBUF_OK);
    assert(p.x == p2.x && p.y == p2.y && p.z == p2.z);

    // check clear on pop
    u32 saved_front = buf.front;
    assert(data[buf.front].x == 1);
    buf.clear_on_pop = true;
    s = rbuf_pop_front(&buf, &p2);
    assert(s == RBUF_OK);
    assert(data[saved_front].x == 0);
    assert(data[saved_front].y == 0);
    assert(data[saved_front].z == 0);

    printf("All tests passed\n");

    return 0;
}