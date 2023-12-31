#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lt) (void* key1, void* key2)) {
    TreeMap *map = (TreeMap *)malloc(sizeof(TreeMap));
    if (!map) return NULL;
    map->root = NULL;
    map->current = NULL;
    map->lower_than = lt;
    return map;

}


void insertTreeMap(TreeMap* tree, void* key, void* value) {
    TreeNode *parent = NULL;
    TreeNode *current = tree->root;
    while (current) {
        parent = current;
        if (is_equal(tree, key, current->pair->key)) return; 
        if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    TreeNode *newNode = createTreeNode(key, value);
    newNode->parent = parent;
    if (!parent) {
        tree->root = newNode;
    } else if (tree->lower_than(key, parent->pair->key)) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    tree->current = newNode;
}

TreeNode * minimum(TreeNode * x) {
    while (x && x->left) {
        x = x->left;
    }
    return x;
}

void transplant(TreeMap* tree, TreeNode* u, TreeNode* v) {
    if (u->parent == NULL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

void removeNode(TreeMap * tree, TreeNode* node) {
    if (node->left == NULL) {
        transplant(tree, node, node->right);
    } else if (node->right == NULL) {
        transplant(tree, node, node->left);
    } else {
        TreeNode* y = minimum(node->right);
        if (y->parent != node) {
            transplant(tree, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        transplant(tree, node, y);
        y->left = node->left;
        y->left->parent = y;
    }
    free(node);
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}


Pair* searchTreeMap(TreeMap* tree, void* key) {
    TreeNode *node = tree->root;
    while (node) {
        if (is_equal(tree, key, node->pair->key)) {
            tree->current = node;
            return node->pair;
        } else if (tree->lower_than(key, node->pair->key)) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}


Pair* upperBound(TreeMap* tree, void* key) {
    TreeNode *ub_node = NULL;
    TreeNode *current = tree->root;
    while (current) {
        if (is_equal(tree, key, current->pair->key)) {
            return current->pair;
        } else if (tree->lower_than(key, current->pair->key)) {
            ub_node = current;
            current = current->left;
        } else {
            current = current->right;
        }
    }
  return ub_node ? ub_node->pair : NULL;
  return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    TreeNode *minNode = minimum(tree->root);
    if (minNode) {
        tree->current = minNode;
        return minNode->pair;
    }
    return NULL;
}

Pair * nextTreeMap(TreeMap * tree) {
  if (!tree->current) return NULL;
  if (tree->current->right) {
      TreeNode *nextNode = minimum(tree->current->right);
      tree->current = nextNode;
      return nextNode->pair;
  }
  TreeNode *parent = tree->current->parent;
  while (parent && tree->current == parent->right) {
       tree->current = parent;
      parent = parent->parent;
  }
  tree->current = parent;
  return parent ? parent->pair : NULL;
  return NULL;
}
