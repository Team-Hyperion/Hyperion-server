// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_LOGGER_H
#define HYPERION_INCLUDE_NET_NET_LOGGER_H
#pragma once

#include <asio/error.hpp>

#include "core/logger.h"

// Logging macros for networking

///
/// Logs if error is not asio::error::operation_aborted
#define NET_LOG(error__, msg__)                                \
    if ((error__).value() != asio::error::operation_aborted) { \
        LOG_MESSAGE(error, msg__);                             \
    }                                                          \
    else {                                                     \
        LOG_MESSAGE(debug, msg__);                             \
    }                                                          \
    static_assert(true)

///
/// Logs if error is not asio::error::operation_aborted
#define NET_LOG_F(error__, format__, ...)                      \
    if ((error__).value() != asio::error::operation_aborted) { \
        LOG_MESSAGE_F(error, format__, __VA_ARGS__);           \
    }                                                          \
    else {                                                     \
        LOG_MESSAGE_F(debug, format__, __VA_ARGS__);           \
    }                                                          \
    static_assert(true)


#endif // HYPERION_INCLUDE_NET_NET_LOGGER_H
