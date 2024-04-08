//Nicholas Alfree AERSP 424
//HW 2

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

//Q1

//Q1.1
using namespace std;

class Sensor
{
public:
    virtual void gatherData() = 0;
    virtual void processData() = 0;
    virtual ~Sensor() {}
};

//Q1.2
//Temp sensor
class TemperatureSensor : public Sensor
{
public:
    void gatherData() override
    {
        cout << "Gathering data from Temperature Sensor." << endl;
    }
    void processData() override
    {
        cout << "Processing data from Temperature Sensor." << endl;
    }
};

//Pressure sensor
class PressureSensor : public Sensor
{
public:
    void gatherData() override
    {
        cout << "Gathering data from Pressure Sensor." << endl;
    }
    void processData() override
    {
        cout << "Processing data from Pressure Sensor." << endl;
    }
};

//Position sensor
class PositionSensor : public Sensor
{
public:
    void gatherData() override
    {
        cout << "Gathering data from Position Sensor." << endl;
    }
    void processData() override
    {
        cout << "Processing data from Position Sensor." << endl;
    }
};

// Q1.3
//Factory Method
class FactorySensor
{
public:
    static Sensor* createSensor(const string& sensorType)
    {
        if (sensorType == "Temperature")
            return new TemperatureSensor();
        else if (sensorType == "Pressure")
            return new PressureSensor();
        else if (sensorType == "Position")
            return new PositionSensor();
        else
            return nullptr;
    }
};

//Q1.4
class AerospaceControlSystem
{
public:
    void addSensor(Sensor* sensor)
    {
        sensors.push_back(sensor);
    }

    void monitorAndAdjust()
    {
        for (Sensor* sensor : sensors)
        {
            sensor->gatherData();
            sensor->processData();
            cout << "Adjusting controls based on sensor data.\n" << endl;
        }
    }

    //Delete sensor objects for cleanup
    ~AerospaceControlSystem()
    {
        for (Sensor* sensor : sensors)
        {
            delete sensor;
        }
    }

private:
    vector<Sensor*> sensors;

    //Q1.5 in int main()

};

//Q2

const int robotNum = 5;
int totalTime = 0;
std::mutex toolM[robotNum];
std::mutex outM;

void Task(int robotId)
{
    //Start time
    auto startTime = std::chrono::high_resolution_clock::now();

    toolM[robotId].lock();
    toolM[(robotId + 1) % robotNum].lock();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //Collect data
    outM.lock();
    std::cout << "Robot " << robotId + 1 << " is collecting data." << std::endl;
    outM.unlock();

    //Grab tools
    outM.lock();
    std::cout << "Robot " << robotId + 1 << " acquired tools and starts performing a task." << std::endl;
    outM.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    toolM[robotId].unlock();
    toolM[(robotId + 1) % robotNum].unlock();

    //Return tools
    outM.lock();
    std::cout << "Robot " << robotId + 1 << " finished the task and returning the tools." << std::endl;
    outM.unlock();

    //End time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

    totalTime = std::max(totalTime, static_cast<int>(duration.count()));
}

//Q3

const int aircraftTotal = 10;
const int aircraftMax = 3;
int airLanded = 0;
int airInPattern = 0;
bool talk = false;
std::mutex m;
std::condition_variable cVari;

//Aircraft Landing Function
void IncomingAircraft(int id)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::unique_lock<std::mutex> lck(m);
    std::cout << "Aircraft #" << id << " requesting landing." << std::endl;

    //Comms check
    if (!talk && airInPattern < aircraftMax)
    {
        talk = true;
        std::cout << "Aircraft #" << id << " is cleared to land." << std::endl;
        cVari.notify_one();
    }

    //Traffic check
    else if (airInPattern >= aircraftMax)
    {
        std::cout << "Approach pattern full. Aircraft #" << id << " redirected to another airport." << std::endl;
        std::cout << "Aircraft #" << id << " flying to another airport." << std::endl;
    }
    airInPattern++;
}
//ATC function
void atc()
{
    auto sTime = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> lck(m);
    while (airLanded < aircraftTotal)
    {
        cVari.wait(lck, [] { return talk || airLanded >= aircraftTotal; });
        if (airLanded >= aircraftTotal)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::cout << "Runway is now free." << std::endl;
        talk = false;
        airInPattern--;
    }

    auto eTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(eTime - sTime);
    std::cout << "duration: " << duration.count() << " seconds." << std::endl;
}

int main()
{
    //Q1
    std::cout << "***Question 1***\n" << std::endl;

    AerospaceControlSystem ctrlSys;

    //Q1.5
    ctrlSys.addSensor(FactorySensor::createSensor("Temperature"));
    ctrlSys.addSensor(FactorySensor::createSensor("Pressure"));
    ctrlSys.addSensor(FactorySensor::createSensor("Position"));

    ctrlSys.monitorAndAdjust();

    //Q2
    std::cout << "***Question 2***\n" << std::endl;

    std::thread rob[robotNum];

    //Perform task
    for (int i = 0; i < robotNum; ++i)
    {
        rob[i] = std::thread(Task, i);
    }

    //Join threads
    for (int i = 0; i < robotNum; ++i)
    {
        rob[i].join();
    }

    std::cout << "duration: " << totalTime << " seconds\n" << std::endl;

    //Q3
    std::cout << "***Question 3***\n" << std::endl;

    std::thread threadATC(atc);

    std::thread threadAircrafts[aircraftTotal];
    for (int i = 0; i < aircraftTotal; ++i)
    {
        threadAircrafts[i] = std::thread(IncomingAircraft, i + 1);
    }

    for (int i = 0; i < aircraftTotal; ++i)
    {
        threadAircrafts[i].join();
        airLanded++;
    }

    //Sleep and aircraft landing check
    cVari.notify_one();
    threadATC.join();

    //Q4 in attached main.cpp

    return 0;
}