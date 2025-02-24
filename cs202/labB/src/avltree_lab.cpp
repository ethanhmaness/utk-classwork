#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "avltree.hpp"
using namespace std;
using CS202::AVLTree;
using CS202::AVLNode;

/* Checks heights of AVLNode children to determine if it has an imbalance */
bool imbalance(const AVLNode *n){
  return (n->right->height + 2 < n->height) || (n->left->height + 2 < n->height);
}

/* Rotates a tree rooted by the given node's parent about the given node */
void rotate(AVLNode *n){
  AVLNode *parent, *grandparent;
  bool rightsiderotation;
  
  /* Define other nodes (exit if n has no parent) */
  
  parent = n->parent;
  if(parent->height == 0) return;
  grandparent = parent->parent;
  
  rightsiderotation = parent->right == n;
  
  /* Replace grandparent's child slot containing parent with n */
  if(grandparent->left == parent) grandparent->left = n;
  else grandparent->right = n;
  n->parent = grandparent;
  parent->parent = n;
  
  /* Swap child subtree parentage */
  
  if(rightsiderotation){
    parent->right = n->left;
    parent->right->parent = parent;
    n->left = parent;
  }
  else {
    parent->left = n->right;
    parent->left->parent = parent;
    n->right = parent;
  }
  
}

/* Makes a node's height be 1 greater than its greatest child's height */
void fix_height(AVLNode *n){
  n->height = (n->left->height > n->right->height) ? n->left->height + 1 : n->right->height + 1;
}

/* Fixes an imbalanced tree through rotations */
void fix_imbalance(AVLNode *n){
  
  /* Do nothing if it is already balanced */
  if(!imbalance(n)){
    return;
  }
  
  size_t h = n->height;
  
  /* Determine specific imbalance case and perform the appropriate rotations */
  if(n->left->height == h-1){
  
    /* Left Zig-Zig */
    if(n->left->left->height == h-2){
      rotate(n->left);
      fix_height(n);
    }
    
    /* Left-Right Zig-Zag */
    else {
      rotate(n->left->right);
      fix_height(n->left->left);
      fix_height(n->left);
      rotate(n->left);
      fix_height(n);
    }
    
  } else {
  
    /* Right Zig-Zig */
    if(n->right->right->height == h-2){
      rotate(n->right);
      fix_height(n);
    }
    
    /* Right-Left Zig-Zag */
    else {
      rotate(n->right->left);
      fix_height(n->right->right);
      fix_height(n->right);
      rotate(n->right);
      fix_height(n);
    }
    
  }
}

/* Copies an AVLTree instance */
AVLTree& AVLTree::operator= (const AVLTree &t)        
{
  Clear();
  
  if(t.sentinel->right == t.sentinel){
    sentinel->right = sentinel;
  } else {
    sentinel->right = recursive_postorder_copy(t.sentinel->right);
    sentinel->right->parent = sentinel;
  }
  
  size = t.size;
  
  return *this;
}

/* Inserts a new node and, if necessary, rebalances along the path to the root */

bool AVLTree::Insert(const string &key, void *val)
{
  AVLNode *parent;
  AVLNode *n;

  parent = sentinel;
  n = sentinel->right;

  /* Find where the key should go.  If you find the key, return false. */

  while (n != sentinel) {
    if (n->key == key) return false;
    parent = n;
    n = (key < n->key) ? n->left : n->right;
  }

  /* At this point, parent is the node that will be the parent of the new node.
     Create the new node, and hook it in. */

  n = new AVLNode;
  n->key = key;
  n->val = val;
  n->parent = parent;
  n->height = 1;
  n->left = sentinel;
  n->right = sentinel;

  /* Use the correct pointer in the parent to point to the new node. */

  if (parent == sentinel) {
    sentinel->right = n;
  } else if (key < parent->key) {
    parent->left = n;
  } else {
    parent->right = n;
  }
  
  /* Traverse path to top of tree, adjusting heights of nodes */
  
  size_t h;
  while(n->parent != sentinel){
    h = n->height + 1;
    n = n->parent;
    if(n->height < h) n->height = h;
    fix_imbalance(n);
  }

  /* Increment the size */

  size++;
  return true;
}
 
/* Deletes a node with a given key and, if necessary, fixes heights and rebalances along the way up to the root */ 
   
bool AVLTree::Delete(const string &key)
{
  AVLNode *n, *parent, *mlc, *rebal;
  string tmpkey;
  void *tmpval;

  /* Try to find the key -- if you can't return false. */

  n = sentinel->right;
  while (n != sentinel && key != n->key) {
    n = (key < n->key) ? n->left : n->right;
  }
  if (n == sentinel) return false;

  /* We go through the three cases for deletion, although it's a little
     different from the canonical explanation. */

  parent = n->parent;

  /* Case 1 - I have no left child.  Replace me with my right child.
     Note that this handles the case of having no children, too. */

  if (n->left == sentinel) {
    if (n == parent->left) {
      parent->left = n->right;
    } else {
      parent->right = n->right;
    }
    if (n->right != sentinel) n->right->parent = parent;
    rebal = n->right;
    delete n;
    size--;

  /* Case 2 - I have no right child.  Replace me with my left child. */

  } else if (n->right == sentinel) {
    if (n == parent->left) {
      parent->left = n->left;
    } else {
      parent->right = n->left;
    }
    n->left->parent = parent;
    rebal = n->left;
    delete n;
    size--;

  /* If I have two children, then find the node "before" me in the tree.
     That node will have no right child, so I can recursively delete it.
     When I'm done, I'll replace the key and val of n with the key and
     val of the deleted node.  You'll note that the recursive call 
     updates the size, so you don't have to do it here. */

  } else {
    for (mlc = n->left; mlc->right != sentinel; mlc = mlc->right) ;
    tmpkey = mlc->key;
    tmpval = mlc->val;
    Delete(tmpkey);
    n->key = tmpkey;
    n->val = tmpval;
    rebal = n;
  }

  /* Go up the tree to the root, fixing heights and rebalancing along the way */
  if(rebal == sentinel) rebal = parent;
  while(rebal != sentinel){
    fix_height(rebal);
    fix_imbalance(rebal);
    rebal = rebal->parent;
  }

  return true;
}

/* creates a vector of all keys in sorted order */
vector <string> AVLTree::Ordered_Keys() const
{
  vector <string> rv;
  
  /* recursive function to push keys in the correct order */
  make_key_vector(sentinel->right, rv);
  
  return rv;
}
    
/* recursively pushes the subtree rooted by n into the vector v in sorted order */
void AVLTree::make_key_vector(const AVLNode *n, vector<string> &v) const
{
  if(n == sentinel) return;
  
  if(n->left != sentinel) make_key_vector(n->left, v);
  v.push_back(n->key);
  if(n->right != sentinel) make_key_vector(n->right, v);
}

/* Returns the height of the tree (the height of the root node) */
size_t AVLTree::Height() const
{
  return sentinel->right->height;
}

/* Makes a copy of the subtree rooted by n. */

AVLNode *AVLTree::recursive_postorder_copy(const AVLNode *n) const
{
  AVLNode* r = new AVLNode();
  
  /* Copy stored info */
  
  r->key = n->key;
  r->val = n->val;
  r->height = n->height;
  
  /* Default values */
  
  r->right = sentinel;
  r->left = sentinel;
  r->parent = sentinel;
  
  /* Recursively copy subtrees */
  
  if(n->left->height != 0){
    r->left = recursive_postorder_copy(n->left);
    r->left->parent = r;
  }
  if(n->right->height != 0){
    r->right = recursive_postorder_copy(n->right);
    r->right->parent = r;
  }
  
  return r;
}
