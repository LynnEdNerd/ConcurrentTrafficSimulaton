#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

// enum to cover all traffic light possibilities.  As stated in the requirements, there are no yellow lights.
enum class TrafficLightPhase {red, green};

template <class T>
class MessageQueue
{
public: 

    // The 2 fundamental methods for accessing the message queue.
    T receive(); 
    void send(T &&msg);

private:
    // Mutex used to protect the message queue from race conditions
    std::mutex _mutex;

    // Variable to coordinate communication between sender and receiver of queue messages.
    std::condition_variable _cond;

    // The message queue iteself.
    std::deque<T> _messages;
    
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();   

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;

    // Messaging object to communicate the traffic light changing color.
    MessageQueue<TrafficLightPhase> _messages;
};

#endif