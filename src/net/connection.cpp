// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/connection.h"

#include <asio/read.hpp>
#include <asio/write.hpp>

#include "media/save.h"
#include "net/comm_format.h"
#include "net/logger.h"

using namespace hyperion;

void net::ConnectionBase::BeginOutFiles(std::string file_directory_path) {
    outFileDirectory_ = std::move(file_directory_path);
    outFilePath_      = std::move(MakeOutFilePath());

    outFile_ = std::move(media::MakeSaveFile(outFilePath_));
    outFile_.close();
}

std::ofstream& net::ConnectionBase::OpenOutFile() {
    assert(!outFilePath_.empty()); // BeginOutFiles was not called
    assert(!outFile_.is_open());   // Forgot to close outFile_ earlier

    // Open existing or make new file
    outFile_.open(outFilePath_, std::ios::app | std::ios::binary);
    if (!outFile_.is_open()) {
        outFile_ = std::move(media::MakeSaveFile(outFilePath_));
    }

    if (!outFile_.is_open()) {
        LOG_MESSAGE_F(error, "Failed to open out file at: %s", outFilePath_.c_str());
    }

    return outFile_;
}

void net::ConnectionBase::FinishOutFile() {
    outFile_.close();
}

std::string net::ConnectionBase::MakeOutFilePath() const {
    std::string file_ext;

    switch (mediaProp.GetType()) {
    case media::MediaType::image:
        file_ext = media::MediaTypeExt::kImage;
        break;
    case media::MediaType::video:
        file_ext = media::MediaTypeExt::kVideo;
        break;
    default:
        assert(false);
    }

    std::string save_name;

    // Also include forward slash only if directory is provided
    // prevents trying to write to root directory when empty
    if (!outFileDirectory_.empty()) {
        save_name.append("/");
    }
    save_name += std::to_string(id) + "." + file_ext;

    return outFileDirectory_ + save_name;
}

// ======================================================================

net::Connection::Connection(SocketT&& socket) : socket_(std::move(socket)), timer_(socket_.get_executor()) {
    assert(socket_.is_open()); // Socket should be open to receive from client

    LOG_MESSAGE_F(
        info, "Created connection %llu, %s", id, this->socket_.remote_endpoint().address().to_string().c_str());
}

net::Connection::~Connection() {
    assert(CanDestruct());
}

bool net::Connection::CanDestruct() const noexcept {
    return refCount_ == 0;
}

void net::Connection::End() noexcept {
    try {
        FinishOutFile();

        if (!receivedEof_) { // Suppresses exception when attempting to shutdown already disconnected socket
            LOG_MESSAGE(debug, "Calling shutdown on connected socket");
            socket_.shutdown(asio::socket_base::shutdown_both);
        }
        else {
            LOG_MESSAGE(debug, "Skipping shutdown on disconnected socket");
        }

        socket_.close();

        LOG_MESSAGE_F(info, "Connection %llu closed", id);
    }
    catch (std::exception& e_close) {
        LOG_MESSAGE_F(error, "Failed to close socket: %s", e_close.what());
    }
}

void net::Connection::AsyncWrite(
    const ByteVector& msg,
    std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    IncRefCount();

    async_write(
        socket_,
        asio::buffer(msg),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(debug, "Async write ec: %s", error.message().c_str());
                return;
            }

            callback(error, bytes_transferred);

            DecRefCount();
        });
}

void net::Connection::AsyncRead(
    const std::size_t n, std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    IncRefCount();

    async_read(
        socket_,
        buf_.prepare(n),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(debug, "Async read ec: %s", error.message().c_str());
            }

            LOG_MESSAGE_F(debug, "Read %llu bytes", bytes_transferred);

            callback(error, bytes_transferred);

            DecRefCount();
        });
}

void net::Connection::AsyncReceive(
    const std::size_t n, std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    IncRefCount();

    socket_.async_receive(
        buf_.prepare(n),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                if (error.value() == asio::error::eof) {
                    receivedEof_ = true;
                }

                LOG_MESSAGE_F(debug, "Async read ec: %s", error.message().c_str());
            }

            callback(error, bytes_transferred);

            DecRefCount();
        });
}

void net::Connection::AsyncWait(std::function<void(const asio::error_code& error)>&& callback) {
    IncRefCount();

    timer_.async_wait([this, callback{std::move(callback)}](const asio::error_code& error) {
        if (error) {
            LOG_MESSAGE_F(debug, "Async wait ec: %s", error.message().c_str());
        }

        callback(error);

        DecRefCount();
    });
}

void net::Connection::IncRefCount() noexcept {
    refCount_++;

    if (refCount_ == 1) {
        LOG_MESSAGE_F(debug, "Connection %llu has first reference", id);
    }
}
void net::Connection::DecRefCount() noexcept {
    assert(refCount_ != 0);

    refCount_--;

    if (refCount_ == 0) {
        LOG_MESSAGE_F(debug, "Connection %llu has zero references", id);
    }
}
