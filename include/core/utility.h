// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_CORE_UTILITY_H
#define HYPERION_INCLUDE_CORE_UTILITY_H
#pragma once

#include <string>
#include <vector>

namespace hyperion::core
{
    ///
    /// Converts provided string to lower case
    [[nodiscard]] std::string StrToLower(std::string str);

    ///
    /// Converts provided string to lower case
    [[nodiscard]] std::string StrToUpper(std::string str);

    ///
    /// Remove specified leading and trailing characters
    [[nodiscard]] std::string StrTrim(std::string str, const std::vector<char>& chars);

    ///
    /// Remove leading and trailing whitespace ' ' and '\0'
    [[nodiscard]] std::string StrTrimWhitespace(const std::string& str);

    ///
    /// If str is less than target_len, it is padded to target_len with char padding
    [[nodiscard]] std::string StrMatchLen(std::string str, std::size_t target_len, char padding = ' ');
} // namespace hyperion::core

#endif // HYPERION_INCLUDE_CORE_UTILITY_H
