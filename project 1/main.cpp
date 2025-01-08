#include <iostream>
#include <vector>
#include <algorithm>
#include "sort.hpp"

using namespace std;


//function to test sort function
//return true if array is sorted correctly 
//return false if sorted incorrectly
template<typename T, typename Compare>
bool test_sort (const vector<T> sorted, void (*sort_func)(vector<T>&, Compare), vector<T> unsorted, Compare comp = less<T>()) {
    vector<T> arr = unsorted;
    sort_func(arr , comp);

    if (arr == sorted) return true; //return true if array is correctly sorted by the testing function 
    
    return false; //array is not correctly sorted
}

int main () {

    vector<double> unsorted_arr = {6, 6, 5.7, 1.9, 0.5};
    vector<double> sorted_arr = {0.5, 1.9, 5.7, 6, 6};

    cout << "original array : " ; 
    copy(unsorted_arr.begin(), unsorted_arr.end(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;;
    //bubble sort
    cout << "bubble sort : ";
    if (test_sort (sorted_arr, bubble_sort, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect";

    cout << "insertion sort : ";
    if (test_sort(sorted_arr, insertion_sort, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect" << endl;

    cout << "selection sort : ";
    if (test_sort (sorted_arr, selection_sort, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect" << endl;

    cout << "merge sort : ";
    if (test_sort (sorted_arr, merge_sort, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect" << endl;
    
    cout << "quick sort inplace : ";
    if (test_sort (sorted_arr, quick_sort_inplace, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect" << endl;

    cout << "quick sort extra : ";
    if (test_sort (sorted_arr, quick_sort_extra, unsorted_arr, less<double>())) {
        cout << "correct" << endl;
    }
    else cout << "incorrect" << endl;
    // copy(arr.begin(), arr.end(), ostream_iterator<int>(cout, " "));
    // cout << endl;

    //test special case. example: arr = {0} , arr = {1,1,1,1}

    return 0;
}

//selection sort: time exceeded
//merge sort wrong answer  
