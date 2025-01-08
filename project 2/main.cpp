// main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <chrono>   // For timing
#include <cctype>   // For std::tolower
#include <algorithm> // For std::transform
#include "hashtable.hpp"

// Define operator<< for std::pair to simplify output
template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<const K, V>& p) {
    os << "{" << p.first << ": " << p.second << "}";
    return os;
}

// Helper function to generate large random strings
std::string generateRandomString(size_t length) {
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    const size_t max_index = sizeof(charset) - 2;
    std::string str(length, 0);
    for (size_t i = 0; i < length; ++i) {
        str[i] = charset[(size_t)rand() % max_index];
    }
    return str;
}

int main() {
    srand(static_cast<unsigned>(time(0)));  // Seed for random number generation

    // Test Case 1: Insert and find elements
    {
        std::cout << "Test Case 1: Insert and find elements" << std::endl;
        HashTable<int, std::string> ht;
        ht.insert(1, "one");
        ht.insert(2, "two");
        ht.insert(3, "three");
        auto it = ht.find(2);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        }
        std::cout << ht[1] << std::endl;
        std::cout << ht[3] << std::endl;
    }
    // Expected Output:
    // Test Case 1: Insert and find elements
    // two
    // one
    // three

    // Test Case 2: Erase element and check find
    {
        std::cout << "Test Case 2: Erase element and check find" << std::endl;
        HashTable<int, std::string> ht;
        ht.insert(1, "one");
        ht.erase(1);
        auto it = ht.find(1);
        if (it != ht.end()) {
            std::cout << "Found" << std::endl;
        } else {
            std::cout << "Not Found" << std::endl;
        }
    }
    // Expected Output:
    // Test Case 2: Erase element and check find
    // Not Found

    // Test Case 3: Operator[] on non-existent key
    {
        std::cout << "Test Case 3: Operator[] on non-existent key" << std::endl;
        HashTable<int, int> ht;
        ht[5] = 10; // Should insert key 5 with value 10
        std::cout << ht[5] << std::endl;
    }
    // Expected Output:
    // Test Case 3: Operator[] on non-existent key
    // 10

    // Test Case 4: Rehash to larger size
    {
        std::cout << "Test Case 4: Rehash to larger size" << std::endl;
        HashTable<int, int> ht;
        for (int i = 0; i < 10; ++i) {
            ht.insert(i, i * i);
        }
        ht.rehash(100); // Increase the number of buckets
        auto it = ht.find(5);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 4: Rehash to larger size
    // 25

    // Test Case 5: Rehash to smaller size
    {
        std::cout << "Test Case 5: Rehash to smaller size" << std::endl;
        HashTable<int, int> ht;
        for (int i = 0; i < 100; ++i) {
            ht.insert(i, i * i);
        }
        ht.rehash(10); // Decrease the number of buckets
        auto it = ht.find(50);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 5: Rehash to smaller size
    // 2500

    // Test Case 6: Copy constructor
    {
        std::cout << "Test Case 6: Copy constructor" << std::endl;
        HashTable<std::string, int> ht;
        ht.insert("one", 1);
        ht.insert("two", 2);
        HashTable<std::string, int> ht_copy(ht);
        auto it = ht_copy.find("one");
        if (it != ht_copy.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 6: Copy constructor
    // 1

    // Test Case 7: Assignment operator
    {
        std::cout << "Test Case 7: Assignment operator" << std::endl;
        HashTable<std::string, int> ht;
        ht.insert("one", 1);
        ht.insert("two", 2);
        HashTable<std::string, int> ht_assign;
        ht_assign = ht;
        auto it = ht_assign.find("two");
        if (it != ht_assign.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 7: Assignment operator
    // 2

    // Test Case 21: Custom Hash Function
    {
        std::cout << "Test Case 21: Custom Hash Function" << std::endl;
        struct CustomHash {
            std::size_t operator()(int key) const {
                // Simple custom hash function: reverse the digits of the key
                int reversed = 0, temp = key;
                while (temp > 0) {
                    reversed = reversed * 10 + temp % 10;
                    temp /= 10;
                }
                return std::hash<int>()(reversed);
            }
        };
        HashTable<int, std::string, CustomHash> ht;
        ht.insert(123, "original");
        auto it = ht.find(123);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 21: Custom Hash Function
    // original

    // Test Case 22: User-defined key type
    {
        std::cout << "Test Case 22: User-defined key type" << std::endl;
        struct Point {
            int x, y;
            bool operator==(const Point& other) const {
                return x == other.x && y == other.y;
            }
        };
        struct PointHash {
            std::size_t operator()(const Point& p) const {
                return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
            }
        };
        HashTable<Point, std::string, PointHash> ht;
        Point p = {1, 2};
        ht.insert(p, "point(1,2)");
        auto it = ht.find(p);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 22: User-defined key type
    // point(1,2)

    // Test Case 23: Exception Safety
    {
        std::cout << "Test Case 23: Exception Safety" << std::endl;
        try {
            HashTable<int, std::string> ht;
            ht.insert(1, "one");
            // Simulate exception
            throw std::runtime_error("Simulated exception");
            ht.insert(2, "two");
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        // Since ht is local to the try block, we need a new instance to check if key 1 exists
        HashTable<int, std::string> ht;
        auto it = ht.find(1);
        if (it != ht.end()) {
            std::cout << it->second << std::endl;
        } else {
            std::cout << "Key 1 not found" << std::endl;
        }
    }
    // Expected Output:
    // Test Case 23: Exception Safety
    // Simulated exception
    // Key 1 not found

    // Test Case 31: Insertion of Max Int Values
    {
        std::cout << "Test Case 31: Insertion of Max Int Values" << std::endl;
        HashTable<int, std::string> ht;
        int max_int = std::numeric_limits<int>::max();
        ht.insert(max_int, "max_int");
        auto it = ht.find(max_int);
        if (it != ht.end()) {
            std::cout << "Found key with max int value: " << it->second << std::endl;
        } else {
            std::cout << "Key not found" << std::endl;
        }
    }
    // Expected Output:
    // Test Case 31: Insertion of Max Int Values
    // Found key with max int value: max_int

    // Test Case 32: Insertion of Min Int Values
    {
        std::cout << "Test Case 32: Insertion of Min Int Values" << std::endl;
        HashTable<int, std::string> ht;
        int min_int = std::numeric_limits<int>::min();
        ht.insert(min_int, "min_int");
        auto it = ht.find(min_int);
        if (it != ht.end()) {
            std::cout << "Found key with min int value: " << it->second << std::endl;
        } else {
            std::cout << "Key not found" << std::endl;
        }
    }
    // Expected Output:
    // Test Case 32: Insertion of Min Int Values
    // Found key with min int value: min_int

    // Test Case 33: Large Scale Insertions
    // {
    //     std::cout << "Test Case 33: Large Scale Insertions" << std::endl;
    //     HashTable<int, int> ht;
    //     const int num_elements = 10000000; // 10 million elements
    //     for (int i = 0; i < num_elements; ++i) {
    //         ht.insert(i, i);
    //     }
    //     std::cout << "Inserted " << num_elements << " elements." << std::endl;
    //     auto it = ht.find(num_elements - 1);
    //     if (it != ht.end()) {
    //         std::cout << "Successfully retrieved last element: " << it->second << std::endl;
    //     }
    // }
    // Expected Output:
    // Test Case 33: Large Scale Insertions
    // Inserted 10000000 elements.
    // Successfully retrieved last element: 9999999

    // Test Case 34: Memory Exhaustion Handling
    // {
    //     std::cout << "Test Case 34: Memory Exhaustion Handling" << std::endl;
    //     try {
    //         HashTable<int, std::string> ht;
    //         const int num_elements = 1000000000; // 1 billion elements
    //         for (int i = 0; i < num_elements; ++i) {
    //             ht.insert(i, "value");
    //         }
    //     } catch (const std::bad_alloc& e) {
    //         std::cout << "Memory allocation failed: " << e.what() << std::endl;
    //     }
    // }
    // Expected Output:
    // Test Case 34: Memory Exhaustion Handling
    // Memory allocation failed: std::bad_alloc

    // Test Case 35: Stress Test with Long Strings
    {
        std::cout << "Test Case 35: Stress Test with Long Strings" << std::endl;
        HashTable<std::string, std::string> ht;
        const int num_elements = 10000;
        for (int i = 0; i < num_elements; ++i) {
            std::string key = generateRandomString(1000); // Keys of length 1000
            std::string value = generateRandomString(1000); // Values of length 1000
            ht.insert(key, value);
        }
        std::cout << "Inserted " << num_elements << " large string elements." << std::endl;
    }
    // Expected Output:
    // Test Case 35: Stress Test with Long Strings
    // Inserted 10000 large string elements.

    // Test Case 36: High Collision Scenario
    {
        std::cout << "Test Case 36: High Collision Scenario" << std::endl;
        struct PoorHash {
            std::size_t operator()(const int&) const {
                return 0; // All keys hash to the same bucket
            }
        };
        HashTable<int, int, PoorHash> ht;
        const int num_elements = 10000;
        for (int i = 0; i < num_elements; ++i) {
            ht.insert(i, i);
        }
        auto it = ht.find(num_elements / 2);
        if (it != ht.end()) {
            std::cout << "Successfully retrieved element in high collision scenario: " << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 36: High Collision Scenario
    // Successfully retrieved element in high collision scenario: 5000

    // Test Case 37: Iterator Integrity after Erase
{
    std::cout << "Test Case 37: Iterator Integrity after Erase" << std::endl;
    HashTable<int, int> ht;
    for (int i = 0; i < 10; ++i) {
        ht.insert(i, i);
    }
    ht.erase(0);
    std::cout << "After erasing key 0, iterating over elements:" << std::endl;
    for (auto it = ht.begin(); it != ht.end(); ++it) {
        std::cout << it->first << " ";
    }
    std::cout << std::endl;
}

    // Expected Output:
    // Test Case 37: Iterator Integrity after Erase
    // After erasing key 0, iterating over elements:
    // [Remaining keys from 1 to 9]

    // Test Case 38: Rehashing to Zero Buckets
    {
        std::cout << "Test Case 38: Rehashing to Zero Buckets" << std::endl;
        HashTable<int, int> ht;
        ht.insert(1, 1);
        try {
            ht.rehash(0); // Attempt to rehash to zero buckets
        } catch (const std::exception& e) {
            std::cout << "Rehashing to zero buckets failed: " << e.what() << std::endl;
        }
    }
    // Expected Output:
    // Test Case 38: Rehashing to Zero Buckets
    // Rehashing to zero buckets failed: [Appropriate error message]

    // Test Case 39: Custom Key with No Hash Function
    {
        std::cout << "Test Case 39: Custom Key with No Hash Function" << std::endl;
        struct NoHashKey {
            int id;
            bool operator==(const NoHashKey& other) const {
                return id == other.id;
            }
        };
        // Attempting to use HashTable with NoHashKey without providing a hash function
        // This should result in a compile-time error
        // Uncommenting the following lines should cause a compilation error
        /*
        HashTable<NoHashKey, std::string> ht;
        NoHashKey key = {1};
        ht.insert(key, "value");
        */
    }
    // Expected Output:
    // Test Case 39: Custom Key with No Hash Function
    // [Compilation error due to missing hash function]

    // Test Case 40: Extreme Load Factor Adjustment
    {
        std::cout << "Test Case 40: Extreme Load Factor Adjustment" << std::endl;
        HashTable<int, int> ht;
        ht.insert(1, 1);
        ht.insert(2, 2);
        ht.insert(3, 3);
        // Assume your HashTable supports setting a maximum load factor (optional)
        // ht.setMaxLoadFactor(0.1); // Set an extremely low load factor
        // ht.rehash(1000); // Rehash to increase the number of buckets
        auto it = ht.find(2);
        if (it != ht.end()) {
            std::cout << "Found key after adjusting load factor: " << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 40: Extreme Load Factor Adjustment
    // Found key after adjusting load factor: 2

        // Test Case 41: Rehashing During Iteration
    {
        std::cout << "Test Case 41: Rehashing During Iteration" << std::endl;
        HashTable<int, int> ht;
        for (int i = 0; i < 10; ++i) {
            ht.insert(i, i);
        }

        // Option 1: Rehash before iteration
        ht.rehash(50);

        std::cout << "Iterating over elements after rehashing:" << std::endl;
        for (auto it = ht.begin(); it != ht.end(); ++it) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
    }
    // Test Case 42: Copy Assignment with Self-assignment
    {
        std::cout << "Test Case 42: Copy Assignment with Self-assignment" << std::endl;
        HashTable<int, int> ht;
        ht.insert(1, 1);
        // ht = ht; // Self-assignment
        auto it = ht.find(1);
        if (it != ht.end()) {
            std::cout << "Self-assignment successful, value: " << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 42: Copy Assignment with Self-assignment
    // Self-assignment successful, value: 1

    // Test Case 43: Move Constructor and Move Assignment
    {
        std::cout << "Test Case 43: Move Constructor and Move Assignment" << std::endl;
        HashTable<int, int> ht;
        ht.insert(1, 1);
        HashTable<int, int> ht_moved(std::move(ht)); // Move constructor
        auto it = ht_moved.find(1);
        if (it != ht_moved.end()) {
            std::cout << "Move constructor successful, value: " << it->second << std::endl;
        }
        HashTable<int, int> ht_assigned;
        ht_assigned = std::move(ht_moved); // Move assignment
        it = ht_assigned.find(1);
        if (it != ht_assigned.end()) {
            std::cout << "Move assignment successful, value: " << it->second << std::endl;
        }
    }

    // Test Case 44: Custom Equality Comparator
    {
        std::cout << "Test Case 44: Custom Equality Comparator" << std::endl;
        struct CaseInsensitiveHash {
            std::size_t operator()(const std::string& key) const {
                std::string lower_key = key;
                std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(), ::tolower);
                return std::hash<std::string>()(lower_key);
            }
        };
        struct CaseInsensitiveEqual {
            bool operator()(const std::string& lhs, const std::string& rhs) const {
                return lhs.size() == rhs.size() &&
                    std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                               [](char a, char b) { return std::tolower(a) == std::tolower(b); });
            }
        };
        HashTable<std::string, int, CaseInsensitiveHash, CaseInsensitiveEqual> ht;
        ht.insert("Key", 1);
        auto it = ht.find("key");
        if (it != ht.end()) {
            std::cout << "Case-insensitive find successful, value: " << it->second << std::endl;
        }
    }
    // Expected Output:
    // Test Case 44: Custom Equality Comparator
    // Case-insensitive find successful, value: 1

    // Continue with Test Cases 45 to 48...

    // Test Case 49: Clear Functionality
    {
        std::cout << "Test Case 49: Clear Functionality" << std::endl;
        HashTable<int, int> ht;
        ht.insert(1, 1);
        ht.insert(2, 2);
        ht = HashTable<int, int>(); // Reset 'ht' to a new, empty hash table
        if (ht.begin() == ht.end()) {
            std::cout << "HashTable cleared successfully." << std::endl;
        } else {
            std::cout << "HashTable is not empty after reset." << std::endl;
        }
    }
    // Expected Output:
    // Test Case 49: Clear Functionality
    // HashTable cleared successfully.

    // Test Case 50: Performance Measurement
    {
        std::cout << "Test Case 50: Performance Measurement" << std::endl;
        HashTable<int, int> ht;
        const int num_elements = 1000000;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_elements; ++i) {
            ht.insert(i, i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end - start;
        std::cout << "Time taken to insert " << num_elements << " elements: "
                  << insert_duration.count() << " seconds" << std::endl;
    }
    // Expected Output:
    // Test Case 50: Performance Measurement
    // Time taken to insert 1000000 elements: [time in seconds]

    return 0;
}
