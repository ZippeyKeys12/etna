

struct List
{
  int value;
  struct List* next;
};

/*@
datatype IntList {
  Nil {},
  Cons { i32 head, IntList tail }
}

function (boolean) validCons(i32 head, IntList tail) {
  match tail {
    Nil {} => { true }
    Cons { head: next, tail: _ } => { head <= next }
  }
}

predicate IntList ListSegment(pointer from, pointer to) {
  if (ptr_eq(from,to)) {
    return Nil {};
  } else {
    take head = Owned<struct List>(from);
    take tail = ListSegment(head.next, to);
    assert(validCons(head.value,tail));
    return Cons { head: head.value, tail: tail };
  }
}
@*/

int sum(struct List* xs)
/*@
  requires
    take l1 = ListSegment(xs,NULL);
  ensures
    take l2 = ListSegment(xs,NULL);
    l1 == l2;
    true;
@*/
{
  int result = 0;
  while(xs) {
    result += xs->value;
    //! //
    //!! sum_modify_list //
    //! xs->value = 7; //
    xs = xs->next;
  }
  return result;
}

void *cn_malloc(unsigned long size);

//! //
//!! cons_invalid //
//! void cons(int x, struct List** xs) /*@ requires take list_ptr = Owned<struct List*>(xs); take list = ListSegment(list_ptr,NULL); ensures take new_list_ptr = Owned<struct List*>(xs); take new_list = ListSegment(new_list_ptr,NULL); @*/{struct List *node = (struct List*) cn_malloc(sizeof(struct List));node->value = x;node->next = *xs;*xs = node;} //

/*@
function [rec] (IntList) insertList(boolean dups, i32 x, IntList xs) {
  match xs {
    Nil {} => { Cons { head: x, tail: Nil {} } }
    Cons { head: head, tail: tail } => {
      if (head < x) {
        Cons { head: head, tail: insertList(dups, x,tail) }
      } else {
        if (!dups && head == x) {
          xs
        } else {
          Cons { head: x, tail: xs }
        }
      }
    }
  }
}
@*/

void insert(int x, struct List **xs)
/*@
  requires
    take list_ptr = Owned<struct List*>(xs);
    take list = ListSegment(list_ptr,NULL);
  ensures
    take new_list_ptr = Owned<struct List*>(xs);
    take new_list = ListSegment(new_list_ptr,NULL);
    //! //
    new_list == insertList(true,x,list);
    //!! insert_dups //
    //! new_list == insertList(false,x,list); //
@*/
{
  struct List *node = (struct List*) cn_malloc(sizeof(struct List));
  node->value = x;

  struct List* prev = 0;
  struct List* cur = *xs;
  while (cur && cur->value < x) {
    prev = cur;
    cur = cur->next;
  }

  if (prev) {
    prev->next = node;
    node->next = cur;
  } else {
    node->next = *xs;
    *xs = node;
  }
}
