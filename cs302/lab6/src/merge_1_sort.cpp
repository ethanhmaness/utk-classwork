#include <iostream>
#include <vector>
#include "sorting.hpp"
using namespace std;

/*
  merge_1_sort.cpp
  Ethan Maness
  March 27, 2022
  
  Provides an implementation of merge sort through the sort_doubles method that will use recursive sorting to sort the given vector in O(n log n) time.
*/

void recursive_sort(vector <double> &v, vector <double> &temp, int start, int size, bool print);

void sort_doubles(vector <double> &v, bool print)
{
  size_t j;
  
  vector<double> w(v.size(), 0);
  
  recursive_sort(v, w, 0, v.size(), print);

  if (print) {
    printf("                     ");
    for (j = 0; j < v.size(); j++) printf("%4.2lf ", v[j]);
    cout << endl;
  }

  return;
}

//Performs merge sort on the vector v by recursively calling itself
void recursive_sort(vector <double> &v, vector <double> &temp, int start, int size, bool print){
  
  double tmp;
  
  //Single element is sorted
  if(size <= 1) return;
  
  //print starting
  if(print){
    printf("B: %5d %5d      ", start, size);
    for(size_t i = 0; i < v.size(); i++){
      printf(" %4.2lf", v[i]);
    }
    printf("\n");
  }
  
  //No merge sort required for 2 elements
  if(size == 2){
    if(v[start] > v[start+1]){
      tmp = v[start];
      v[start] = v[start+1];
      v[start+1] = tmp;
    }
    
    //print ending
    if(print){
	    printf("E: %5d %5d      ", start, size);
	    for(size_t i = 0; i < v.size(); i++){
	      printf(" %4.2lf", v[i]);
      }
      printf("\n");
    }
    
    return;
  }
  
  //vars
  int left = start;
  int right = start + size / 2;
  int leftend = right - 1;
  int rightend = start + size - 1;
  int index = 0;
  
  //recursively sort halves
  //printf("first recursive_sort(v, temp, %d, %d, print);\n", left, right - left); /////////////////remove
  recursive_sort(v, temp, left, right - left, print);
  //printf("second recursive_sort(v, temp, %d, %d, print);\n", right, start+size-right); /////////////////remove
  recursive_sort(v, temp, right, start + size - right, print);
  
  //printf("merging %d-%d with %d-%d\n", left, leftend, right, rightend); /////////////////remove
  //merge halves into sorted temp vector
  while(left <= leftend || right <= rightend){
    if(right > rightend){
      temp[index] = v[left];
      left++;
    } else if(left > leftend){
      temp[index] = v[right];
      right++;
    } else if(v[left] > v[right]){
      temp[index] = v[right];
      right++;
    } else {
      temp[index] = v[left];
      left++;
    }
    index++;
  }
  
  //overwrite vector with temp
  for(int i = 0; i < size; i++){
    v[start + i] = temp[i];
  }
  
  //print ending
  if(print){
    printf("E: %5d %5d      ", start, size);
    for(size_t i = 0; i < v.size(); i++){
      printf(" %4.2lf", v[i]);
    }
    printf("\n");
  }
  
  return;
  
}