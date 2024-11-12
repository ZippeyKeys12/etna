#include <stdlib.h>

extern void* cn_malloc(size_t size);
extern void cn_free_sized(void *ptr, size_t size);

struct sllist {
  int head;
  struct sllist* tail;
};

/*@
datatype List {
  Nil {},
  Cons {i32 Head, datatype List Tail}
}

predicate (datatype List) SLList_At(pointer p) {
  if (is_null(p)) {
    return Nil{};
  } else {
    take H = Owned<struct sllist>(p);
    take T = SLList_At(H.tail);
    return (Cons { Head: H.head, Tail: T });
  }
}
@*/

/*@
function [rec] (datatype List) Append(datatype List L1, datatype List L2) {
  match L1 {
    Nil {} => {
      L2
    }
    Cons {Head : H, Tail : T}  => {
      //! //
      Cons {Head: H, Tail: Append(T, L2)}
      //!! append_forget_head //
      //! Append(T, L2) //
    }
  }
}
@*/

struct sllist* IntList_append(struct sllist* xs, struct sllist* ys)
/*@ requires take L1 = SLList_At(xs);
             take L2 = SLList_At(ys); @*/
/*@ ensures take L3 = SLList_At(return);
            L3 == Append(L1, L2); @*/
{
  if (xs == 0) {
    /*@ unfold Append(L1, L2); @*/
    return ys;
  } else {
    /*@ unfold Append(L1, L2); @*/
    struct sllist *new_tail = IntList_append(xs->tail, ys);
    xs->tail = new_tail;
    return xs;
  }
}

struct sllist* slnil()
/*@ ensures take Ret = SLList_At(return);
            Ret == Nil{};
 @*/
{
  return 0;
}

struct sllist* slcons(int h, struct sllist* t)
/*@ requires take T = SLList_At(t);
    ensures take Ret = SLList_At(return);
            Ret == Cons{ Head: h, Tail: T};
 @*/
{
  struct sllist *p = cn_malloc(sizeof(struct sllist));
  p->head = h;
  p->tail = t;
  return p;
}


struct sllist* slcopy (struct sllist *l)
/*@ requires take L = SLList_At(l);
    ensures take L_ = SLList_At(l);
            take Ret = SLList_At(return);
            L == L_;
            L == Ret;
@*/
{
  if (l == 0) {
    return slnil();
  } else {
    struct sllist *new_tail = slcopy(l->tail);
    return slcons(l->head, new_tail);
  }
}

void free__rec_sllist(struct sllist* l)
/*@ requires take L = SLList_At(l); @*/
{
  if (l == 0) {
  } else {
    free__rec_sllist(l->tail);
    cn_free_sized(l, sizeof(struct sllist));
  }
}

/*@
function [rec] (u32) Length(datatype List L) {
  match L {
    Nil {} => {
      0u32
    }
    Cons {Head: H, Tail : T}  => {
      1u32 + Length(T)
    }
  }
}
@*/

unsigned int length (struct sllist *l)
/*@ requires take L = SLList_At(l);
    ensures take L_post = SLList_At(l);
            L == L_post;
            return == Length(L);
@*/
{
  if (l == 0) {
    /*@ unfold Length(L); @*/
    return 0;
  } else {
    /*@ unfold Length(L); @*/
    return 1 + length(l->tail);
  }
}

/*@
function [rec] ({datatype List fst, datatype List snd}) 
                 Split (datatype List xs)
{
  match xs {
    Nil {} => {
      {fst: Nil{}, snd: Nil{}}
    }
    Cons {Head: h1, Tail: Nil{}} => {
      {fst: Nil{}, snd: xs}
    }
    Cons {Head: h1, Tail: Cons {Head : h2, Tail : tl2 }} => {
      let P = Split(tl2);
      {fst: Cons { Head: h1, Tail: P.fst},
       snd: Cons { Head: h2, Tail: P.snd}}
    }
  }
}

function [rec] (datatype List) Merge(datatype List xs, datatype List ys) {
  match xs {
      Nil {} => { ys }
      Cons {Head: x, Tail: xs1} => {
        match ys {
          Nil {} => { xs }
          Cons{ Head: y, Tail: ys1} => {
            //! //
            (x < y) ?
              (Cons{ Head: x, Tail: Merge(xs1, ys) })
            : (Cons{ Head: y, Tail: Merge(xs, ys1) })
            //!! merge_swap_cases_spec //
            //! (x < y) ? (Cons{ Head: y, Tail: Merge(xs, ys1) }) : (Cons{ Head: x, Tail: Merge(xs1, ys) }) //
            
          }
        }
      }
  }
}

function [rec] (datatype List) MergeSort(datatype List xs) {
  match xs {
      Nil{} => { xs }
      Cons{Head: x, Tail: Nil{}} => { xs }
      Cons{Head: x, Tail: Cons{Head: y, Tail: zs}} => {
        let P = Split(xs);
        let L1 = MergeSort(P.fst);
        let L2 = MergeSort(P.snd);
        Merge(L1, L2)
      }
    }
}
@*/

struct sllist_pair {
  struct sllist* fst;
  struct sllist* snd;
};

struct sllist_pair split(struct sllist *xs)
/*@ requires take Xs = SLList_At(xs); @*/
/*@ ensures take Ys = SLList_At(return.fst); @*/
/*@ ensures take Zs = SLList_At(return.snd); @*/
/*@ ensures {fst: Ys, snd: Zs} == Split(Xs); @*/
{
  if (xs == 0) {
    /*@ unfold Split(Xs); @*/
    struct sllist_pair r = {.fst = 0, .snd = 0};
    return r;
  } else {
    struct sllist *cdr = xs -> tail;
    if (cdr == 0) {
      /*@ unfold Split(Xs); @*/
      struct sllist_pair r = {.fst = 0, .snd = xs};
      return r;
    } else {
      /*@ unfold Split(Xs); @*/
      struct sllist_pair p = split(cdr->tail);
      xs->tail = p.fst;
      cdr->tail = p.snd;
      struct sllist_pair r = {.fst = xs, .snd = cdr};
      return r;
    }
  }
}

struct sllist* merge(struct sllist *xs, struct sllist *ys)
/*@ requires take Xs = SLList_At(xs); @*/
/*@ requires take Ys = SLList_At(ys); @*/
/*@ ensures take Zs = SLList_At(return); @*/
/*@ ensures Zs == Merge(Xs, Ys); @*/
{
  if (xs == 0) {
    /*@ unfold Merge(Xs, Ys); @*/
    return ys;
  } else {
    /*@ unfold Merge(Xs, Ys); @*/
    if (ys == 0) {
      /*@ unfold Merge(Xs, Ys); @*/
      return xs;
    } else {
      /*@ unfold Merge(Xs, Ys); @*/
      //! //
      if (xs->head < ys->head) {
      //!! merge_swap_cases_impl //
      //! if (xs->head > ys->head) { //
        struct sllist *zs = merge(xs->tail, ys);
        xs->tail = zs;
        return xs;
      } else {
        struct sllist *zs = merge(xs, ys->tail);
        ys->tail = zs;
        return ys;
      }
    }
  }
}

struct sllist* sl_mergesort(struct sllist *xs)
/*@ requires take Xs = SLList_At(xs); @*/
/*@ ensures take Ys = SLList_At(return); @*/
/*@ ensures Ys == MergeSort(Xs); @*/
{
  if (xs == 0) {
    /*@ unfold MergeSort(Xs); @*/
    return xs;
  } else {
    struct sllist *tail = xs->tail;
    if (tail == 0) {
      /*@ unfold MergeSort(Xs); @*/
      return xs;
    } else {
      /*@ unfold MergeSort(Xs); @*/
      struct sllist_pair p = split(xs);
      p.fst = sl_mergesort(p.fst);
      p.snd = sl_mergesort(p.snd);
      return merge(p.fst, p.snd);
    }
  }
}

/*@
function [rec] (datatype List) Snoc(datatype List Xs, i32 Y) {
  match Xs {
    Nil {} => {
      Cons {Head: Y, Tail: Nil{}}
    }
    Cons {Head: X, Tail: Zs}  => {
      //! //
      Cons{Head: X, Tail: Snoc (Zs, Y)}
      //!! snoc_real_bug //
      //! Snoc (RevList(Zs), X) //
    }
  }
}

function [rec] (datatype List) RevList(datatype List L) {
  match L {
    Nil {} => {
      Nil {}
    }
    Cons {Head : H, Tail : T}  => {
      Snoc (RevList(T), H)
    }
  }
}

function (i32) Hd (datatype List xs) {
  match xs {
    Nil {} => {
      0i32
    }
    Cons {Head : H, Tail : _} => {
      H
    }
  }
}

function (datatype List) Tl (datatype List xs) {
  match xs {
    Nil {} => {
      Nil {}
    }
    Cons {Head : _, Tail : Tail} => {
      Tail
    }
  }
}

lemma Append_Nil_RList (datatype List L1)
  requires true;
  ensures Append(L1, Nil{}) == L1;

lemma Append_Cons_RList (datatype List L1, i32 X, datatype List L2)
  requires true;
  ensures Append(L1, Cons {Head: X, Tail: L2})
          == Append(Snoc(L1, X), L2);
@*/

struct sllist* rev_aux(struct sllist* l1, struct sllist* l2)
/*@ requires take L1 = SLList_At(l1); @*/
/*@ requires take L2 = SLList_At(l2); @*/
/*@ ensures take R = SLList_At(return); @*/
/*@ ensures R == Append(RevList(L2), L1); @*/
{
  if (l2 == 0) {
    /*@ unfold RevList(L2); @*/
    /*@ unfold Append(Nil{}, L1); @*/
    return l1;
  } else {
    /*@ unfold RevList(L2); @*/
    /*@ apply Append_Cons_RList(RevList(Tl(L2)), Hd(L2), L1); @*/
    struct sllist *tmp = l2->tail;
    l2->tail = l1;
    return rev_aux(l2, tmp);
  }
}

struct sllist* rev(struct sllist* l1)
/*@ requires take L1 = SLList_At(l1); @*/
/*@ ensures take L1_Rev = SLList_At(return); @*/
/*@ ensures L1_Rev == RevList(L1); @*/
{
  /*@ apply Append_Nil_RList(RevList(L1)); @*/
  return rev_aux (0, l1);
}

struct sllist* rev_loop(struct sllist *l)
/*@ requires take L = SLList_At(l);
    ensures  take L_post = SLList_At(return);
             L_post == RevList(L);
@*/
{
  struct sllist *last = 0;
  struct sllist *cur = l;
  /*@ apply Append_Nil_RList(RevList(L)); @*/
  while(1)
  /*@ inv take Last = SLList_At(last);
          take Cur = SLList_At(cur);
          Append(RevList(Cur), Last) == RevList(L);
  @*/
  {
    if (cur == 0) {
      /*@ unfold RevList(Nil{}); @*/
      /*@ unfold Append(Nil{}, Last); @*/
      return last;
    }
    struct sllist *tmp = cur->tail;
    cur->tail = last;
    //! //
    last = cur;
    cur = tmp;
    //!! rev_loop_cur_to_early //
    //! cur = tmp; last = cur; //
    /*@ unfold RevList(Cur); @*/
    /*@ apply Append_Cons_RList(RevList(Tl(Cur)), Hd(Cur), Last); @*/
  }
}
