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

    m_hash = hash;    // hash function
    m_newPolicy = probing;     // stores the change of policy request
    m_currentCap = capacity;    // hash table size (capacity)
    m_currentTable = new Car[m_currentCap]{ EMPTY };  // hash table
    m_currentSize = 0;   // current number of entries
    // m_currentSize includes deleted entries 
    m_currNumDeleted = 0;// number of deleted entries
    m_currProbing = probing;       // collision handling policy

    m_oldTable = 0;      // hash table
    m_oldCap = 0;        // hash table size (capacity)
    m_oldSize = 0;       // current number of entries
    // m_oldSize includes deleted entries
    m_oldNumDeleted = 0; // number of deleted entries
     m_oldProbing = probing;    // collision handling policy
}
CarDB::~CarDB() {
	// Delete the current table
	if (m_currentTable != nullptr) {
		delete[] m_currentTable;
	}

	// Delete the old table
	if (m_oldTable != nullptr) {
		delete[] m_oldTable;
	}

}

void CarDB::changeProbPolicy(prob_t policy) {
// If the current probing policy is different from the new one, rehash the table
	if (m_currProbing != policy) {
		m_newPolicy = policy;
	}
}

bool CarDB::insert(Car car) {
	// Insert the object into the current table
	int index = insertHelper(car);

    if (lambda() > 0.5) {
        reHash();
    }

	// If the object was inserted successfully, increment the current size
    if (index != -1) {
		m_currentSize++;
		return true;
	}

	return false;
}


bool CarDB::remove(Car &car) {
    // Find the bucket of the object using the proper probing policy
    int hashValue = m_hash(car.m_model) % m_currentCap;
    int probeCount = 0;
    int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue] == car) {
            // Tag the bucket as deleted
            m_currentTable[hashValue].m_used = true;
            car = m_currentTable[hashValue]; // Update the car object
            return true; // Car object found and deleted
        }

        // Increment the hash value based on the probing policy
        if (m_currProbing == QUADRATIC)
            hashValue = (hashValue + increment * increment) % m_currentCap;
        else
            hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;

        

        // Increment the probe count and update the increment value
            increment++;
    }

    return false; // Car object not found
}


Car CarDB::getCar(string model, int dealer) const {
    // Search the current table
    int hashValue = m_hash(model) % m_currentCap;
    int probeCount = 0;
    int increment = 0;

    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue].m_model == model && m_currentTable[hashValue].m_dealer == dealer) {
            return m_currentTable[hashValue]; // Car object found in the current table
        }

        // Increment the hash value based on the probing policy
        if (m_currProbing == QUADRATIC)
            hashValue = (hashValue + increment * increment) % m_currentCap;
        else
            hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;


        // Increment the probe count and update the increment value
        increment++;
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
            if (m_currProbing == QUADRATIC)
                hashValue = (hashValue + increment * increment) % m_currentCap;
            else
                hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;


            // Increment the probe count and update the increment value
            increment++;
        }
    }

    // Car object not found in both tables, return empty object
    return Car();
}


float CarDB::lambda() const {
    float loadFactor = 1.0 * m_currentSize / m_currentCap;
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
    // Find the bucket of the object using the proper probing policy
	int hashValue = m_hash(car.m_model) % m_currentCap;
	int probeCount = 0;
	int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue] == car) {
			// Update the quantity
			m_currentTable[hashValue].m_quantity = quantity;
			return true; // Car object found and updated
		}

        if (m_currProbing == QUADRATIC)
            hashValue = (hashValue + increment * increment) % m_currentCap;
        else
            hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;

        increment++;
	}

	return false; // Car object not found

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

//********************************************************************************
//************************** Private Helper Below ********************************
//********************************************************************************

int CarDB::insertHelper(Car car) const{
	// Find the bucket of the object using the proper probing policy
	int hashValue = m_hash(car.m_model) % m_currentCap;
	int increment = 0;

    while (m_currentTable[hashValue].getUsed() == true) {
        if (m_currProbing == NONE) {
            return -1;
        }
		// Increment the hash value based on the probing policy
        if(m_currProbing == QUADRATIC)
			hashValue = (hashValue + increment * increment) % m_currentCap;
        else
			hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;
		// Increment the probe count and update the increment value
        increment++;
	}
    

	// Insert the object into the table

	m_currentTable[hashValue] = car;
	return hashValue;
}

void CarDB::reHash() {
    int sizeNew = 0;
    int percent = 0;
    int increment = 0;
    bool ifBreak = true;

    // Create a new table with the same capacity as the current table
    if (m_oldTable == nullptr) {
        m_oldTable = new Car [m_currentCap]{ EMPTY };

        for (int i = 0; i < m_currentCap; i++) {
            m_oldTable[i] = m_currentTable[i];
        }
        // Set the member variables
        m_oldCap = m_currentCap;
        m_oldNumDeleted = m_currNumDeleted;
        m_oldSize = m_currentSize;
        m_currentTable = nullptr;
        delete[] m_currentTable;

        sizeNew = findNextPrime((m_currentCap - m_currNumDeleted) * 4);
        m_currentTable = new Car[sizeNew]{ EMPTY };
        m_currentCap = sizeNew;
        m_currProbing = m_newPolicy;
        m_currentSize = 0;
        m_currNumDeleted = 0;

    }

    percent = m_oldSize * .25;
    int count = 0;

    for(int i =0; i < m_oldCap; i++) {
        if (m_oldTable[i].getUsed() == true) {
            insertRehash(m_oldTable[i]);
            m_oldTable[i].m_used = false;
            m_oldNumDeleted++;
        }
	}

    if (m_oldNumDeleted == m_oldSize) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
        m_oldNumDeleted = 0;
        m_oldProbing = m_currProbing;
    }
}

//
int CarDB::findCar(Car car) const {
	// Find the bucket of the object using the proper probing policy
	int hashValue = m_hash(car.m_model) % m_currentCap;
	int probeCount = 0;
	int increment = 1;
    while (m_currentTable[hashValue].m_model != "") {
        if (m_currentTable[hashValue] == car) {
			return hashValue; // Car object found in the current table
		}

        // Increment the hash value based on the probing policy
        if (m_currProbing == QUADRATIC)
            hashValue = (hashValue + increment * increment) % m_currentCap;
        else
            hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;
        // Increment the probe count and update the increment value
        increment++;
	}

	// Search the old table if it exists
    if (m_oldTable != nullptr) {
		hashValue = m_hash(car.m_model) % m_oldCap;
		probeCount = 0;
		increment = 1;
        while (m_oldTable[hashValue].m_model != "") {
            if (m_oldTable[hashValue] == car) {
				return hashValue; // Car object found in the old table
			}

            // Increment the hash value based on the probing policy
            if (m_currProbing == QUADRATIC)
                hashValue = (hashValue + increment * increment) % m_currentCap;
            else
                hashValue = (((hashValue + increment * m_hash(car.m_model)) % m_currentCap));


            // Increment the probe count and update the increment value
            increment++;
		}
	}

	// Car object not found in both tables, return -1
	return -1;
}

void CarDB::insertRehash(Car car) {
    // Find the bucket of the object using the proper probing policy
    int hashValue = m_hash(car.m_model) % m_currentCap;
    int probeCount = 0;
    int increment = 1;

    if (m_currProbing == NONE) {
        return;
    }

    while (m_currentTable[hashValue].getUsed() == true) {

        // Increment the hash value based on the probing policy
        if (m_currProbing == QUADRATIC)
            hashValue = (hashValue + increment * increment) % m_currentCap;
        else
            hashValue = (hashValue + increment * (11 - (hashValue % 11))) % m_currentCap;
        // Increment the probe count and update the increment value
        increment++;


    }

    // Insert the object into the table
    m_currentTable[hashValue] = car;
}