// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_TIMED_ACTION_H
#define HYPERION_INCLUDE_NET_TIMED_ACTION_H
#pragma once

#include <asio/io_service.hpp>
#include <asio/steady_timer.hpp>
#include <functional>
#include <utility>

#include "hyperion.h"

namespace hyperion::net
{
    class TimedAction
    {
        using TimedAsyncActionT = std::function<void()>;

    public:
        using DurationT = std::chrono::milliseconds;


        explicit TimedAction(const DurationT& duration, TimedAsyncActionT timed_async_action)
            : timer_(ioService_, duration), timedAction_(std::move(timed_async_action)) {}

        TimedAction(const TimedAction& other)     = delete;
        TimedAction(TimedAction&& other) noexcept = delete;


        void Start() {
            timer_.async_wait([this](const asio::error_code& error) { HandleTimeout(error); });

            timedAction_();

            ioService_.run();
        }

    private:
        void Stop() {
            ioService_.stop();
        }

        void HandleTimeout(const asio::error_code& ec) {
            if (ec) {
                LOG_MESSAGE_F(error, "TimedAction timeout error: %s", ec.message().c_str());
            }

            LOG_MESSAGE(debug, "TimedAction timeout");
            Stop();
        }

        asio::io_service ioService_;
        asio::steady_timer timer_;

        TimedAsyncActionT timedAction_;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_TIMED_ACTION_H
