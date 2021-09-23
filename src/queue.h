typedef struct queue {
    int size;
    void *head;
    void *tail;
    int *array;
    // thread related stuff
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} queue_t;

typedef enum {
    CREATE = 0,
    READ = 1,
    READ_ALL = 2,
    UPDATE = 2,
    DELETE = 3,
} operation_t;

typedef struct queue_item {
    char *data;
    operation_t operation;
    int id;
    void *next;
} queue_item_t;


// Language: c
// Path: src/queue.c
queue_t *queue_create();
int queue_destroy(queue_t *queue);
void queue_enqueue(queue_t *queue, int value);
int queue_dequeue(queue_t *queue);
int queue_front(queue_t *queue);
int queue_rear(queue_t *queue);
int queue_size(queue_t *queue);
bool queue_is_empty(queue_t *queue);
int queue_is_full(queue_t *queue);
void queue_print(queue_t *queue);

