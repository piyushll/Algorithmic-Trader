//---------------------------------------
// DO NOT MODIFY THIS FILE
//---------------------------------------

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <atomic>
#include <mutex>

const int NUM_STEPS = 100000;
extern std::atomic<int> commonTimer;
extern std::mutex printMutex;

void* workerThread(void* arg) {
    int thread_id = *(int*)arg;
    std::ifstream inputFile("../../../testcases/phase2/inputs/input" + std::to_string(thread_id) + ".txt");
    std::string line;
    for (int step = 0; step < NUM_STEPS; ++step) {
        int currentTime;
        // Perform some work based on the current time
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate work

        // Print thread-exclusive output
        {
            std::lock_guard<std::mutex> lock(printMutex);
            currentTime = commonTimer.load();

            // Increment the shared timer
            if (thread_id == 1)
            commonTimer.fetch_add(1);
            
            std::string str;
            if(!std::getline(inputFile, line)) break;            
            std::cout << currentTime << " " << line << std::endl;
        }
    }
    return NULL;
}
