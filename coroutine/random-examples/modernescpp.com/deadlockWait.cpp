// https://www.modernescpp.com/index.php/blocking-and-non-blocking

// deadlockWait.cpp

#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex coutMutex;

int main(){

  exit(1);  // GB this demonstrates deadlock *** do not execute.  It hangs.


  std::thread t([]{
    std::cout << "Still waiting ..." << std::endl;            // 2
    std::lock_guard<std::mutex> lockGuard(coutMutex);         // 3
    std::cout << "child: " << std::this_thread::get_id() << std::endl;}
  );

  {

    std::lock_guard<std::mutex> lockGuard(coutMutex);          // 1
    std::cout << "creator: " << std::this_thread::get_id() << std::endl;

    t.join();                                                  // 5

  }                                                            // 4

}
