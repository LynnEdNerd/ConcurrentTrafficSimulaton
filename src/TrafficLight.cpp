#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
        std::unique_lock<std::mutex> uLock(_mutex);

        // Wait for a message.
        _cond.wait(uLock);

        // remove last element from queue
        T msg = std::move(_messages.back());
        _messages.pop_back();
        // This was needed for the satellite intersections that had the traffic light change phases many times with no vehicles at the intersection.
        // Having received the latest message, clear out the legacy messages from scenarios where no vehicles were at the intersection while the light
        // was changing.
        _messages.clear();

        return msg; // will not be copied due to return value optimization (RVO) in C++

}

template <typename T>
void MessageQueue<T>::send(T &&msg) 
{
        // perform message queue modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        
 //     std::cout << "   Message " << (int)msg << " has been sent to the queue" << std::endl;

        // add message to queue
        _messages.push_back(std::move(msg));

        // notify client after pushing the new message onto the queue.
        _cond.notify_one(); 

}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (1) {
        TrafficLightPhase lightColor = _messages.receive();
        if (lightColor == TrafficLightPhase::green)
            break;
    }
    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // Launch a thread to simulate the traffic light cycling between red and green.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}
 
void TrafficLight::cycleThroughPhases()
{
   while(1)
    {
        int phaseTimeMilliseconds;

        // Select a phase time between 4 and 6 seconds.  NOTE!!! This algorithm of choosing a number between 4000 and 6000 (including both 4000 & 6000) was
        // obtained from Stackoverflow.com.
        phaseTimeMilliseconds = rand()%((6000 - 4000) + 1) + 4000; 

        for (int i = 0; i < phaseTimeMilliseconds; i ++ ) {
            std::this_thread::sleep_for( std::chrono::milliseconds(1) );
        }

        // Simulate the changing of the color.
        if (_currentPhase == TrafficLightPhase::red ) {
            _currentPhase = TrafficLightPhase::green;
        } else {
            _currentPhase = TrafficLightPhase::red;
        }

        // Send a message to the rest of the system that the light has changed color.
        _messages.send(std::move(_currentPhase));

    }
}

