#include "board.h"



// ����ť �ʱ�ȭ �Լ�
void init_queue(QueueType *q)
{
    q->front = q->rear = 0;    
}

// ������� ���� �Լ�
int is_empty(QueueType *q)
{
    return (q->front == q->rear);
}

// ��ȭ���� ���� �Լ�
int is_full(QueueType *q)
{
    return ((q->rear+1)%MAX_QUEUE_SIZE == q->front);
}

// ���� �Լ�
QueueStatus enqueue(QueueType *q, unsigned char item)
{ 
    if( is_full(q) ) 
        return QUEUE_ERROR;
	q->rear = (q->rear+1) % MAX_QUEUE_SIZE;
	q->queue[q->rear] = item;
    
    return QUEUE_SUCCESS;    
}

// ���� �Լ�
QueueStatus dequeue(QueueType *q, unsigned char *item) 
{ 
    if( is_empty(q) )
        return QUEUE_ERROR;
    q->front = (q->front+1) % MAX_QUEUE_SIZE;
    *item = q->queue[q->front];
    
    return QUEUE_SUCCESS;
}
