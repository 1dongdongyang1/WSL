#include "threadpool.hpp"
#include <iostream>

using namespace std;

class PrintTask : public Task {
    int _id;
public:
    PrintTask(int id) : _id(id) {}
    Any run() override {
        std::cout << "Task " << _id << " running in thread "
                  << std::this_thread::get_id() << "\n";
    }
};

int main()
{
    ThreadPool tp;

    for (int i = 1; i <= 10; ++i) {
        tp.submit(std::make_shared<PrintTask>(i));
    }

    return 0;
}