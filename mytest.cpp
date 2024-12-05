#include "hash.h"
#include <random>

std::mt19937 generator(10);
std::uniform_int_distribution<> distribution(1000, 99999);

using namespace std;

unsigned int hashCode(string str);
unsigned int vaccineSerial() {return distribution(generator);}

class Tester{
public:
    static bool TestInsert(HashTable *Table);
    static bool TestRemove(HashTable *Table);
private:
    // Private Helpers for TestInsert
    static bool NonCollide(HashTable *Table);
    static bool Collide(HashTable *Table);
    static bool SizeChange(HashTable *Table);
};

int main(){
    // newTester;
    HashTable Table1(101, hashCode);
    HashTable Table2(101, hashCode);

    cout << "------------------------------------------------------------------------------" << endl;
    cout << "Testing the Hash Table Insert function in the following capacities: " << endl;
    if(Tester::TestInsert(&Table1)){
        cout << "All Tests passed, overall insert works" << endl;
    }else{
        cout << "Failed Test, Evaluate above:" << endl;
    }

    cout << "------------------------------------------------------------------------------" << endl;
    cout << "Testing the Hash Table Removal Function in the following ways: " << endl;
    if(Tester::TestRemove(&Table2)){
        cout << "All Tests passed, overall removal works" <<endl;
    }else{
        cout << "Test failure occurred: Evaluate Above:  " << endl;
    }

    cout << "------------------------------------------------------------------------------" << endl;
}

/* -- Test the insertion operation in the hash table. The following presents a sample algorithm to test the normal insertion operation:
 * There is some non-colliding data points in the hash table
 * Insert multiple non-colliding keys
 * Check whether they are inserted in the correct bucket (correct index)
 * Check whether the data size changes correctly
 */
bool Tester::TestInsert(HashTable *Table) {
    bool No_collide, collide, size;
    probing prob = Table->m_currentProb;
    // Test No Collision Inserts
    if(NonCollide(Table)){
        No_collide = true;
    }else{
        No_collide = false;
    }

    // Test Collision Inserts
    if(Collide(Table)){
        collide = true;
    }else{
        collide = false;
    }

    // Test Size Change of Hash Table
    if(SizeChange(Table)){
        size = true;
    }else{
        size = false;
    }

    cout << "    Probing Policy Change: ";
    if(prob == Table->m_currentProb){
        cout << "0" << endl;
        return false;
    }else{
        cout << "1" << endl;
    }

    if(No_collide && collide && size){
        return true;
    }else{
        return false;
    }
}
// A Non Collision Tester Insert, should insert 5 data points that will not collide
bool Tester::NonCollide(HashTable *Table) {
    cout << "    No Collide Test: ";
    bool T1 = Table->insert(Person("A", vaccineSerial()));
    bool T2 = Table->insert(Person("B", vaccineSerial()));
    bool T3 = Table->insert(Person("C", vaccineSerial()));
    bool T4 = Table->insert(Person("D", vaccineSerial()));
    bool T5 = Table->insert(Person("E", vaccineSerial()));
    if(T1 && T2 && T3 && T4 && T5 && Table->m_size == 5){
        cout << "1" << endl;
        return true;
    }else{
        cout << "0" << endl;
        return false;
    }
}

// A Collision Insert tester, will manufacture a collision to test collision handling
bool Tester::Collide(HashTable *Table) {
    cout << "    Collision Test: ";
    bool T1 = Table->insert(Person("S", vaccineSerial()));
    bool T2 = Table->insert(Person("B", vaccineSerial()));
    if(T1 && T2){
        cout << "1" << endl;
        return true;
    }else{
        cout << "0" << endl;
        return false;
    }
}

// A Tester to check that a the Hash table's size increases correctly
bool Tester::SizeChange(HashTable *Table) {
    cout << "    Size Change Test: ";
    unsigned int startsize = Table->tableSize();
    while(startsize > 51){
        Table->insert(Person("SizeTest", vaccineSerial()));
        startsize--;
    }
    if(Table->tableSize() > startsize){
        cout << "1" << endl;
        return true;
    }else{
        cout << "0" << endl;
        return false;
    }
}

/*
 * -- Test the remove operation in the hash table
 */
bool Tester::TestRemove(HashTable *Table) {
    bool removebob, rehash, size;
    probing starting = Table->m_currentProb;
    unsigned int stSize = Table->tableSize();

    Person Bob("Bob", vaccineSerial());
    Person John("John", vaccineSerial());
    Person Terry("Terry", vaccineSerial());
    Person Sam("Sam", vaccineSerial());
    Person Jack("Jack", vaccineSerial());

    Table->insert(Bob);
    Table->insert(John);
    Table->insert(Terry);
    Table->insert(Sam);
    Table->insert(Jack);

    cout << "    Basic Remove Test: ";
    if(Table->remove(Bob)){
        cout << "1" << endl;
        removebob = true;
    }else{
        cout << "0" << endl;
        removebob = false;
    }

    cout << "    Rehash on Removal of 1/4 of objects: ";
    Table->remove(Sam);
    if(Table->m_currentProb != starting){
        cout << "1" << endl;
        rehash = true;
    }else{
        cout << "0" <<endl;
        rehash = false;
    }

    cout << "    Size update occurred: ";
    if(Table->tableSize() > stSize){
        cout << "1" << endl;
        size = true;
    }else{
        cout << "0" << endl;
        size = false;
    }

    if(removebob && size && rehash){
        return true;
    }else{
        return false;
    }
}

/*
 * -- A hash code for use in the hashing function, will take a string key from a person object
 */
unsigned int hashCode(string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for(int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}