#include <stdio.h>
#include <stdlib.h>
#include "sq.h"


typedef struct node{
  int val;
  int inUse;
  struct node* next;
  struct node* prev;
} NODE;

struct service_queue{
  int size;
  NODE* head;
  NODE* tail;
  NODE* reuse;
  NODE** bucket;
  int bucketsize;
  int buzzerAvail;
};

SQ *sq_create(){
  SQ* q;
  q = malloc(sizeof(SQ));
  q->size = 0;
  q->head = NULL;
  q->tail = NULL;
  q->bucket = (NODE**)malloc(sizeof(NODE*));

  NODE *n = (NODE*)malloc(sizeof(NODE));
  n->next = NULL;
  n->prev = NULL;
  n->val = 0;
  n->inUse = 0;

  q->reuse = n;
  q->bucket[0] = n;
  q->bucketsize = 1;
  q->buzzerAvail = 1;
  
  return q;
}

void sq_free(SQ *q)
{
  for (int i = 0; i < q->bucketsize; i++)
    free(q->bucket[i]);
  free(q->bucket);
  free(q);
}

extern void sq_display(SQ *q)
{
  NODE* p = q->head;
  while(p!=NULL)
  {
    printf("%d", p->val);
    p = p->next;
  }
  printf("\n");
}

extern int sq_length(SQ *q)
{
  return q->size;
}

extern int sq_give_buzzer(SQ *q)
{
  if(q->reuse != NULL)
  {
    int buzz = q->reuse->val;
    NODE* newn = q->reuse;
    
    if(q->reuse->prev != NULL)
    {
      q->reuse = q->reuse->prev;
      q->reuse->next = NULL;
    }
    else
    {
      q->reuse = NULL;
    }
    q->bucket[buzz]->inUse = 1;
    newn->next = NULL;
    newn->prev = NULL;

    if(q->head == NULL)
    {
      q->head = newn;
      q->tail = newn;
    }
    else
    {
      q->tail->next = newn;
      newn->prev = q->tail;
      q->tail = newn;
    }
    q->size++;
    return buzz;
  }
  
  else
  {
    q->bucketsize++;
    NODE** temp = (NODE**)malloc(q->bucketsize*2*sizeof(NODE*));
    for (int i=0; i < q->bucketsize-1; i++)
    {
      temp[i] = q->bucket[i];
    }
    temp[q->bucketsize-1] = (NODE*)malloc(sizeof(NODE));
    temp[q->bucketsize-1]->val = q->bucketsize-1;
    temp[q->bucketsize-1]->inUse = 1;
    free(q->bucket);
    q->bucket = temp;

    int buzz = q->bucketsize-1;
    NODE* newn = q->bucket[q->bucketsize-1];
    newn->next = NULL;
    newn->prev = NULL;
    if(q->head == NULL)
    {
      q->head = newn;
      q->tail = newn;
    }
    else
    {
      q->tail->next = newn;
      newn->prev = q->tail;
      q->tail = newn;
    }
    q->size++;
    return buzz;
  }
}


extern int sq_seat(SQ *q)
{
  if(q->size == 0)
    return -1;
  else
  {
    int ID = q->head->val;
    q->bucket[ID]->inUse = 0;
    if(q->head->next == NULL)
    {
      q->head = NULL;
      q->tail = NULL;
    }
    else
    {
      q->head = q->head->next;
      q->head->prev = NULL;
    }

    NODE* newn = q->bucket[ID];
    newn->next = NULL;
    newn->prev = NULL;
    if(q->reuse != NULL)
    {
      q->reuse->next = newn;
      newn->prev = q->reuse;
      q->reuse = newn;
    }
    else
    {
      q->reuse = newn;
    }
    q->size--;
    return ID;
  }
}

extern int sq_kick_out(SQ *q, int buzzer)
{
  if(buzzer >= q->bucketsize || q->bucket[buzzer]->inUse == 0)
  {
    return 0;
  }

  if(q->head->next == NULL)
  {
    q->head = NULL;
    q->tail = NULL;
  }
  else if(q->bucket[buzzer] == q->head) // to do: if its the only one in the list
  {
    q->head = q->head->next;
    q->head->prev = NULL;
  }
  else if(q->bucket[buzzer] == q->tail)
  {
    q->tail = q->tail->prev;
    q->tail->next = NULL;
  }
  else
  {
    NODE* NewNext = q->bucket[buzzer]->next;
    NODE* NewPrev = q->bucket[buzzer]->prev;
    NewPrev->next = NewNext;
    NewNext->prev = NewPrev;
  }
  q->bucket[buzzer]->inUse = 0;

  NODE* newn = q->bucket[buzzer];
  newn->next = NULL;
  newn->prev = NULL;
  if(q->reuse != NULL)
  {
    q->reuse->next = newn;
    newn->prev = q->reuse;
    q->reuse = newn;
  }
  else
  {
    q->reuse = newn;
  }
  q->size--;
  return 1;
}

extern int sq_take_bribe(SQ *q, int buzzer)
{
  if(buzzer >= q->bucketsize || q->bucket[buzzer]->inUse == 0)
  {
    return 0;
  }
  if(q->bucket[buzzer] == q->head)
  {
    return 1;
  }
  else if(q->bucket[buzzer] == q->tail)
  {
    NODE* newn = q->tail;
    q->tail = q->tail->prev;
    q->tail->next = NULL;
    q->head->prev = newn;
    newn->next = q->head;
    newn->prev = NULL;
    q->head = newn;
  }
  else
  {
    NODE* cur = q->bucket[buzzer];
    NODE* NewNext = q->bucket[buzzer]->next;
    NODE* NewPrev = q->bucket[buzzer]->prev;
    NewPrev->next = NewNext;
    NewNext->prev = NewPrev;
    q->head->prev = cur;
    cur->next = q->head;
    cur->prev = NULL;
    q->head = cur;
  }
  return 1;
}