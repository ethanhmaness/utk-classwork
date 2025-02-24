#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <random>
using namespace std;

// Get vector of ints from file containing newline-separated integers
vector<int> readVals(ifstream &input_file)
{
    string line;
    vector<int> vals;
    while(getline(input_file, line))
    {
        int val;
        try
        {
            val = stoi(line);
        }
        catch(const invalid_argument& e)
        {
            printf("Invalid input format: %s\n", line);
        }
        vals.push_back(val);
    }
    return vals;
}

string vector_to_string(const vector<int> &vec, size_t start, size_t end, string sep = " ")
{
    string s = "";
    for (size_t i = start; i < end; i++)
    {
        s += to_string(vec[i]);
        if (i != end - 1)
            s += sep;
    }
    return s;
}

string vector_to_string(const vector<int> &vec, string sep = " ")
{
    return vector_to_string(vec, 0, vec.size(), sep);
}

string vector_vector_to_string(const vector<vector<int>> &vecvec, string rowsep = "\n", string colsep = " ")
{
    string s = "";
    for (size_t i = 0; i < vecvec.size(); i++)
    {
        s += vector_to_string(vecvec[i], colsep);
        if (i != vecvec.size() - 1)
            s += rowsep;
    }
    return s;
}

// Return the median of a vector (the lesser median if length is even)
int median(const vector<int> &vec)
{
    int median;
    if (vec.size() == 1)
        median = vec[0];
    else
    {
        size_t n = (vec.size()-1) / 2;
        vector<int> copy(vec);
        nth_element(copy.begin(), copy.begin() + n, copy.end());
        median = copy[n];
    }
    //printf("Median of %s:\n%d\n", vector_to_string(vec).c_str(), median);
    return median;
}

// Return the index of a value in a vector
size_t index_of(const vector<int> &vec, int val)
{
    return distance(find(vec.begin(), vec.end(), val), vec.begin());
}

vector<int> subvector(const vector<int> &vec, size_t start, size_t end)
{
    vector<int>::const_iterator start_iter = vec.begin() + start;
    vector<int>::const_iterator end_iter = start_iter + (end - start);
    vector<int> subvec(start_iter, end_iter);
    return subvec;
}

// Divide vector into slices of a given size
// If slice_size does not divide vec.size(), last slice will be shorter
vector<vector<int>> slice_vector(const vector<int> &vec, size_t slice_size)
{
    size_t num_slices = (vec.size() + slice_size - 1) / slice_size;
    vector<vector<int>> slices;
    size_t i = 0;
    for (size_t slice_index = 0; slice_index < num_slices; slice_index += 1)
    {
        size_t start = slice_index * slice_size;
        size_t end = start + min(slice_size, vec.size() - slice_index * slice_size);
        slices.push_back(subvector(vec, start, end));
    }
    //printf("Slices of %s:\n%s\n", vector_to_string(vec).c_str(), vector_vector_to_string(slices).c_str());
    return slices;
}

// Get the median of the medians of all length-r sub-lists
int median_of_medians(const vector<int> &vec, size_t r)
{
    if (vec.size() <= r)
        return median(vec);
    
    vector<int> medians((vec.size() + r - 1)/r);
    vector<vector<int>> slices = slice_vector(vec, r);
    for (size_t i = 0; i < slices.size(); i++)
        medians[i] = median(slices[i]);
    return median(medians);
}

// Swap the values stored in two variables
void swap(int &a, int &b)
{
    int c = a;
    a = b;
    b = c;
}

// Swap the values stored at two indices
void vector_swap(vector<int> &vec, size_t i, size_t j)
{
    swap(vec[i], vec[j]);
}

// Moves all elements smaller than pivot to before all elements larger than pivot
// Returns length of sub-vector of elements known to be no larger than pivot
size_t partition(vector<int> &vec, int pivot, size_t start, size_t end)
{
    //printf("Partitioning %s with pivot %d\n", vector_to_string(vec, start, end).c_str(), pivot);

    size_t l_wick = start, r_wick = end - 1;
    while (l_wick < r_wick)
    {
        while (vec[l_wick] < pivot && l_wick < r_wick)
            l_wick++;
        while (vec[r_wick] > pivot && r_wick > l_wick)
            r_wick--;
        if (l_wick == r_wick)
            break;
        vector_swap(vec, l_wick, r_wick);
        l_wick++;
        r_wick--;
    }
    if (vec[l_wick] <= pivot)
        l_wick++;
    
    //printf("Partitioned at index %d:\n%s\n", l_wick, vector_to_string(vec, start, end).c_str());

    return l_wick;
}

// Partitions full vector
size_t partition(vector<int> &vec, int pivot)
{
    return partition(vec, pivot, 0, vec.size()); 
}

void insertion_sort(vector<int> &vec, size_t start, size_t end)
{
    size_t i = start;
    while (i < end)
    {
        size_t j = i;
        while (j > start && vec[j-1] > vec[j])
        {
            vector_swap(vec, j, j-1);
            j--;
        }
        i++;
    }
}

void insertion_sort(vector<int> &vec)
{
    insertion_sort(vec, 0, vec.size());
}

// Median-of-medians quicksort, guaranteed O(n log n) time
void mom_quicksort(vector<int> &vec, size_t r, size_t start, size_t end)
{
    //printf("Quicksorting %s\n", vector_to_string(vec, start, end).c_str());
    if (end - start < 600)
    {
        insertion_sort(vec, start, end);
    }
    else
    {
        int pivot = median_of_medians(subvector(vec, start, end), r);
        size_t p = partition(vec, pivot, start, end);
        if (p == end) p--;
        if (p == start) p++;
        mom_quicksort(vec, r, start, p);
        mom_quicksort(vec, r, p, end);
    }
    //printf("Sorted as %s\n", vector_to_string(vec, start, end).c_str());
}

void mom_quicksort(vector<int> &vec, size_t r)
{
    mom_quicksort(vec, r, 0, vec.size());
}

vector<int> random_vector(size_t len, size_t max)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(1,int(max));
    vector<int> vals;
    for (size_t i = 0; i < len; i++)
    {
        vals.push_back(distr(gen));
    }
    return vals;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
    }
    
    ifstream input_file;
    input_file.open(argv[1]);
    if (!(input_file.is_open()))
    {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    vector<int> vals = readVals(input_file);

    mom_quicksort(vals, 5);
    printf("%s\n", vector_to_string(vals).c_str());

    input_file.close();
    return 0;
}