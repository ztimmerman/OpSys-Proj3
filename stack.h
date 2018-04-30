#include <stdlib.h>

struct node {
  int data;
  struct node * next;
};

void init(struct node * head);

struct node* push(struct node * head, int data);
struct node* pop(struct node * head, int * element);
