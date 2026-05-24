#include <exception>
#include <iostream>
#include <format>

#include "ping/application.hpp"

int main(int argc, char** argv)
{
    try
    {
        ping::application(argc, argv).run();
    }

    catch (const std::exception& exception)
    {
        std::cerr << std::format(
            "{}: {}", ping::application::name(), exception.what())
                  << std::endl;

        return 1;
    }
}
