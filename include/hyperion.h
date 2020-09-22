// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_HYPERION_H
#define HYPERION_INCLUDE_HYPERION_H
#pragma once

#include "core/logger.h"

#include <cassert>
#include <cstdint>

// Build type
#ifdef HYPERION_DEBUG_BUILD
#define BUILD_TYPE "Debug"
#else
#define BUILD_TYPE "Release"
#endif

#endif // HYPERION_INCLUDE_HYPERION_H