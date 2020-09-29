// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_CORE_GUARDED_DATA_H
#define HYPERION_INCLUDE_CORE_GUARDED_DATA_H
#pragma once

#include <mutex>

namespace hyperion::core
{
    template <typename T>
    class GuardedAccess;

    ///
    /// Data guarded with mutex for multi-threaded environments
    template <typename T>
    class GuardedData
    {
        using ValueT = T;

        friend GuardedAccess<T>;

    public:
        GuardedData() = default;

        GuardedData(const GuardedData& other)     = delete;
        GuardedData(GuardedData&& other) noexcept = delete;


        [[nodiscard]] ValueT& GetUnsafe() {
            return data_;
        }

        [[nodiscard]] const ValueT& Read() const {
            return data_;
        }

    private:
        ValueT data_{};
        std::mutex mutex_;
    };

    ///
    /// Locks and safely access data within GuardedData
    template <typename T>
    class GuardedAccess
    {
        using GuardedDataT = GuardedData<T>;

    public:
        explicit GuardedAccess(GuardedDataT& guarded_data) : guardedData_(guarded_data) {
            guardedData_.mutex_.lock();
        }

        ~GuardedAccess() {
            guardedData_.mutex_.unlock();
        }

        GuardedAccess(const GuardedAccess& other)     = delete;
        GuardedAccess(GuardedAccess&& other) noexcept = delete;


        typename GuardedDataT::ValueT operator*() const {
            return guardedData_.data_;
        }


        typename GuardedDataT::ValueT* operator->() noexcept {
            return &guardedData_.data_;
        }

        const typename GuardedDataT::ValueT* operator->() const noexcept {
            return &guardedData_.data_;
        }


        GuardedDataT* Get() noexcept {
            return &guardedData_;
        }

        const GuardedDataT* Get() const noexcept {
            return &guardedData_;
        }

    private:
        GuardedDataT& guardedData_;
    };

} // namespace hyperion::core

#endif // HYPERION_INCLUDE_CORE_GUARDED_DATA_H
