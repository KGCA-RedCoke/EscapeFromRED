#include "Platform/Window.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

int main(int argc, char** argv)
{
    try
    {
        bool smoke = false;
        for (int i = 1; i < argc; ++i)
        {
            if (std::string_view(argv[i]) == "--smoke-test") smoke = true;
            else throw std::invalid_argument("Usage: PlatformSmoke [--smoke-test]");
        }
        Jacob::Platform::WindowDesc desc;
        desc.title = "Jacob Platform - window/input only";
        desc.size = {640, 360};
        auto window = Jacob::Platform::CreatePlatformWindow(desc);
        unsigned polls = 0;
        while (!window->IsClosed())
        {
            window->PollEvents();
            ++polls;
            if (smoke && polls == 120) window->RequestClose();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        if (smoke && polls != 120) throw std::runtime_error("Window closed before 120 event polls.");
        std::cout << "Platform window completed: " << polls << " event polls (no renderer).\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
