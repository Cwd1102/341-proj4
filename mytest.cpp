// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM { UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE };
class Random {
public:
    Random(int min, int max, RANDOM type = UNIFORMINT, int mean = 50, int stdev = 20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL) {
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean, stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min, max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min, (double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum) {
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int>& array) {
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i <= m_max; i++) {
            array.push_back(i);
        }
        shuffle(array.begin(), array.end(), m_generator);
    }

    void getShuffle(int array[]) {
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i <= m_max; i++) {
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it = temp.begin(); it != temp.end(); it++) {
            array[i] = *it;
            i++;
        }
    }

    int getRandNum() {
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if (m_type == NORMAL) {
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while (result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT) {
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum() {
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result * 100.0) / 100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester {
    public:
        bool testInsert();
        bool testGetCarError();
        bool testGetCar();
        bool testGetCarColliding();
        bool testRemove();
        bool testRemoveColliding();

};

unsigned int hashCode(const string str);

string carModels[5] = { "challenger", "stratos", "gt500", "miura", "x101" };
string dealers[5] = { "super car", "mega car", "car world", "car joint", "shack of cars" };

unsigned int hashCode(const string str) {
    unsigned int val = 0;
    const unsigned int thirtyThree = 33;  // magic number from textbook
    for (unsigned int i = 0; i < str.length(); i++)
        val = val * thirtyThree + str[i];
    return val;
}

int main() {
    Tester test;

    if(test.testInsert())
		cout << "testInsert passed" << endl;
	else
		cout << "testInsert failed" << endl;

    if(test.testGetCarError())
        cout << "testGetCarError passed" << endl;
    else
        cout << "testGetCarError failed" << endl;

    if(test.testGetCar())
        cout << "testGetCar passed" << endl;
	else
		cout << "testGetCar failed" << endl;

    if(test.testGetCarColliding())
		cout << "testGetCarColliding passed" << endl;
    else
        cout << "testGetCarColliding failed" << endl;

    if(test.testRemove())
        cout << "testRemove passed" << endl;
	else
		cout << "testRemove failed" << endl;

    if (test.testRemoveColliding())
		cout << "testRemoveColliding passed" << endl;
	else
		cout << "testRemoveColliding failed" << endl;


    return 0;
}

bool Tester::testInsert() {

    CarDB carDB(10, hashCode, DEFPOLCY);

    Car car1("challenger", 5, 1001);
    Car car2("stratos", 3, 1002);
    Car car3("gt500", 7, 1003);

    // Insert Car objects into the CarDB
    if (carDB.insert(car1) && carDB.insert(car2) && carDB.insert(car3)) {
        // Check if the size and load factor are correct after insertions
        if (carDB.deletedRatio() == 0.0 &&
            carDB.getCar("challenger", 1001) == car1 && carDB.getCar("stratos", 1002) == car2 &&
            carDB.getCar("gt500", 1003) == car3) {
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    // Create more non-colliding Car objects for testing
    Car car4("miura", 2, 1004);
    Car car5("x101", 1, 1005);

    // Insert additional Car objects into the CarDB
    if (carDB.insert(car4) && carDB.insert(car5)) {
        // Check if the size and load factor are correct after insertions
        if (carDB.deletedRatio() == 0.0 &&
            carDB.getCar("miura", 1004) == car4 && carDB.getCar("x101", 1005) == car5) {
            return true;
        }
        else {
        }
    }
    else {
    }

    return false;
}


bool Tester::testGetCarError() {
    
    CarDB carDB(10, hashCode, DEFPOLCY);
    // Create some non-colliding Car objects for testing
    Car nonExistingCar = carDB.getCar("nonexistent", 9999);

    if (nonExistingCar.getModel().empty() && nonExistingCar.getQuantity() == 0 && nonExistingCar.getDealer() == 0) {
        return true;
    }
    else {

        return false;
    }
}

bool Tester::testGetCar() {
    // Create a CarDB object with a small size for testing
    CarDB carDB(10, hashCode, DEFPOLCY);

    // Create some non-colliding Car objects for testing
    Car car1("challenger", 5, 1001);
    Car car2("stratos", 3, 1002);
    Car car3("gt500", 7, 1003);

    // Insert Car objects into the CarDB
    carDB.insert(car1);
    carDB.insert(car2);
    carDB.insert(car3);

    // Retrieve the Car objects using getCar
    Car retrievedCar1 = carDB.getCar("challenger", 1001);
    Car retrievedCar2 = carDB.getCar("stratos", 1002);
    Car retrievedCar3 = carDB.getCar("gt500", 1003);

    // Check if the retrieved Car objects match the inserted ones
    if (retrievedCar1 == car1 && retrievedCar2 == car2 && retrievedCar3 == car3) {
        return true;
    }
    else {
        return false;
    }
}

bool Tester::testGetCarColliding() {
    // Create a CarDB object with a small size for testing
    CarDB carDB(10, hashCode, DEFPOLCY);

    // Create some colliding Car objects for testing
    Car car1("challenger", 5, 1001);
    Car car2("stratos", 3, 1002);
    Car car3("gt500", 7, 1003);

    // Insert Car objects into the CarDB (colliding keys)
    carDB.insert(car1);
    carDB.insert(car2);
    carDB.insert(car3);

    // Retrieve the Car objects using getCar (colliding keys)
    Car retrievedCar1 = carDB.getCar("challenger", 1001);
    Car retrievedCar2 = carDB.getCar("stratos", 1002);
    Car retrievedCar3 = carDB.getCar("gt500", 1003);

    // Check if the retrieved Car objects match the inserted ones
    if (retrievedCar1 == car1 && retrievedCar2 == car2 && retrievedCar3 == car3) {
        return true;
    }
    else {
        return false;
    }
}

bool Tester::testRemove() {
    CarDB carDB(10, hashCode, DEFPOLCY);

    Car car1("challenger", 5, 1001);
    Car car2("stratos", 3, 1002);
    Car car3("gt500", 7, 1003);

    carDB.insert(car1);
    carDB.insert(car2);
    carDB.insert(car3);

    carDB.remove(car1);
    carDB.remove(car2);
    carDB.remove(car3);

    Car retrievedCar1 = carDB.getCar("challenger", 1001);
    Car retrievedCar2 = carDB.getCar("stratos", 1002);
    Car retrievedCar3 = carDB.getCar("gt500", 1003);

    if (retrievedCar1.getUsed() == true && retrievedCar2.getUsed() == true && retrievedCar3.getUsed() == true) {
        return true;
    }
    else {
        return false;
    }
}

bool Tester::testRemoveColliding() {
    // Create a CarDB object with a small size for testing
    CarDB carDB(10, hashCode, DEFPOLCY);
    Car car1("challenger", 5, 1001);
    Car car2("stratos", 3, 1002);
    Car car3("gt500", 7, 1003);

    //inserting cars
    carDB.insert(car1);
    carDB.insert(car2);
    carDB.insert(car3);
    //removing
    carDB.remove(car1);
    carDB.remove(car2);
    carDB.remove(car3);
    
    // Attempt to retrieve the removed Car objects using getCar (colliding keys)
    Car retrievedCar1 = carDB.getCar("challenger", 1001);
    Car retrievedCar2 = carDB.getCar("stratos", 1002);
    Car retrievedCar3 = carDB.getCar("gt500", 1003);

    // Check if the retrieved Car objects are empty (indicating successful removal)
    if (retrievedCar1.getUsed() == true && retrievedCar2.getUsed() == true && retrievedCar3.getUsed() == true) {
        return true;
    }
    else {
        return false;
    }
}
