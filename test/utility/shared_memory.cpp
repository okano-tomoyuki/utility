#include <string>
#include <iostream>
#include <thread>
#include <chrono>

#include "utility/shared_memory.hpp"

struct A
{
    double b;
    double c;
    int a;
};

int main()
{
    auto sm = Utility::SharedMemory<A>("AAAA", "BBBB");

    int counter=100;
    while(counter--)
    {
        if(sm.wait_for_single_object(sm.mutex(), 30))
        {
            std::cout << "lock" << std::endl;
            sm.get()->a++;
            sm.get()->b++;
            sm.get()->c++;
            std::cout << sm.get()->a << std::endl;
            std::cout << sm.get()->b << std::endl;
            std::cout << sm.get()->c << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            sm.release_mutex(sm.mutex());
            std::cout << "unlock" << std::endl;
        }
        else
        {
            std::cout << "lock failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}