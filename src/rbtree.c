#include "rbtree.h"

#include <stdlib.h>

#define SENTINEL

// function prototype
int rotate_left(rbtree *, node_t *);
int rotate_right(rbtree *, node_t *);
int insert_fixup(rbtree *, node_t *);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  // Done!

  // make sentinel node of the tree
  p->nil = (node_t *)malloc(sizeof(node_t));

  // initiate nil(sentinel node)
  p->nil->color = RBTREE_BLACK;
  p->nil->key = p->nil->parent = p->nil->left = p->nil->right = NULL;

  // empty node: root is nil(the sentinel node)
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert

  // make and initialize new node with key
  node_t *newNode = (node_t*)malloc(sizeof(node_t));
  newNode->color = RBTREE_RED;
  newNode->key = key;
  newNode->left = newNode->right = t->nil;

  // initialize current and next node pointer
  node_t *current = t->nil;
  node_t *next = t->root;

  // find newNode's parent
  while (next != t->nil) {
    current = next;
    if (key < next->key) {
      next = next->left;
    } else {
      next = next->right;
    }
  }
  newNode->parent = current;

  if (current = t->nil) { // if the tree is empty: the new node is the root
    t->root = newNode;
  } else if (key < current->key) {  // append to current node(left)
    current->left = newNode;
  } else {                          // append to current node(right)
    current->right = newNode;
  }

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

// my functions

int rotate_left(rbtree *t, node_t *x) {
  // rotate to left, pivots around x to x's right child
  // assumes x has right child

  // get pointer to x's right child
  node_t *y = x->right;

  // x takes y's left child as its right child(the child doesn't know yet)
  x->right = y->left;

  // if the new child exist(not nil), let it know it's new parent
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  // y takes x's parent as its new parent
  y->parent = x->parent;

  if (x->parent == t->nil) { // if x was root, change root to y
    t->root = y;
  } else if (x == x->parent->left) { // x wasn't root: tell x'parent its new child(left)
    x->parent->left = y;
  } else { // x wasn't root: tell x'parent its new child(right)
    x->parent->right = y;
  }
  y->left = x; // put x on y's left
  x->parent = y; // now x have y as parent

  return 0;
};

int rotate_right(rbtree *t, node_t *y) {
  // rotate to right, pivots around y to y's left child
  // assumes y has left child

  // get pointer to y's left child
  node_t *x = y->right;

  // y takes x's right child as its left child(the child doesn't know yet)
  y->left = x->right;

  // if the new child exist(not nil), let it know it's new parent
  if (x->right != t->nil) {
    x->right->parent = y;
  }

  // x takes y's parent as its new parent
  x->parent = y->parent;

  if (y->parent == t->nil) { // if y was root, change root to x
    t->root = x;
  } else if (y == y->parent->left) { // y wasn't root: tell y'parent its new child(left)
    y->parent->left = x;
  } else { // y wasn't root: tell y'parent its new child(right)
    y->parent->right = x;
  }
  x->right = y; // put y on x's right
  y->parent = x; // now y have x as parent

  return 0;
}

int insert_fixup(rbtree *t, node_t *z) {
  // fixes the tree to satisfy RB properties
  return 0;
}