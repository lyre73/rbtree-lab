#include "rbtree.h"

#include <stdlib.h>

// my function prototypes
int rotate_left(rbtree *, node_t *);
int rotate_right(rbtree *, node_t *);
int insert_fixup(rbtree *, node_t *);
node_t *subtree_min(rbtree *, node_t *);
node_t *subtree_max(rbtree *, node_t *);
int transplant(rbtree *, node_t *, node_t *);
int erase_fixup(rbtree *, node_t *);

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed

  // make sentinel node of the tree
  t->nil = (node_t *)malloc(sizeof(node_t));

  // initiate nil(sentinel node)
  t->nil->color = RBTREE_BLACK;
  t->nil->key = 0;
  t->nil->parent = t->nil->left = t->nil->right = NULL;

  // empty node: root is nil(the sentinel node)
  t->root = t->nil;

  return t;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  // Done!

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

  if (current == t->nil) { // if the tree is empty: the new node is the root
    t->root = newNode;
    // no need to change color: will be changed when fixing
  } else if (key < current->key) {  // append to current node(left)
    current->left = newNode;
  } else {                          // append to current node(right)
    current->right = newNode;
  }
  insert_fixup(t, newNode);

  return newNode; // return pointer to newly added node
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  // Done!

  node_t *current = t->root;
  while (current != t->nil) {
    if (current->key == key) {
      return current;
    } else if (key < current->key) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return NULL;
  // return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *current = t->root;
  while (current != t->nil) {
    if (current->left == t->nil) {
      return current;
    }
    current = current->left;
  }
  return NULL;
  // return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *current = t->root;
  while (current != t->nil) {
    if (current->right == t->nil) {
      return current;
    }
    current = current->right;
  }
  return NULL;
  // return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  
  // remove node p
  node_t *x;
  node_t *y = p;
  color_t y_original_color = y->color;
  if (p->left == t->nil) {
    x = p->right;
    transplant(t, p, p->right);
  } else if (p->right == t->nil) {
    x = p->left;
    transplant(t, p, p->left);
  } else {
    y = subtree_min(t, p->right);
    x = y->right;
    if (y->parent == p) {
      x->parent = y;
    } else {
      transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  free(p);                            // why?????
  if (y_original_color == RBTREE_BLACK) {
    erase_fixup(t, x);
  }
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
  // Done!

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
  // Done!

  // get pointer to y's left child
  node_t *x = y->left;

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

int insert_fixup(rbtree *t, node_t *current) {
  // fixes the tree to satisfy RB properties
  while (current->parent->color == RBTREE_RED) {
    if (current->parent == current->parent->parent->left) { // if parent is left child
      node_t *uncle = current->parent->parent->right;
      if (uncle->color == RBTREE_RED) {               // case 1
        current->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        current->parent->parent->color = RBTREE_RED;
        current = current->parent->parent;
      } else {                                        // case 2, 3
        if (current == current->parent->right) {      // case 2 -> case 3
          current = current->parent;
          rotate_left(t, current);
        }
        current->parent->color = RBTREE_BLACK;        // case 3
        current->parent->parent->color = RBTREE_RED;
        rotate_right(t, current->parent->parent);
      }
    } else { // if parent is right child - unchecked
      node_t *uncle = current->parent->parent->left;
      if (uncle->color == RBTREE_RED) {               // case 1 - right
        current->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        current->parent->parent->color = RBTREE_RED;
        current = current->parent->parent;
      } else {                                        // case 2, 3
        if (current == current->parent->left) {       // case 2 -> case 3
          current = current->parent;
          rotate_right(t, current);
        }
        current->parent->color = RBTREE_BLACK;        // case 3
        current->parent->parent->color = RBTREE_RED;
        rotate_left(t, current->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  return 0;
}

node_t *subtree_min(rbtree *t, node_t *current) {
  while (current != t->nil) {
    if (current->left == t->nil) {
      return current;
    }
    current = current->left;
  }
  return NULL;
}

int transplant(rbtree *t, node_t *u, node_t* v) {
  // transplant v into position of u
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
  return 0;
}

int erase_fixup(rbtree *t, node_t *x) {
  while (x != t->nil && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED) {
        w->color == RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_left(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_right(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rotate_left(t, x->parent);
        x = t->root;
      }
    } else {
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color == RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_right(t, x->parent);
        w = x->parent->left;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_left(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rotate_right(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
  return 0;
}