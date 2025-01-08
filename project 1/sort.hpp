#ifndef VE281P1_SORT_HPP
#define VE281P1_SORT_HPP


#include <vector>
#include <stdlib.h>
#include <functional>

//swap helper function
template<typename T>
void swap(std::vector<T> &vector, std::size_t i, std::size_t j) {
    //swap the element at vector[i] and vector[j]
    T temp = vector[i];
    vector[i] = vector[j];
    vector[j] = temp;
}

template<typename T, typename Compare>
void bubble_sort(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement   

    if (vector.size() == 0 || vector.size() == 1) return;

    for (std::size_t i = vector.size(); i > 0; i --) {
        for (std::size_t j = 0; j < i - 1; j ++) {
            if (comp(vector[j + 1], vector [j])) {
                swap(vector, j, j + 1);
            }
        }
    }

}

template<typename T, typename Compare>
void insertion_sort(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement

    if (vector.size() == 0 || vector.size() == 1) return;

    for (std::size_t i = 0; i < vector.size() - 1; i ++) {
        std::size_t j = i;

        while (j >= 0 && comp(vector[j + 1], vector[j])) {
            swap(vector, j, j + 1);
            if (j == 0) break;
            else j --;
        }
    }
}

template<typename T, typename Compare>
void selection_sort(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement

    if (vector.size() == 0 || vector.size() == 1) return;

    T min_el = vector[0]; //store the minimum element found on the current loop 
    for (std::size_t i = 0; i < vector.size(); i ++) {
        std::size_t min_index = i;
        min_el = vector[i];
        for (std::size_t j = i + 1; j < vector.size(); j ++) {
            if (comp(vector[j], min_el)) {
                min_index = j;
                min_el = vector[j];
            }
        }
        if (min_index != i) swap(vector, min_index, i);
    }
}

//mergesort helper function
//sort and merge array
template<typename T, typename Compare>
void merge(std::vector<T> &arr, std::size_t l, std::size_t mid, std::size_t r, Compare comp = std::less<T>()) {

    std::vector<T> leftarray(mid - l + 1);
    std::vector<T> rightarray(r - mid);

    for (std::size_t i = 0; i < (mid - l + 1); i ++) {
        leftarray[i] = arr[l + i];
    } 
    for (std::size_t i = 0; i < (r - mid); i ++) {
        rightarray[i] = arr[mid + 1 + i];
    }

    std::size_t i = 0, j = 0;
    std::size_t k = l;
    while (i < leftarray.size() && j < rightarray.size()) {
        if (comp(leftarray[i], rightarray[j])) {
            //leftarray[i] < rightarray[j]
            //insert leftarray[i] into array
            arr[k] = leftarray[i];
            i ++;
            k ++;
        }

        else if (comp(rightarray[j], leftarray[i])){
            //leftarray[i] > rightarray[j]
            //insert rightarray[j] into array
            arr[k] = rightarray[j];
            j ++;
            k ++;
        } 
        else {
            //leftarray[i] == rightarray[j]
            //insert leftarray into the array first, follow with rightarray[j]
            arr[k] = leftarray[i];
            i ++;
            k ++;
        }
    }

    if (j < (r - mid)) {
        while (j < rightarray.size()) {
            arr[k] = rightarray[j];
            j ++;
            k ++;
        }
    }
    else if (i < (mid - l + 1)) {
        while (i < leftarray.size()) {
            arr[k] = leftarray[i];
            i ++;
            k ++;
        }
    }
}

//mergesort helper function
//split array
template<typename T, typename Compare>
void mergesort(std::vector<T> &vector, std::size_t l, std::size_t r, Compare comp = std::less<T>()) {
    if (l >= r) return; //end splitting

    std::size_t mid = l + (r - l) / 2;
    mergesort(vector, l, mid, comp);
    mergesort(vector, mid + 1, r, comp);
    merge(vector, l, mid, r, comp);

}

template<typename T, typename Compare>
void merge_sort(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement

    if (vector.size() == 0 || vector.size() == 1) return;

    mergesort(vector, 0, vector.size() - 1, comp);
}


//quicksort in place partition
// helper function 
template<typename T, typename Compare>
std::size_t partition_inplace(std::vector<T> &vector, std::size_t low, std::size_t high, Compare comp = std::less<T>()) {
    T pivot_el = vector[high]; //choose pivot postd::size_t as the most right element first
    std::size_t i = low; 

    //i: start iteration and increment from the left most element, 
    //j: start iteration and decrement from the second right most element 
    for (std::size_t j = low; j < high; j ++) {
        if (comp(vector[j], pivot_el)) {
            swap(vector, i, j);
            i ++;
        }
    }

    swap(vector, i, high);
    return i;
}

//helper function
//quick sort in place
template<typename T, typename Compare>
void quicksort_inplace(std::vector<T> &vector, std::size_t low, std::size_t high, Compare comp = std::less<T>()) {
    if (low >= high ) return; //end sorting 

    std::size_t pivot = partition_inplace(vector, low, high, comp); //return the index of the sorted pivot element

    if (pivot > 0) quicksort_inplace(vector, low, pivot - 1, comp);
    if (pivot < vector.size()) quicksort_inplace(vector, pivot + 1, high, comp);
}

template<typename T, typename Compare>
void quick_sort_inplace(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement

    if (vector.size() == 0 || vector.size() == 1) return;

    quicksort_inplace(vector, 0, vector.size() - 1, comp);
}

//quicksort extra place 
//helper function 
template<typename T, typename Compare>
std::size_t partition_extra(std::vector<T> &arr, std::size_t low, std::size_t high, Compare comp = std::less<T>()) {
    T pivot_el = arr[high]; //choose pivot point as the most right element first


    std::vector<T> less;
    std::vector<T> greater;

    for (std::size_t i = low; i < high; i ++) {
        if (comp(arr[i], pivot_el)){
            less.push_back(arr[i]);
        }
        else {
            greater.push_back(arr[i]);
        }
    }

    for (std::size_t i = 0; i < less.size(); i ++) {
        arr[low + i] = less[i];
    }
    arr[low + less.size()] = pivot_el;

    for (std::size_t i = 0; i < greater.size(); i ++) {
        arr[low + less.size() + 1 + i] = greater[i];
    }

    return low + less.size();
}
 

//quicksort_extra
//helper function
template<typename T, typename Compare>
void quicksort_extra(std::vector<T> &vector, std::size_t low, std::size_t high, Compare comp = std::less<T>()) {

    if (low >= high) return;

    std::size_t pivot = partition_extra(vector, low, high, comp);

    if (pivot != 0) quicksort_extra(vector, low, pivot - 1, comp);
    if (pivot < vector.size()) quicksort_extra(vector, pivot + 1, high, comp);
}

template<typename T, typename Compare>
void quick_sort_extra(std::vector<T> &vector, Compare comp = std::less<T>()) {
    // TODO: implement
    if (vector.size() == 0 || vector.size() == 1) return;

    quicksort_extra(vector, 0, vector.size() - 1, comp);
}



#endif //VE281P1_SORT_HPP