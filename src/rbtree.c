#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  return p;
}

void delete_node(rbtree *t, node_t *node) {
  if(node == t->nil) {
    return;
  }
  delete_node(t, node->left);
  delete_node(t, node->right);
  free(node);
  node = NULL;
}

void delete_rbtree(rbtree *t) {
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *node) {
  node_t *rightNode = node->right;
  node->right = rightNode->left;
  if(rightNode->left != t->nil){
    rightNode->left->parent = node;
  }
  rightNode->parent = node->parent;
  if(node->parent == t->nil){
    t->root = rightNode;
  } else if(node == node->parent->left) {
    node->parent->left = rightNode;
  } else {
    node->parent->right = rightNode;
  }
  rightNode->left = node;
  node->parent = rightNode;
}

void right_rotate(rbtree *t, node_t *node) {
  node_t *leftNode = node->left;
  node->left = leftNode->right;
  if(leftNode->right != t->nil){
    leftNode->right->parent = node;
  }
  leftNode->parent = node->parent;
  if(node->parent == t->nil){
    t->root = leftNode;
  } else if(node == node->parent->left) {
    node->parent->left = leftNode;
  } else {
    node->parent->right = leftNode;
  }
  leftNode->right = node;
  node->parent = leftNode;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *endNode = t->root;
  node_t *parentNode = t->nil;
  node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
  newNode->key = key;

  while(endNode != t->nil) {
    parentNode = endNode;
    if(key < endNode->key) {
      endNode = endNode->left;
    } else {
      endNode = endNode->right;
    }
  }
  newNode->parent = parentNode;

  if(parentNode == t->nil) {
    t->root = newNode;
  } else if(key < parentNode->key) {
    parentNode->left = newNode;
  } else {
    parentNode->right = newNode;
  }
  newNode->left = t->nil;
  newNode->right = t->nil;
  newNode->color = RBTREE_RED;

  rbtree_insert_fixup(t, newNode);
  return t->root;
}

void rbtree_insert_fixup(rbtree *t, node_t *curNode) {
  while(curNode->parent->color == RBTREE_RED) {
    if(curNode->parent == curNode->parent->parent->left) {
      node_t *uncleNode = curNode->parent->parent->right;
      if(uncleNode->color == RBTREE_RED) { // case 1
        curNode->parent->color = RBTREE_BLACK;
        uncleNode->color = RBTREE_BLACK;
        curNode->parent->parent->color = RBTREE_RED;
        curNode = curNode->parent->parent;
      } else {
        if(curNode == curNode->parent->right) { // case 2
          curNode = curNode->parent;
          left_rotate(t, curNode);
        }
        curNode->parent->color = RBTREE_BLACK; // case 3
        curNode->parent->parent->color = RBTREE_RED;
        right_rotate(t, curNode->parent->parent);
      }
    } else {
      node_t *uncleNode = curNode->parent->parent->left;
      if(uncleNode->color == RBTREE_RED) {
        curNode->parent->color = RBTREE_BLACK;
        uncleNode->color = RBTREE_BLACK;
        curNode->parent->parent->color = RBTREE_RED;
        curNode = curNode->parent->parent;
      } else{
        if(curNode == curNode->parent->left) {
          curNode = curNode->parent;
          right_rotate(t, curNode);
        }
        curNode->parent->color = RBTREE_BLACK;
        curNode->parent->parent->color = RBTREE_RED;
        left_rotate(t, curNode->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curNode = t->root;
  while(curNode != t->nil) {
    if(key == curNode->key) {
      return curNode;
    } else if(key < curNode->key) {
      curNode = curNode->left;
    } else {
      curNode = curNode->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *minNode = t->nil;
  node_t *curNode = t->root;
  while(curNode != t->nil) {
    minNode = curNode;
    curNode = curNode->left;
  }
  return minNode;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *maxNode = t->nil;
  node_t *curNode = t->root;
  while(curNode != t->nil) {
    maxNode = curNode;
    curNode = curNode->right;
  }
  return maxNode;
}

void rbtree_transplant(rbtree *t, node_t *oldNode, node_t *newNode) {
  if(oldNode->parent == t->nil) {
    t->root = newNode;
  } else if(oldNode == oldNode->parent->left) {
    oldNode->parent->left = newNode;
  } else {
    oldNode->parent->right = newNode;
  }
  newNode->parent = oldNode->parent;
}

node_t *find_successor(rbtree *t, node_t *node) {
  node_t *successor = node;
  while(successor->left != t->nil) {
    successor = successor->left;
  }
  return successor;
}

void rbtree_erase_fixup(rbtree *t, node_t *curNode) {
  while(curNode != t->root && curNode->color == RBTREE_BLACK) {
    if(curNode == curNode->parent->left) {
      node_t *siblingNode = curNode->parent->right;
      if(siblingNode->color == RBTREE_RED) {
        siblingNode->color = RBTREE_BLACK;
        curNode->parent->color = RBTREE_RED;
        left_rotate(t, curNode->parent);
        siblingNode = curNode->parent->right;
      }
      if(siblingNode->left->color == RBTREE_BLACK && siblingNode->right->color == RBTREE_BLACK) {
        siblingNode->color = RBTREE_RED;
        curNode = curNode->parent;
      }
      else {
        if(siblingNode->right->color == RBTREE_BLACK) {
          siblingNode->left->color = RBTREE_BLACK;
          siblingNode->color = RBTREE_RED;
          right_rotate(t, siblingNode);
          siblingNode = curNode->parent->right;
        }
        siblingNode->color = curNode->parent->color;
        curNode->parent->color = RBTREE_BLACK;
        siblingNode->right->color = RBTREE_BLACK;
        left_rotate(t, curNode->parent);
        curNode = t->root;
      }
    } else {
      node_t *siblingNode = curNode->parent->left;
      if(siblingNode->color == RBTREE_RED) {
        siblingNode->color = RBTREE_BLACK;
        curNode->parent->color = RBTREE_RED;
        right_rotate(t, curNode->parent);
        siblingNode = curNode->parent->left;
      }
      if(siblingNode->right->color == RBTREE_BLACK && siblingNode->left->color == RBTREE_BLACK) {
        siblingNode->color = RBTREE_RED;
        curNode = curNode->parent;
      }
      else {
        if(siblingNode->left->color == RBTREE_BLACK) {
          siblingNode->right->color = RBTREE_BLACK;
          siblingNode->color = RBTREE_RED;
          left_rotate(t, siblingNode);
          siblingNode = curNode->parent->left;
        }
        siblingNode->color = curNode->parent->color;
        curNode->parent->color = RBTREE_BLACK;
        siblingNode->left->color = RBTREE_BLACK;
        right_rotate(t, curNode->parent);
        curNode = t->root;
      }
    }
  }
  curNode->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *replaceNode = p;
  color_t originalColor = replaceNode->color;
  node_t *fixupNode;

  if(p->left == t->nil) {
    fixupNode = p->right;
    rbtree_transplant(t, p, p->right);
  } else if(p->right == t->nil) {
    fixupNode = p->left;
    rbtree_transplant(t, p, p->left);
  } else {
    replaceNode = find_successor(t, p->right);
    originalColor = replaceNode->color;
    fixupNode = replaceNode->right;
    if(replaceNode != p->right) {
      rbtree_transplant(t, replaceNode, replaceNode->right);
      replaceNode->right = p->right;
      replaceNode->right->parent = replaceNode;
    } else {
      fixupNode->parent = replaceNode;
    }
    rbtree_transplant(t, p, replaceNode);
    replaceNode->left = p->left;
    replaceNode->left->parent = replaceNode;
    replaceNode->color = p->color;
  }
  if(originalColor == RBTREE_BLACK) {
    rbtree_erase_fixup(t, fixupNode);
  }
  free(p);
  p = NULL;
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *curNode = t->root;
  node_t *prevNode = t->nil;
  node_t *tempNode = t->nil;
  int index = 0;

  while(curNode != t->nil) {
    if(curNode->left != t->nil) {
      prevNode = curNode->left;
      while(prevNode->right != t->nil) {
        prevNode = prevNode->right;
      }
      prevNode->right = curNode;
      tempNode = curNode;
      curNode = curNode->left;
      tempNode->left = t->nil;
    } else {
      arr[index++] = curNode->key;
      if(index >= n) {
        break;
      }
      curNode = curNode->right;
    }
  }
  return 0;
}