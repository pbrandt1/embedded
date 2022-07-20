#ifndef RBUF_H_
#define RBUF_H_

#include <string.h>
#include <stdio.h>
#include "primitives.h"

/**
 * @brief ring buffer
 * 
 */
typedef struct rbuf {
    void * const buf;
    const u32 length;
    const size_t obj_size;
    u32 front;
    u32 back;
    u32 count; //< for tracking if full
    bool overwrite_when_full;
    bool clear_on_pop;
} rbuf;

/**
 * @brief return status for all ring buffer operations
 * 
 */
typedef enum RBUF_STATUS {
    RBUF_OK = 0,
    RBUF_NULL_BUFFER_POINTER = 1,
    RBUF_NULL_OBJECT_POINTER = 2,
    RBUF_BUFFER_EMPTY = 3,
    RBUF_BUFFER_FULL = 4,
    RBUF_ERROR
} RBUF_STATUS;

/**
 * @brief check if it's empty
 * 
 * if you have to ask i'm not telling you
 * 
 * @param rb 
 * @return true 
 * @return false 
 */
static bool rbuf_is_empty(rbuf* rb) {
    return rb->count == 0;
}

/**
 * @brief check if full
 * 
 * if you have to ask i'm not telling you
 * 
 * @param rb 
 * @return true 
 * @return false 
 */
static bool rbuf_is_full(rbuf* rb) {
    return rb->count >= rb->length;
    // return (rb->front == rb->back - 1) || ( (rb->back == rb->length - 1) && ( rb->front == 0 ) );
}

/**
 * @brief push an object on the the back of the buffer
 * 
 * @param rb 
 * @param obj 
 * @return RBUF_STATUS 
 */
static RBUF_STATUS rbuf_push_back(rbuf* rb, void* obj) {
    RBUF_STATUS ret = RBUF_ERROR;

    if (rb == 0) {
        ret = RBUF_NULL_BUFFER_POINTER;
    } else if (obj == 0) {
        ret = RBUF_NULL_OBJECT_POINTER;
    } else if (!rb->overwrite_when_full && rbuf_is_full(rb)) {
        ret = RBUF_BUFFER_FULL;
    } else {

        i32 next;
        if (rb->back == rb->length - 1) {
            next = -rb->back;
        } else {
            next = 1;
        }

        memcpy(rb->buf + rb->back * rb->obj_size, obj, rb->obj_size);
        
        // rb->back = next;
        __atomic_fetch_add(&(rb->back), next, __ATOMIC_SEQ_CST);

        // rb->count++;
        __atomic_fetch_add(&(rb->count), 1, __ATOMIC_SEQ_CST);
        ret = RBUF_OK;
    }

    return ret;
}

/**
 * @brief pops the front of the buffer off into your obj pointer
 * 
 * @param rb 
 * @param obj 
 * @return RBUF_STATUS 
 */
static RBUF_STATUS rbuf_pop_front(rbuf* rb, void* obj) {
    RBUF_STATUS ret = RBUF_ERROR;

    if (rb == 0) {
        ret = RBUF_NULL_BUFFER_POINTER;
    } else if (obj == 0) {
        ret = RBUF_NULL_OBJECT_POINTER;
    } else {

        u32 next;
        if (rb->front == rb->length - 1) {
            next = 0;
        } else {
            next = rb->front + 1;
        }

        memcpy(obj, rb->buf + rb->front * rb->obj_size, rb->obj_size);
        if (rb->clear_on_pop) {
            memset(rb->buf + rb->front * rb->obj_size, 0, rb->obj_size);
        }
        rb->front = next;
        --rb->count;
        ret = RBUF_OK;

    }

    return ret;
}

static void rbuf_hexdump(rbuf* rb) {
    printf("length: %d, obj_size: %ld, front: %d, back: %d, count: %d\n", rb->length, rb->obj_size, rb->front, rb->back, rb->count);
    for (u32 i = 0; i < rb->length; ++i) {
        printf("%2d|", i);
        for (u32 b = 0; b < rb->obj_size; ++b) {
            if (b % 8 == 0 && b != 0) {
                printf("  ");
            } else {
                printf(" ");
            }
            printf("%02x", *(u8*)(rb->buf + i * rb->obj_size + b));
        }
        printf("\n");
    }
}


#endif
