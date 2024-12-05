// CMSC 341 - Spring 2021 - Project 4
// hash.cpp
// The implementation of the class HashTable
#include "hash.h"
#include "math.h"

/**
 * The Overloaded Constructor, for the hash table class. Passed two arguments, the hash_fn type is defined in the
 * hash.h. The size variable must be checked to not exceed the MAXPRIME and not be less than the MINPRIME
 * @param size the size of the Hash table 
 * @param hash the hash function that will be used to compute the spacing and filling of the Hash Table
 */
HashTable::HashTable(unsigned size, hash_fn hash){
    // Check if the size is prime
    if(isPrime(size)){
        cout << "Prime Size: Continuing" << endl;
    }else{
        size = findNextPrime(size);
        cout << "Non-Prime Size: Finding next prime: " << size;
    }

    // Check for a size that is too large
    if(size > MAXPRIME){
        cout << "Size too large, setting size to: " << MAXPRIME << endl;
        size = MAXPRIME;
    // Check for a size that is too small
    }else if(size < MINPRIME){
        cout << "Size too small, setting size to: " << MINPRIME << endl;
        size = MINPRIME;
    }

    // Declaration of Variables

    // Set the variables to be equal to
    m_size = 0;
    // The current Hash function
    m_hash = hash;
    // The Current Probing function should be initialized as LINEAR
    m_currentProb = LINEAR;
    // The Current Capacity of the Table
    m_capacity = size;
    // The number of deleted objects in the Hash Table
    m_numDeleted = 0;
    // Pointer to the table, a dynamically allocated array
    m_table = new Person[size];

    // Fill m_table with null people, this will allow collisions to be tracked
    for(unsigned int i = 0; i < m_size; i++){
        m_table[i] = Person("", 0);
    }
}

/**
 * The Destructor for the Hash Table. Delete m_table, and set all other variables to zero or nullptr
 */
HashTable::~HashTable(){
    // Delete the dynamically allocated Array
    delete[] m_table;
    // Set variables to zero/nullptr
    m_size = 0;
    m_hash = nullptr;
    m_capacity = 0;
    m_numDeleted = 0;
}

/**
 * Insert a person object, The objects insertion index is determined by applying the Hash Function to the Person object
 * passed to the function as an argument. Collisions in insert should be handled using probing,
 * the current probing policy is stored with m_currentProb, At the creation of the Hash Table the m_currentProb is Linear.
 * The Same person can never be inserted twice, and should return a false in that case.
 * @param person The person object being inserted in the Hash Table
 * @return Returns a bool value based on the successfulness of the insert operation, true if successful, false otherwise
 */
bool HashTable::insert(Person person){
    bool success; // A bool value to track success
    unsigned int Index = m_hash(person.key()) % m_capacity;

    // Handle Collisions
    if(m_table[Index].serial() != 0){
        unsigned int TempIndex = Index;

    // Handle the Linear Probing
        if(m_currentProb == LINEAR){
            // Linear: (hash(key) + i) % tablesize
            unsigned int i = 1;
            while(m_table[TempIndex].serial() != 0){
                // Duplicate Checking Will end insert with false
                if(m_table[TempIndex] == person){
                    return false;
                }
                // Linear probing function
                TempIndex = (m_hash(person.key()) + i) % tableSize();
                if(i >= m_capacity){
                    cout << "No Available insertion location" << endl;
                    return false;
                }
                i++;
            }
            // Insert the person object
            m_table[TempIndex] = person;
            // Increment size
            m_size++;
            success = true;

    // Handle the Quadratic Probing
        }else if(m_currentProb == QUADRATIC){
            unsigned int i = 1;
            while(m_table[TempIndex].serial() != 0){
                // Duplicate Checking will return false, and not insert
                if(m_table[TempIndex].serial() == person.serial()){
                    return false;
                }
                // Quadratic Probing Function
                TempIndex = ((m_hash(person.key()) % tableSize()) + i * i) % tableSize();
                if(i >= m_capacity){
                    cout << "No Available insertion location" << endl;
                    return false;
                }
                i++;
            }
            // Insert the person object
            m_table[TempIndex] = person;
            // Increment size
            m_size++;
            success = true;

    // Handle invalid Probing, This shouldn't happen
        }else{
            cout << "Fatal Error In Probing Priority, Canceling Insert" << endl;
            return false;
        }

    // Handle no collisions, this is a standard insert.
    }else{
        m_table[Index] = person;
        m_size++;
        success = true;
    }

    // Check if a rehash is required before returning
    if(lambda() > 0.5){
    // Handle for current LINEAR
        if(m_currentProb == LINEAR){
            // Set new probing and rehash
            setProbingPolicy(QUADRATIC);

    // Handle for current QUADRATIC
        }else if(m_currentProb == QUADRATIC){
            // Set new probing and rehash
            setProbingPolicy(LINEAR);
        }
    }

    return success;
}

/**
 * A function to remove the specified data point from the Hash table. The removed objects must be moved to the bucket.
 * To find the bucket of an object we need to use the m_currentProb. After every remove we need to evaluate for changes
 * in the m_currentProb.
 * @param person The Person object to be removed from the Hash Table
 * @return Returns a bool value, True if Remove is successful, False otherwise
 */
bool HashTable::remove(Person person){
    // Variable Declaration
    bool didRemove = false;
    unsigned int TestIndex = m_hash(person.key()) % tableSize();

// Handles checking for a probed insertion of the person object
    if(!(m_table[TestIndex] == person)){
        unsigned int i = 0;
    // Find matching serial with LINEAR probing
        if(m_currentProb == LINEAR){
            // Runs until the person is found or we run out of entries
            while(!(person == m_table[TestIndex])){
                // Calculates the next potential hash value
                TestIndex = (m_hash(person.key()) + i) % tableSize();
                i++;

            // Makes sure the while will not run forever in the case of removing non inserted person
                if(i >= m_size){
                    cout << "Person Does not exist" << endl;
                    return false;
                }
            }
            // Removes the person, and replaces them with a placeholder
            m_table[TestIndex] = Person("DELETED", 0);
            didRemove = true;
            m_numDeleted++;

    // Find matching serial with Quadratic Probing
        }else if(m_currentProb == QUADRATIC){
            // Runs until the person is found or we run out of entries
            while(!(person == m_table[TestIndex])){
                // Calculates the next potential hash value
                TestIndex = (m_hash(person.key()) + i * i) % tableSize();
                i++;

            // Makes sure the while will not run forever in the case of removing non inserted person
                if(i >= m_size){
                    cout << "Person Does not exist" << endl;
                    return false;
                }
            }
            // Removes the person, and replaces them with a placeholder
            m_table[TestIndex] = Person("DELETED", 0);
            didRemove = true;
            m_numDeleted++;
    // Handle issues with a bad Probing Function
        }else{
            cout << "Bad Probing Function, Unable to Remove" << endl;
            return false;
        }
// Handles Remove when the probing function is not required
    }else{
        m_table[TestIndex] = Person("DELETED", 0);
        didRemove = true;
        m_numDeleted++;
    }

    if(deletedRatio() > 0.25){
        if(m_currentProb == LINEAR){
            setProbingPolicy(QUADRATIC);
        }else if(m_currentProb == QUADRATIC){
            setProbingPolicy(LINEAR);
        }else{
            cout << "Existing Invalid Probing Type" << endl;
        }
    }

    return didRemove;
}

/**
 * Sets the probing policy for the Hash table, this will be limited to either Linear, or Quadratic.
 * @param prob The new probability function for the Hash Table
 */
void HashTable::setProbingPolicy(probing prob){
    m_currentProb = prob;

    // Copy some variables for later use
    unsigned int originalCapacity = m_capacity;
    Person* oldHash = m_table;

    // Set the new maximum capacity
    m_size -= m_numDeleted;
    m_capacity = findNextPrime(int(4 * m_size));
    m_size = 0;
    m_numDeleted = 0;
    m_table = new Person[m_capacity];

    //Initialize an array and fill it with placeholder people
    for(unsigned int i = 0; i < m_capacity; i++){
        m_table[i] = Person("", 0);
    }

    // Add data from the old hash table to the new
    for(unsigned int i = 0; i < originalCapacity; i++){
        if(oldHash[i].serial() != 0) {
            // Forces the program to only reinsert real data value
            insert(oldHash[i]);
        }
    }

    // Delete the old hash table array
    delete[] oldHash;
}

/**
 * A function to return the load factor of the hash table. The load factor is the ration of occupied buckets
 * to the table size.
 * @return The load factor
 */
float HashTable::lambda() const {
    // Will provide something like .77 or 77% of the table is full
    float RLambda = (float)m_size / (float)m_capacity;
    return RLambda;
}

/**
 * A function to return the ratio of deleted buckets to the total number of occupied buckets
 * @return The ratio of deleted buckets to non-deleted buckets
 */
float HashTable::deletedRatio() const {
    // Checks for size = 0 edge cases
    if(m_size == 0){
        cout << "A Zero Size at removal" << endl;
        return 0;
    }
    // Cast m_capacity and m_numDeleted to float values and finds the ratio between them
    float RDeleted = (float)m_numDeleted / (float)m_size;
    return RDeleted;
}

/**
 * Prints the Hash Table
 * Implementation provided by instructor
 */
void HashTable::dump() const {
    for (unsigned int i = 0; i < m_capacity; i++) {
        cout << "[" << i << "] : " << m_table[i] << endl;
    }
}

/**
 * Determines if a passed number is prime, this was provided by the instructors
 * @param number provided to the function, will be checked to see if prime
 * @return True if Prime, False otherwise
 */
bool HashTable::isPrime(int number){
    // If number is prime this function returns true
    // otherwise it returns false
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

/**
 * Takes a value, and finds the next prime value, this was provided by the instructors
 * @param current The value, which we will use to find the next prime
 * @return The next prime value after the current
 */
int HashTable::findNextPrime(int current){
    //we won't go beyond MAXPRIME
    //the smallest prime would be next after MINPRIME
    if (current < MINPRIME) current = MINPRIME;
    for (int i=current; i<MAXPRIME; i++) {
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

// Helper Functions