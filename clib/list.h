typedef struct ListNode List;
typedef struct ListNode ListNode;
struct ListNode {
  ListNode *prev, *next;
};

void list_init(List *list) {
  list->prev = list;
  list->next = list;
}

void list_add(List *list, ListNode *new) {
  list->next->prev = new;
  new->next = list->next;
  new->prev = list;
  list->next = new;
}

void list_add_tail(List *list, ListNode *new) {
  list->prev->next = new;
  new->prev = list->prev;
  new->next = list;
  list->prev = new;
}

void list_del(ListNode *node) {
  node->next->prev = node->prev;
  node->prev->next = node->next;
  list_init(node);
}

int list_empty(List *list) {
  return list->next == list;
}

ListNode *list_remove_head(List *list) {
  ListNode *node = NULL;
  if (list_empty(list)) {
     return NULL;
  }
  node = list->next;
  list_del(node);
  return node;
}

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

