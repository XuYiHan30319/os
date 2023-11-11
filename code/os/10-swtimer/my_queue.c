#include "os.h"

queue *init_queue()
{
    queue *head = (queue *)malloc(sizeof(queue));
    queue_node *node = (queue_node *)malloc(sizeof(queue_node));
    node->next = NULL;
    head->tail = head->head = node;
    return head;
}

void push_queue(queue *que, struct context *ctx)
{
    queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
    new_node->next = NULL;
    new_node->ctx = ctx;
    que->tail->next = new_node;
    que->tail = new_node;
}

void pop_queue(queue *que)
{
    if (que->head->next != NULL)
    {
        queue_node *temp = que->head->next;
        if (temp->next == NULL)
        {
            que->tail = que->head;
        }
        que->head->next = temp->next;
        free(temp);
    }
}

struct context *queue_head(queue *head)
{
    if (head->head->next != NULL)
    {
        return head->head->next->ctx;
    }
    return NULL;
}