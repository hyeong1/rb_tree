#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  p->root = nil;
  p->root->parent = nil;
  p->nil = nil;

  return p;
}

static void postorder(rbtree * t, node_t * n) {
  if (n == t->nil)
    return;
  postorder(t, n->left);
  postorder(t, n->right);
  free(n);
} 

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  postorder(t, t->root);
  free(t->nil);
  free(t);
}

static void rotate_left(rbtree * t, node_t * n) {
  node_t* c = n->right;
  n->right = c->left;
  if (c->left != t->nil)
    c->left->parent = n;
  c->parent = n->parent;

  if (n->parent == t->nil)
    t->root = c;
  else if (n->parent->left == n)
    n->parent->left = c;
  else
    n->parent->right = c;
  
  c->left = n;
  n->parent = c;
}

static void rotate_right(rbtree * t, node_t * n) {
  node_t* c = n->left;
  n->left = c->right;
  if (c->right != t->nil)
    c->right->parent = n;
  c->parent = n->parent;

  if (n->parent == t->nil)
    t->root = c;
  else if (n->parent->left == n)
    n->parent->left = c;
  else
    n->parent->right = c;
  
  c->right = n;
  n->parent = c;
}

static void insert_fixup(rbtree * t, node_t * n) {
  while (n->parent->color == RBTREE_RED) {
    if (n->parent->parent->left == n->parent) {
      node_t * u = n->parent->parent->right;
      if (u->color == RBTREE_RED) {
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        n = n->parent->parent;
      } else {
        if (n->parent->right == n) {
          n = n->parent;
          rotate_left(t, n);
        }
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        rotate_right(t, n->parent->parent);
      }
    } else {
      node_t * u = n->parent->parent->left;
      if (u->color == RBTREE_RED) {
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        n = n->parent->parent;
      } else {
        if (n->parent->left == n) {
          n = n->parent;
          rotate_right(t, n);
        }  
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        rotate_left(t, n->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t * cur = t->root;
  node_t * parent = t->nil;
  node_t * newNode = (node_t *)calloc(1, sizeof(node_t));
  newNode->key = key;

  while (cur != t->nil) {
    parent = cur;
    if (cur->key <= key)
      cur = cur->right;
    else
      cur = cur->left;
  } 

  newNode->parent = parent;
  if (parent == t->nil)
    t->root = newNode;
  else if (parent->key <= newNode->key)
    parent->right = newNode;
  else
    parent->left = newNode;

  newNode->color = RBTREE_RED;
  newNode->left = t->nil;
  newNode->right = t->nil;

  insert_fixup(t, newNode);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t * cur = t->root;
  while (cur != t->nil) {
    if (cur->key == key)
      return cur;
    else if (cur->key > key)
      cur = cur->left;
    else
      cur = cur->right;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

static void transplant(rbtree * t, node_t * u, node_t * v) {
  // u가 삭제할 노드, v가 대체할 노드 => v가 u 자리로 이동
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

static void delete_fixup(rbtree * t, node_t * n) {
  while (n != t->root && n->color == RBTREE_BLACK) {
    if (n == n->parent->left) {
      node_t * w = n->parent->right;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        n->parent->color = RBTREE_RED;
        rotate_left(t, n->parent);
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        n = n->parent; // n->p를 red_and_black 노드로 만듦
      } else {
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_right(t, w);
          w = n->parent->right;
        }
        w->color = n->parent->color;
        n->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rotate_left(t, n->parent);
        n = t->root;
      }
    } else {
      node_t * w = n->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        n->parent->color = RBTREE_RED;
        rotate_right(t, n->parent);
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        n = n->parent; // n->p를 red_and_black 노드로 만듦
      } else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_left(t, w);
          w = n->parent->left;
        }
        w->color = n->parent->color;
        n->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rotate_right(t, n->parent);
        n = t->root;
      }
    }
  }
  n->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t * p) {
  // TODO: implement erase
  node_t * succ = p;
  node_t * replaceN = NULL;
  color_t succOriginalColor = succ->color;

  if (p->left == t->nil) {
    replaceN = p->right;
    transplant(t, p, replaceN);
  } else if (p->right == t->nil) {
    replaceN = p->left;
    transplant(t, p, replaceN);
  } else { 
    succ = succ->right;
    while (succ->left != t->nil) {
      succ = succ->left;
    }

    succOriginalColor = succ->color;
    replaceN = succ->right;
    if (succ != p->right) 
      transplant(t, succ, succ->right);
    else 
      succ->right->parent = succ;
    transplant(t, p, succ);
    succ->left = p->left;
    p->left->parent = succ;
    succ->color = p->color;
  }

  if (succOriginalColor == RBTREE_BLACK)
    delete_fixup(t, replaceN);
  free(p);
  return 0;
}

static int pos = 0;
static void inorder(const rbtree * t, node_t * c, key_t * arr, const size_t n) {
  if ((pos > n) || (c == t->nil))
    return;
  inorder(t, c->left, arr, n);
  arr[pos++] = c->key;
  inorder(t, c->right, arr, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  pos = 0;
  inorder(t, t->root, arr, n);
  return 0;
}
