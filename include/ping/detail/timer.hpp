#pragma once

#include <system_error>
#include <type_traits>
#include <functional>
#include <utility>
#include <chrono>

#include <csignal>

namespace ping
{
    class timer final
    {
    public:

        using event_type = std::function<void(std::error_code&)>;

        timer(event_type event) noexcept(
            std::is_nothrow_move_constructible_v<event_type>) :
                event_      {std::move(event)},
                is_expired_ {false}
        {
            struct ::sigaction action;

            action.sa_handler = [](int) { return; };

            ::sigaction(SIGALRM, &action, nullptr);
        }

        timer(const timer&) = delete;

        timer(timer&& other) noexcept(
            std::is_nothrow_move_constructible_v<event_type>) :
                event_      {std::move(other.event_)},
                is_expired_ {other.is_expired_}
        {}

        timer& operator=(const timer&) = delete;

        timer& operator=(timer&& other) noexcept(
            std::is_nothrow_swappable_v<event_type>)
        {
            if (this != &other)
            {
                std::swap(event_, other.event_);
            }

            return *this;
        }

        ~timer()
        {
            struct ::sigaction action;

            action.sa_handler = SIG_DFL;

            ::sigaction(SIGALRM, &action, nullptr);
        }

        void cancel() const noexcept
        {
            ::alarm(0);
        }

        bool is_expired() const noexcept
        {
            return is_expired_;
        }

        void start_countdown(const std::chrono::seconds& seconds)
        {
            if (seconds.count())
            {
                static const auto interrupted = std::make_error_code(
                    std::errc::interrupted);

                std::error_code error;

                ::alarm(seconds.count());

                if (event_(error); error && (error != interrupted))
                {
                    throw std::system_error {error, "timer::start_countdown"};
                }

                is_expired_ = (error == interrupted ? true : false);
            }
        }

    private:

        event_type event_;
        bool       is_expired_;
    };
}
