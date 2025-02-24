#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include "bstree.hpp"
using namespace std;
using CS202::BSTree;
using CS202::BSTNode;

/*
  bstree_lab.cpp
  Ethan Maness
  November 11, 2021
  
  Completes implementation of the BSTree class, allowing for the building, modifying, balancing, printing, and copying of binary search trees with keys of type std::string and vals of type void*.
*/

//returns the depth of the node with the associated key, or -1 if no node has such a key
int BSTree::Depth(const string &key) const
{
  //start at root
  BSTNode* n = sentinel->right;
  //if empty tree, key is not present
  if(n == sentinel) return -1;
  
  //root has depth 0
  int d = 0;
  //navigate the tree
  while(1){
    //if key not present
    if(n == sentinel) return -1;
    //if key found return depth
    if(n->key == key) return d;
    //otherwise, increment depth and go down
    d++;
    //go left or right depending on value comparison
    if(key < n->key) n = n->left;
    else n = n->right;
  }
}

//returns the height of the tree
int BSTree::Height() const
{
  
  //recursively find height, starting at root
  return recursive_find_height(sentinel->right);
}

//creates a vector of all keys in sorted order
vector <string> BSTree::Ordered_Keys() const
{
  vector <string> rv;
  
  //recursive function to push keys in the correct order
  make_key_vector(sentinel->right, rv);
  
  return rv;
}
    
//initializes sentinel, then uses assignment overload to copy data from given tree
BSTree::BSTree(const BSTree &t)        
{
  
  size = 0;
  
  //create sentinel
  sentinel = new BSTNode();
  sentinel->key = "--SENTINEL--";
  sentinel->right = sentinel;
  
  //use assignment overload to copy tree
  *this = t;
  
}

//copies the keys and vals of the given tree into vectors, then remakes a new sorted tree from the vectors
BSTree& BSTree::operator= (const BSTree &t) 
{

  //empty tree
  Clear();
  
  //make the vectors
  vector<string> keyVec = t.Ordered_Keys();
  vector<void *> valVec = t.Ordered_Vals();
    
  //convert back into a tree
  BSTNode* root = make_balanced_tree(keyVec, valVec, 0, keyVec.size());
  
  //store in this BSTree instance
  sentinel->right = root;
  size = t.size;
  
  return *this;
  
}

//recursively determines the height of the given node by executing on any children nodes
int BSTree::recursive_find_height(const BSTNode *n) const
{
  
  //if empty, height is 0
  if(n == sentinel) return 0;

  //if no children, height is one
  if(n->left == sentinel && n->right == sentinel) return 1;

  //determine children heights
  int lh = recursive_find_height(n->left);
  int rh = recursive_find_height(n->right);
  
  //height is max child height plus one
  if(lh > rh) return lh + 1;
  return rh + 1;
}

//recursively pushes the subtree rooted by n into the vector v in sorted order
void BSTree::make_key_vector(const BSTNode *n, vector<string> &v) const
{
  if(n == sentinel) return;
  
  if(n->left != sentinel) make_key_vector(n->left, v);
  v.push_back(n->key);
  if(n->right != sentinel) make_key_vector(n->right, v);
}

//recursively creates a balanced tree given vectors of keys and vals (must be same size, will be paired by index), starting at the given index and including num_indices elements
BSTNode *BSTree::make_balanced_tree(const vector<string> &sorted_keys, 
                            const vector<void *> &vals,
                            size_t first_index,
                            size_t num_indices) const
{
  
  //if no info, no tree
  if(num_indices == 0) return sentinel;
  
  //create new BSTNode
  BSTNode* root = new BSTNode();
  root->left = sentinel;
  root->right = sentinel;
  root->parent = sentinel;
  
  //note: center of an even size will be biased to the greater index
  size_t center = first_index + (num_indices / 2);
  size_t last_index = first_index + num_indices - 1;
  
  //set appropriate key and val
  root->key = sorted_keys.at(center);
  root->val = vals.at(center);
  
  //if there are indices before center, make them a balanced tree as the left child of this node
  if(center != first_index){
    root->left = make_balanced_tree(sorted_keys, vals, first_index, center - first_index);
    root->left->parent = root;
  }
  
  //if there are indices after center, make them a balanced tree as the right child of this node
  if(center != last_index){
    root->right = make_balanced_tree(sorted_keys, vals, center + 1, last_index - center);
    root->right->parent = root;  
  }
  
  return root;
  
}
