#include "request.h";
#include "queue.h";
#include "queue.c";
#include "socket.c";

/** @brief convert a buffer to a queue item structure on the stack
 * @param buffer the buffer to convert
 * @return a queue item structure on the stack or NULL on error
 */
queue_item_t *parse_request(char *buffer)
{
    queue_item_t item;
    operation_t op = -1;
    char *p = buffer;
    item.next = NULL;

    if (buffer != NULL) {
        if (strncmp(buffer, "CREATE", 6) == 0)
        {
            op = CREATE;
            p += 6;
        }
        else if (strncmp(buffer, "READ", 4) == 0)
        {
            op = READ;
            p += 4;
        }
        else if (strncmp(buffer, "READ_ALL", 8) == 0)
        {
            // op = READ_ALL;
            // p += 8;
            item.operation = READ_ALL;
            item.data = NULL;
            item.id = -1;
            return &item;
        }
        else if (strncmp(buffer, "UPDATE", 6) == 0)
        {
            op = UPDATE;
            p += 6;
        }
        else if (strncmp(buffer, "DELETE", 6) == 0)
        {
            op = DELETE;
            p += 6;
        }
    }

    // skip whitespace
    while (*p == ' ')
        p++;

    if (*p == '\0')
    {
        // The only case where this is possible is READ_ALL, but if we reach
        // this point here and not in the operation check, it means that the
        // buffer is malformed.
        return NULL;
    }

    switch (op)
    {
    case READ:
    case DELETE:
        // both operations need only to parse the ID
        item.data = NULL;
        break;
    case CREATE:
    case UPDATE:
        // both operations need to parse the data
        break;
    default:
        // this should never happen
        return NULL;
    }
}

/**
 * @brief Accepts and dispatches a request to a worker thread using the queue
 * @param socket_fd The socket file descriptor to accept the request from
 * @param queue The queue to dispatch the request to
 * @return 0 on success, -1 on failure
 * @note This function is blocking
 */
int dispatch_request(int socket_fd, queue_t *queue)
{
    // Accept the request
    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("accept");
        return -1;
    }

    // Receive the socket data from client_fd
    char *buffer = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
    int bytes_read = read(client_fd, buffer, REQUEST_BUFFER_SIZE - 1);
    // append null terminator
    buffer[bytes_read] = '\0';

    if (bytes_read < 0)
    {
        perror("read");
        return -1;
    }
    else if (bytes_read == 0)
    {
        // Client closed connection
        return 0;
    }

    // Dispatch the request to the queue
    queue_item_t *item = parse_request(buffer);
    queue_enqueue(queue, buffer);
    return 0;
}
