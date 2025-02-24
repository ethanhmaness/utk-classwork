#include <iostream>
#include <vector>
#include "sorting.hpp"
using namespace std;

/*
  quick_2_sort.cpp
  Ethan Maness
  March 27, 2022
  
  Provides an implementation of quicksort through the sort_doubles method that will use recursive sorting to sort the given vector in O(n log n) time.
  Performance is improved through the use of the median rule for determining the pivot in each call to the recursive sorting function.
*/

void recursive_sort(vector <double> &v, int start, int size, bool print);

void sort_doubles(vector <double> &v, bool print)
{
  size_t j;
  
  recursive_sort(v, 0, v.size(), print);

  if (print) {
    printf("                     ");
    for (j = 0; j < v.size(); j++) printf("%4.2lf ", v[j]);
    cout << endl;
  }

  return;
}

//Performs quicksort on vector v by recursively calling itself, using the median pivot selecting method
void recursive_sort(vector <double> &v, int start, int size, bool print){
  
  //A single element is already sorted
  if(size <= 1) return;
  
  //Starting print call
  if(print){
    printf("S: %5d %5d      ", start, size);
    for(size_t i = 0; i < v.size(); i++){
      printf(" %4.2lf", v[i]);
    }
    printf("\n");
  }
  
  //def vars
  int end = start + size - 1;
  int midpoint = start + size / 2;
  int pivot_index;
  double tmp;
  
  //Simple swap for 2 element group
  if(size == 2){
    if(v[start] > v[end]){
      tmp = v[start];
      v[start] = v[end];
      v[end] = tmp;
    }
    return;
  }
  
  
/*
  Determine pivot
*/
  
  // ??? end ??? start ???
  if(v[start] > v[end]){
  
    // end start mid
    if(v[midpoint] > v[start]){
      pivot_index = start;
    
    // end mid start
    } else if (v[midpoint] > v[end]) {
      pivot_index = midpoint;
    
		// mid end start
		} else {
      pivot_index = end;
    }
    
  // ??? start ??? end ???
  } else {
    
    // mid start end
    if(v[midpoint] < v[start]){
      pivot_index = start;
    
    // start mid end
    } else if (v[midpoint] < v[end]) {
      pivot_index = midpoint;
    
		// start mid end
		} else {
      pivot_index = end;
    }
    
  }
  
  //swap median and start
  tmp = v[start];
  v[start] = v[pivot_index];
  v[pivot_index] = tmp;
  
  //print pivot
  if(print && size > 2){
    printf("M: %5d %5d %5.2lf", start, size, v[start]);
    for(size_t i = 0; i < v.size(); i++){
      printf(" %4.2lf", v[i]);
    }
    printf("\n");
  }
  
/*
  Partition the segment
*/

  int lower = start + 1;
  int higher = end;
  
  //sort partitions
  while(1){
  
	  //slide pointers towards middle until reaching a value in the wrong partition
	  while(v[lower] < v[start] && lower != end) lower++;
	  while(v[higher] > v[start] && higher != start + 1) higher--;
    
    //end if indices swap
    if(lower > higher) break;
    
    if(lower == higher){
      lower++;
		  break;
	  }
    
    //swap elements
	  if(v[lower] > v[start] || v[higher] < v[start]){
      tmp = v[lower];
      v[lower] = v[higher];
      v[higher] = tmp;
    }
    
    //increment and decrement indices accordingly
    lower++;
    higher--;
  
  }
  
  //swap pivot with last element in lower set
  
  tmp = v[higher];
  v[higher] = v[start];
  v[start] = tmp;
  
  if(print && size > 2){
    printf("P: %5d %5d %5d", start, size, higher);
    for(size_t i = 0; i < v.size(); i++){
      printf(" %4.2lf", v[i]);
    }
    printf("\n");
  }
  
  //call recursively on partitions
  
  recursive_sort(v, start, higher - start, print);
  recursive_sort(v, higher + 1, end - higher, print);
  
  return;
  
}