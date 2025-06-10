// chapter 01 threads - C++ Concurrency in Action

// #include <iostream>

// int main() {
//     std::cout << "Hello from chapter_01_threads" << std::endl;
//     return 0;
// }


#include <iostream>
#include <thread>

void hello(){
    std::cout<<"Hello Concurrent World!\n";
}

int main(){
    std::thread t(hello);
    t.join();
}