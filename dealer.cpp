// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY) {
    // Set the capacity based on the user's input
    int capacity = size;
    if (capacity < MINPRIME) {
        capacity = MINPRIME;
    }
    else if (capacity > MAXPRIME) {
        capacity = MAXPRIME;
    }
    else if (!isPrime(capacity)) {
        capacity = findNextPrime(capacity);
    }

    // Set the member variables
    m_currentCap = capacity;
    m_currentTable = new Car[m_currentCap];
    m_oldCap = 0;
    m_oldTable = nullptr;
    m_hash = hash;
    m_currProbing = probing;
}
CarDB::~CarDB() {

}

void CarDB::changeProbPolicy(prob_t policy) {

}

bool CarDB::insert(Car car) {
    // Calculate the hash value
    int hashValue = m_hash(car.m_model) % m_currentCap;

    // Handle hash collisions using the specified probing policy
    int probeCount = 0;
    int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue] == car) {
            return false; // Car object already exists in the hash table
        }

        // Increment the hash value based on the probing policy
        switch (m_currProbing) {
        case QUADRATIC:
            hashValue = (hashValue + increment * increment) % m_currentCap;
            break;
        case DOUBLEHASH:
            hashValue = (hashValue + increment * m_hash(car.m_model)) % m_currentCap;
            break;
        }

        // Increment the probe count and update the increment value
        probeCount++;
        increment = probeCount * probeCount;
    }

    // Insert the Car object into the hash table
    m_currentTable[hashValue] = car;

    // Check if rehashing is required
    if (probeCount >= m_currentCap / 4) {
        // Create a new table with increased capacity
        int newCap = m_currentCap * 2;
        Car* newTable = new Car[newCap];

        // Transfer the data from the old table to the new table
        int transferCount = 0;
        for (int i = 0; i < m_currentCap; i++) {
            if (m_currentTable[i].m_model != "") {
                int newHashValue = m_hash(m_currentTable[i].m_model) % newCap;
                while (newTable[newHashValue].m_model != "") {
                    newHashValue = (newHashValue + 1) % newCap;
                }
                newTable[newHashValue] = m_currentTable[i];
                transferCount++;
                if (transferCount == m_currentCap / 4) {
                    break;
                }
            }
        }

        // Update the member variables
        delete[] m_oldTable;
        m_oldTable = m_currentTable;
        m_oldCap = m_currentCap;
        m_currentTable = newTable;
        m_currentCap = newCap;
    }

    return true; // Car object successfully inserted into the hash table
}


bool CarDB::remove(Car car) {
    // Find the bucket of the object using the proper probing policy
    int hashValue = m_hash(car.m_model) % m_currentCap;
    int probeCount = 0;
    int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue] == car) {
            // Tag the bucket as deleted
            m_currentTable[hashValue].m_used = true;
            return true; // Car object found and deleted
        }

        // Increment the hash value based on the probing policy
        switch (m_currProbing) {
        case QUADRATIC:
            hashValue = (hashValue + increment * increment) % m_currentCap;
            break;
        case DOUBLEHASH:
            hashValue = (hashValue + increment * m_hash(car.m_model)) % m_currentCap;
            break;
        }

        // Increment the probe count and update the increment value
        probeCount++;
        increment = probeCount * probeCount;
    }

    return false; // Car object not found
}


Car CarDB::getCar(string model, int dealer) const {
    // Search the current table
    int hashValue = m_hash(model) % m_currentCap;
    int probeCount = 0;
    int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue].m_model == model && m_currentTable[hashValue].m_dealer == dealer) {
            return m_currentTable[hashValue]; // Car object found in the current table
        }

        // Increment the hash value based on the probing policy
        switch (m_currProbing) {
        case QUADRATIC:
            hashValue = (hashValue + increment * increment) % m_currentCap;
            break;
        case DOUBLEHASH:
            hashValue = (hashValue + increment * m_hash(model)) % m_currentCap;
            break;
        }

        // Increment the probe count and update the increment value
        probeCount++;
        increment = probeCount * probeCount;
    }

    // Search the old table if it exists
    if (m_oldTable != nullptr) {
        hashValue = m_hash(model) % m_oldCap;
        probeCount = 0;
        increment = 1;
        while (m_oldTable[hashValue].m_model != "") {
            if (m_oldTable[hashValue].m_model == model && m_oldTable[hashValue].m_dealer == dealer) {
                return m_oldTable[hashValue]; // Car object found in the old table
            }

            // Increment the hash value based on the probing policy
            switch (m_currProbing) {
            case QUADRATIC:
                hashValue = (hashValue + increment * increment) % m_oldCap;
                break;
            case DOUBLEHASH:
                hashValue = (hashValue + increment * m_hash(model)) % m_oldCap;
                break;
            }

            // Increment the probe count and update the increment value
            probeCount++;
            increment = probeCount * probeCount;
        }
    }

    // Car object not found in both tables, return empty object
    return Car();
}


float CarDB::lambda() const {
    float occupiedBuckets = 0;
    for (int i = 0; i < m_currentCap; i++) {
        if (m_currentTable[i].m_model != "" || m_currentTable[i].m_used) {
            occupiedBuckets++;
        }
    }
    float loadFactor = occupiedBuckets / m_currentCap;
    return loadFactor;
}


float CarDB::deletedRatio() const {
    float deletedBuckets = 0;
    float occupiedBuckets = 0;
    for (int i = 0; i < m_currentCap; i++) {
        if (m_currentTable[i].m_model != "" && m_currentTable[i].m_used) {
            deletedBuckets++;
        }
        if (m_currentTable[i].m_model != "" || m_currentTable[i].m_used) {
            occupiedBuckets++;
        }
    }
    float ratio = deletedBuckets / occupiedBuckets;
    return ratio;
}


void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool CarDB::updateQuantity(Car car, int quantity) {
	return false;

}

bool CarDB::isPrime(int number) {
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current) {
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME - 1;
    for (int i = current; i < MAXPRIME; i++) {
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0)
                break;
            else if (j + 1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Car& car) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity << ")";
    else
        sout << "";
    return sout;
}

bool operator==(const Car& lhs, const Car& rhs) {
    // since the uniqueness of an object is defined by model and delaer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}