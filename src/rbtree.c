#include "rbtree.h"

#include <stdlib.h>

// my function prototypes
int rotate_left(rbtree *, node_t *);
int rotate_right(rbtree *, node_t *);
int insert_fixup(rbtree *, node_t *);
int transplant(rbtree *, node_t *, node_t *);
int erase_fixup(rbtree *, node_t *);
void postorder_free(rbtree *, node_t *);
void inorder_arr(const rbtree *, node_t *, size_t *, key_t *, const size_t);

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed

  // make sentinel node of the tree
  t->nil = (node_t *)malloc(sizeof(node_t));

  // initiate nil(sentinel node)
  t->nil->color = RBTREE_BLACK;
  t->nil->key = 0;
  t->nil->parent = t->nil->left = t->nil->right = t->nil; // to make sure T.nil point available address

  // empty node: root is nil(the sentinel node)
  t->root = t->nil;

  return t;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory

  postorder_free(t, t->root); // postorder to prevent free a node before end of using
  free(t->nil); // don't forget to free nil node too!
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

  if (current == t->nil) { // if the tree is empty: the new node is the root
    t->root = newNode;
    // no need to change color: will be changed when fixing
  } else if (key < current->key) {  // append to current node(left)
    current->left = newNode;
  } else {                          // append to current node(right)
    current->right = newNode;
  }
  insert_fixup(t, newNode); // the tree may violate the rule, fix it around the node t

  return newNode; // return pointer to newly added node
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find

  // initialize current node as root
  node_t *current = t->root;

  // look for the node in the tree in while loop, moving down
  while (current != t->nil) {
    if (current->key == key) {        // node found! return current node, loop terminates
      return current;
    } else if (key < current->key) {  // move down(left)
      current = current->left;
    } else {                          // move down(right)
      current = current->right;
    }
  }
  return NULL; // if you cannot find(loop ends not finding), return NULL
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find

  // initialize current node as root
  node_t *current = t->root;

  // if the tree is empty, no node exists, return NULL
  if (current == t->nil) {
    return NULL;
  }

  // going down left(smaller)
  while (current->left != t->nil) {
    current = current->left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find

  // initialize current node as root
  node_t *current = t->root;

  // if the tree is empty, no node exists, return NULL
  if (current == t->nil) {
    return NULL;
  }

  // going down right(larger)
  while (current->right != t->nil) {
    current = current->right;
  }
  return current;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  // remove node p
  node_t *x; // x is p or y's successor, which would occur violation
  node_t *y = p; // y is p' successor
  color_t y_original_color = y->color; // check y's original color

  // when p has 0~1 child, just bring the child into p's position
  // in this case, x will be used as y
  if (p->left == t->nil) {
    x = p->right;
    transplant(t, p, p->right);
  } else if (p->right == t->nil) { // when p has 0~1 child, just bring the child into p's position
    x = p->left;
    transplant(t, p, p->left);

  } else { // when p has 2 children
    // find p's successor y, whose key is minimum in the subtree whose root is p's right child
    y = p->right;
    while (y->left != t->nil) {
      y = y->left;
    }

    y_original_color = y->color; // y changed, update y's original color
    x = y->right; // y's successor is y's right child, b/c y is leftest(has no left child)
    if (y->parent == p) { // if y is p's child, x keeps following y
      x->parent = y;
    } else { // if y isn't p's child, x replaces y and y gets p's right child
      transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    // put y in p's position(y gets p's parent and left child)
    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  free(p); // don't forget to free the node p before all references are gone!
  if (y_original_color == RBTREE_BLACK) { // if y was red, no violation occurs
    erase_fixup(t, x);
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array  

  size_t idx = 0; // to check how many nodes have been pushed into the array(if exceeds n, stop pushing)
  // inorder to make sorted output
  inorder_arr(t, t->root, &idx, arr, n); // get the tree, start node, pointer to idx and arr, arr size

  return 0;
}

// my functions

int rotate_left(rbtree *t, node_t *x) {
  // rotate to left, pivots around x to x's right child
  // assumes x has right child
  // up-down position changes, left-right position does not changes

  // get pointer to x's right child
  node_t *y = x->right;

  // x points y's left child as its right child(the child doesn't know yet)
  x->right = y->left;

  // if the new child exists(is not nil), let it point it's new parent
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  // y points x's parent as its new parent
  y->parent = x->parent;

  if (x->parent == t->nil) { // if x was root, change root to y
    t->root = y;
  } else if (x == x->parent->left) { // x wasn't root: tell x's parent its new (left)child
    x->parent->left = y;
  } else { // x wasn't root: tell x'parent its new (right)child
    x->parent->right = y;
  }
  y->left = x; // put x on y's left(as child)
  x->parent = y; // now x points y as parent

  return 0; // success: return 0(not needed)
};

int rotate_right(rbtree *t, node_t *y) {
  // rotate to right, pivots around y to y's left child
  // assumes y has left child
  // up-down position changes, left-right position does not changes

  // get pointer to y's left child
  node_t *x = y->left;

  // y points x's right child as its left child(the child doesn't know yet)
  y->left = x->right;

  // if the new child exists(not nil), let it point it's new parent
  if (x->right != t->nil) {
    x->right->parent = y;
  }

  // x points y's parent as its new parent
  x->parent = y->parent;

  if (y->parent == t->nil) { // if y was root, change root to x
    t->root = x;
  } else if (y == y->parent->left) { // y wasn't root: tell y's parent its new (left)child
    y->parent->left = x;
  } else { // y wasn't root: tell y'parent its new (right)child
    y->parent->right = x;
  }
  x->right = y; // put y on x's right(as child)
  y->parent = x; // now y points x as parent

  return 0; // success: return 0(not needed)
}

int insert_fixup(rbtree *t, node_t *current) {
  // fixes the tree to satisfy RB properties
  while (current->parent->color == RBTREE_RED) {
    if (current->parent == current->parent->parent->left) { // if parent is left child
      node_t *uncle = current->parent->parent->right; // find uncle
      if (uncle->color == RBTREE_RED) {               // case 1: recolor, update current
        current->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        current->parent->parent->color = RBTREE_RED;
        current = current->parent->parent;
      } else {                                        // case 2, 3
        if (current == current->parent->right) {      // case 2 -> case 3
          current = current->parent;
          rotate_left(t, current);
        }
        current->parent->color = RBTREE_BLACK;        // case 3: recolor, rotate
        current->parent->parent->color = RBTREE_RED;
        rotate_right(t, current->parent->parent);
      }
    } else { // if parent is right child
      node_t *uncle = current->parent->parent->left;
      if (uncle->color == RBTREE_RED) {               // case 1 - right: recolor, update current
        current->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        current->parent->parent->color = RBTREE_RED;
        current = current->parent->parent;
      } else {                                        // case 2, 3
        if (current == current->parent->left) {       // case 2 -> case 3
          current = current->parent;
          rotate_right(t, current);
        }
        current->parent->color = RBTREE_BLACK;        // case 3: recolor, rotate
        current->parent->parent->color = RBTREE_RED;
        rotate_left(t, current->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  return 0; // success: return 0(not needed)
}

int transplant(rbtree *t, node_t *u, node_t* v) {
  // transplant v into position of u
  // do not change u's pointers(u->parent, u->left, u->right)
  if (u->parent == t->nil) {          // if u was root, v is the new root
    t->root = v;
  } else if (u == u->parent->left) {  // u's parent points v as new (left)child
    u->parent->left = v;
  } else {                            // u's parent points v as new (right)child
    u->parent->right = v;
  }
  v->parent = u->parent; // v points u's parent as its new parent
  return 0; // success: return 0(not needed)
}

int erase_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) { // to determine w(and x)
      node_t *w = x->parent->right; // w is x's sibling
      if (w->color == RBTREE_RED) {             // case 1 -> 2, 3, 4
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_left(t, x->parent);
        w = x->parent->right; // w changed, update w
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // case 2
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->right->color == RBTREE_BLACK) {  // case 3
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_right(t, w);
          w = x->parent->right; // w changed, update w
        }
        w->color = x->parent->color;            // case 4
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rotate_left(t, x->parent);
        x = t->root; // end while loop
      }
    } else {
      node_t *w = x->parent->left; // w is x's sibling
      if (w->color == RBTREE_RED) {             // case 1 -> 2, 3, 4
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_right(t, x->parent);
        w = x->parent->left; // w changed, update w
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // case 2
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) {   // case 3
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate_left(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;            // case 4
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rotate_right(t, x->parent);
        x = t->root; // end while loop
      }
    }
  }
  x->color = RBTREE_BLACK;
  return 0;
}

void postorder_free(rbtree *t, node_t *p) {
  // while DFS through the tree, free all the nodes
  // postorder to prevent freeing a node before end of being referenced
  if (p == t->nil) { // if it's the sentinel node, should not free now
    return;
  }
  // no need to check if p's children are nil, b/c fuction will return before freeing if it is
  postorder_free(t, p->left);
  postorder_free(t, p->right);
  free(p); // no more reference(child), free the node
}

void inorder_arr(const rbtree *t, node_t *p, size_t *idx, key_t *arr, const size_t n) {
  // DFS through the tree, push the key of current node to array
  // inorder to make sorted output
  if (p != t->nil && *idx < n) { // to not call the function again after fully filling up the array
    inorder_arr(t, p->left, idx, arr, n);
    if (*idx < n) { // always check idx to prevent change wrong memory(over given array)
      arr[*idx] = p->key; // push the key to array
      (*idx)++; // update idx
    }
    inorder_arr(t, p->right, idx, arr, n);
  }
}