// A fifo queue implementation with mutex locking and dynamic memory allocation.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "queue.h"

/**
 * @brief Initialize a queue
 * @param q pointer to the queue to initialize
 * @return 0 on success, -1 on failure
 * @note This function is not thread-safe if the pointer q is being
 * used by another thread.
 */

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
    q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    q->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);

    return 0;
}

/**
 * @brief Create a queue
 * @return 0 on success, -1 on failure
 * @note This function is thread safe
 */

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

/**
 * @brief Push an item onto the tail of the queue.
 * @param q - the queue
 * @param item - the item to push onto the queue
 * @return 0 on success, -1 on failure
 * @note
 *    This function is thread-safe.
 *    The mutex is locked before pushing the item.
 *    The mutex is unlocked after pushing the item.
 *    The cond is signaled after pushing the item to wake up the waiting thread.
 * 
 */

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

/**
 * @brief Pop an item from the head of the queue.
 * @param q - the queue
 * @param item - pointer to a pointer to store the item
 * @return 0 on success, -1 on failure
 * @note This function is thread-safe.
 */

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

/**
 * @brief Get the size of the queue.
 * @param q - pointer to the queue
 * @return the size of the queue
 * @note this function is thread-safe
 */

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
//     true if the queue is empty, false otherwise
//

/**
 * @brief Check if the queue is empty
 * @param q - pointer to the queue
 * @return true if the queue is empty, false otherwise
 * @note This function is thread-safe
 */

bool queue_is_empty(queue_t *q)
{
    if (q == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return (q->size == 0);
}

/**
 * @brief Empty the queue and frees all the memory allocated to the queue.
 * @param q - pointer to the queue
 * @return 0 on success, -1 on failure
 * @note This function is thread-safe
 */

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

/**
 * @brief Destroy a queue
 * @param q - pointer to the queue to destroy
 * @return 0 on success, -1 on failure
 * @note This function is NOT thread-safe
 */

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

/**
 * @brief Delete a queue
 * @param q - pointer to the queue to delete
 * @return 0 on success, -1 on failure
 * @note This function is NOT thread-safe
 */

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
