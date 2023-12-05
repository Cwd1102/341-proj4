// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
unsigned int hashCode(const string str);
string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};

class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
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
class Tester{
    public:
        bool insertionTest(CarDB &lhs){
            bool result = true;
            int indexCheck = 0;
            Car inserting = EMPTY;
            int count = 0;
            CarDB cardbTwo(lhs.m_currentCap, hashCode, DOUBLEHASH);

            for (int i = 0; i < lhs.m_currentCap; i++)
            {   
                inserting = Car(lhs.m_currentTable[i].getModel(),lhs.m_currentTable[i].getQuantity(),
                                    lhs.m_currentTable[i].getDealer(),true);

                if (cardbTwo.insert(inserting))
                {
                    count++;
                }
                else {
                    cout << count << endl;
                }
            }

            result = result && (count == lhs.m_currentCap);
            
            for (int i = 0; i < lhs.m_currentCap; i++)
            {
                if (lhs.m_currentTable[i].getUsed() == true)
                {
                    inserting = Car(lhs.m_currentTable[i].getModel(),lhs.m_currentTable[i].getQuantity(),
                                lhs.m_currentTable[i].getDealer(),true);
                    
                    result = result && (i == indexChecker(lhs,inserting,indexCheck));
                    
                }   
            }
        
            return result;

        }

    bool removeTest(CarDB &lhs){
        bool removed = true;
        float ratio = 0;
        int removing = 0;
        Car inserting = EMPTY;
        //removes 70 percent under hash requirement
        for (int i = 0; i < lhs.m_currentCap*0.7; i++)
        {   
            if (lhs.m_currentTable[i].getUsed() == true)
            {   
                inserting = Car(lhs.m_currentTable[i].getModel(),lhs.m_currentTable[i].getQuantity(),
                        lhs.m_currentTable[i].getDealer(),true);
                removed = removed && (lhs.remove(inserting));
                if (removed)
                {
                    removing++;
                }
            }      
        }

        if (removing == 0)
        {
            removed = removed && (removing == lhs.m_currNumDeleted);
            
        }
        else{
            ratio = 1.0 * removing / lhs.m_currentSize;
            removed = removed && (ratio == lhs.deletedRatio());
        }
        
        return removed;
    }

    bool duplicateTest(CarDB &lhs){
        bool result = true;
        Car inserting = EMPTY;
        for (int i = 0; i < lhs.m_currentCap; i++)
        {   
            inserting = Car(lhs.m_currentTable[i].getModel(),lhs.m_currentTable[i].getQuantity(),
                        lhs.m_currentTable[i].getDealer(),true);
        
            if (!lhs.insert(inserting))
            {
                result = result && true;
            }
            else{
                result = false;
            }
            
        }

        for (int i = 0; i < lhs.m_currentCap*0.7; i++)
        {   
            inserting = Car(lhs.m_currentTable[i].getModel(),lhs.m_currentTable[i].getQuantity(),
                        lhs.m_currentTable[i].getDealer(),true);
            if(!lhs.remove(inserting)){
                result = result && true;
            }
            else{
                result = false;
            }
        }
        return result;
    }

    bool insertHash(CarDB &lhs){
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);
        Random RndQuantity(0,50);
        int percent = lhs.m_currentCap *0.51;
        int amount = 0;
        bool result = true;
        int count = 1;
        for (int i=0;i<percent;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);

            // inserting data in to the CarDB object
            lhs.insert(dataObj);   
        }


        amount = percent * 0.25;
        result = result && (lhs.lambda() < 0.51);
        result = result && (amount == lhs.m_currentSize);

        if (result)
        {
            for (int i = 0; i < 3; i++)
            {
                Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);  
                lhs.insert(dataObj);
                if (lhs.m_oldTable != nullptr)
                {
                    count++;
                }
            }

            result = result && (count == 4);

        }
        

    
        return result;


    }




        int indexChecker(CarDB &lhs,Car &insertion, int index){
            int newIndex = 0;
            int bucket = hashCode(insertion.getModel()) % lhs.m_currentCap;
            int jumpCount = 0;
            bool looping = true;

        if (insertion == lhs.m_currentTable[bucket])
        {   
            return bucket;
        }
        
        else{
            while(looping){

                if (lhs.m_currProbing == QUADRATIC)
                {
                    newIndex = ((bucket % lhs.m_currentCap) + (jumpCount * jumpCount)) % lhs.m_currentCap;
                }
                else if (lhs.m_currProbing == DOUBLEHASH)
                {   
                    newIndex = ((bucket % lhs.m_currentCap) + jumpCount * (11 - (bucket % 11))) % lhs.m_currentCap;
                }

                if(lhs.m_currentTable[newIndex] == insertion){
                    looping = false;
                }  

                if(lhs.m_currentTable[newIndex].getUsed() == false){
                    looping = false;
                }   
                jumpCount++;
            }
        }
            return newIndex;
        }






};



int main(){
    Tester test;
    vector<Car> dataList;

    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    CarDB quadProb(MINPRIME, hashCode, QUADRATIC);
    CarDB noneProb(MINPRIME, hashCode, NONE);
    CarDB errorProb(MINPRIME, hashCode, NONE);
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);
    Random RndQuantity(0,50);
    for (int i=0;i<200;i++){
        // generating random data
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                        RndID.getRandNum(), true);

        // inserting data in to the CarDB object
        cardb.insert(dataObj);   
        quadProb.insert(dataObj);
        noneProb.insert(dataObj);
    }

    cardb.dump();
    if (test.insertionTest(cardb))
    {
        cout << "Normal Double Insertation Test: Passed" << endl;
    }
    else{
        cout << "Normal Double Insertation Test: Failed" << endl;
    }

    if (test.insertionTest(quadProb))
    {
        cout << "Normal Quadratic Insertation Test: Passed" << endl;
    }
    else{
        cout << "Normal Quadratic Insertation Test: Failed" << endl;
    }

    if (test.insertionTest(noneProb))
    {
        cout << "Normal None Insertation Test: Passed" << endl;
    }
    else{
        cout << "Normal None Insertation Test: Failed" << endl;
    }

    if (test.insertionTest(errorProb))
    {
        cout << "Error Insertation Test: Passed" << endl;
    }
    else{
        cout << "Error Insertation Test: Failed" << endl;
    }

    if (test.removeTest(cardb))
    {
        cout << "Normal Double Remove Test: Passed" << endl;
    }
    else{
        cout << "Normal Double Test: Failed" << endl;
    }
    
    if (test.removeTest(quadProb))
    {
        cout << "Normal Quad Remove Test: Passed" << endl;
    }
    else{
        cout << "Normal Quad Remove Test: Failed" << endl;
    }

    if (test.removeTest(noneProb))
    {
        cout << "Normal None Remove Test: Passed" << endl;
    }
    else{
        cout << "Normal None Remove Test: Failed" << endl;
    }

    if (test.removeTest(errorProb))
    {
        cout << "Error Remove Test: Passed" << endl;
    }
    else{
        cout << "Error Remove Test: Failed" << endl;
    }

    if(test.duplicateTest(cardb)){
        cout << "Normal Double Duplicate Test: Passed" << endl;
    }
    else{
        cout << "Normal Double Duplicate Test: Failed" << endl;
    }

    if(test.duplicateTest(quadProb)){
        cout << "Normal Quad Duplicate Test: Passed" << endl;
    }
    else{
        cout << "Normal Quad Duplicate Test: Failed" << endl;
    }

    
    if(test.duplicateTest(noneProb)){
        cout << "Normal None Duplicate Test: Passed" << endl;
    }
    else{
        cout << "Normal None Duplicate Test: Failed" << endl;
    }
    
    if(test.duplicateTest(errorProb)){
        cout << "Normal Error Duplicate Test: Passed" << endl;
    }
    else{
        cout << "Normal Error Duplicate Test: Failed" << endl;
    }

    CarDB cardbTwo(MINPRIME, hashCode, DOUBLEHASH);
    CarDB quadProbTwo(MINPRIME, hashCode, QUADRATIC);
    CarDB noneProbTwo(MINPRIME, hashCode, NONE);

    if (test.insertHash(cardbTwo))
    {
        cout << "Normal Double insertHash Test: Passed" << endl;
    }
    else{
        cout << "Normal Double insertHash Test: Failed" << endl;
    }
    



}
unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}