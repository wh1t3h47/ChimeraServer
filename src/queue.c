// A fifo queue implementation with mutex locking and dynamic memory allocation.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "queue.h"

//
// Function: queue_init
//
// Initialize a queue.
//
// Parameters:
//     q - pointer to the queue to initialize
//
// Returns:
//     0 on success, -1 on failure
//
int queue_init(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);

    return 0;
}

//
// Function: queue_create
//
// Create a queue.
//
// Returns:
//     pointer of the created queue on success, NULL on failure
//
queue_t *queue_create()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL)
    {
        return NULL;
    }

    if (queue_init(q) == -1)
    {
        free(q);
        return NULL;
    }

    return q;
}

//
// Function: queue_destroy
//
// Destroy a queue.
//
// Parameters:
//     q - pointer to the queue to destroy
//
// Returns:
//     0 on success, -1 on failure
//
int queue_destroy(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);

    return 0;
}

//
// Function: queue_push
//
// Push an item onto the tail of the queue.
//
// Parameters:
//     q - pointer to the queue
//     item - pointer to the item to push
//
// Returns:
//     0 on success, -1 on failure
//
int queue_push(queue_t *q, void *item)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&q->mutex);

    queue_item_t *new_item = malloc(sizeof(queue_item_t));
    if (new_item == NULL)
    {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }

    new_item->data = ((queue_item_t *)item)->data;
    new_item->operation = ((queue_item_t *)item)->operation;
    new_item->id = ((queue_item_t *)item)->id;
    new_item->next = NULL;

    if (q->tail == NULL)
    {
        q->head = new_item;
    }
    else
    {
        q->tail = new_item;
    }

    q->tail = new_item;
    q->size++;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);

    return 0;
}

//
// Function: queue_pop
//
// Pop an item from the head of the queue.
//
// Parameters:
//     q - pointer to the queue
//     item - pointer to a pointer to store the item
//
// Returns:
//     0 on success, -1 on failure
//
int queue_pop(queue_t *q, void **item)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if (item == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&q->mutex);

    while (q->size == 0)
    {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    queue_item_t *old_head = q->head;
    q->head = old_head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    q->size--;

    *item = old_head;
    free(old_head);

    pthread_mutex_unlock(&q->mutex);

    return 0;
}

//
// Function: queue_size
//
// Get the size of the queue.
//
// Parameters:
//     q - pointer to the queue
//
// Returns:
//     the size of the queue
//
int queue_size(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return q->size;
}

//
// Function: queue_is_empty
//
// Check if the queue is empty.
//
// Parameters:
//     q - pointer to the queue
//
// Returns:
//     1 if the queue is empty, 0 otherwise
//
int queue_is_empty(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return (q->size == 0);
}

//
// Function: empty_queue
//
// Empty the queue and frees all the memory allocated to the queue.
//
// Parameters:
//     q - pointer to the queue
//
// Returns:
//     0 on success, -1 on failure
//
int empty_queue(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&q->mutex);

    while (q->size > 0)
    {
        queue_item_t *old_head = q->head;
        q->head = old_head->next;
        if (q->head == NULL)
        {
            q->tail = NULL;
        }
        q->size--;

        free(old_head);
    }

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

//
// Function: queue_destroy
//
// Destroy a queue.
//
// Parameters:
//     q - pointer to the queue to destroy
//
// Returns:
//     0 on success, -1 on failure
//
int queue_destroy(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);

    return 0;
}

//
// Function: queue_delete
//
// Free the entire queue using empty_queue and destroy it.
//
// Parameters:
//     q - pointer to the queue
//     item - pointer to the item to delete
//
// Returns:
//     0 on success, -1 on failure
//
int queue_delete(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    empty_queue(q);
    queue_destroy(q);
    free(q);
    return 0;
}
