#include "stack.h"

void init(struct node * head) {
  head = NULL;
}

struct node * push(struct node * head, int data)  {
  struct node* temp = (struct node*)malloc(sizeof(struct node));
  temp->data = data;
  temp->next = head;
  head = temp;
  return head;
}

struct node * pop(struct node * head, int* data)  {
  struct node * temp = head;
  *data = head->data;
  head = head->next;
  free(temp);
  return head;
}
