#include "defaultprotocol.hpp"

DefaultProtocol::DefaultProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

bool DefaultProtocol::has_message() const {}
std::string return_message() {}

void DefaultProtocol::decode(const uint8_t *, size_t) {}
void DefaultProtocol::encode(const std::string &) {} 



// #pragma once
// #include "iprotocol.hpp"
// #include <queue>
// #include <vector>
// #include <cstdint>
// #include <algorithm>
// #include <stdexcept>
//
// class DefaultProtocol : public IProtocol {
// private:
//     std::vector<uint8_t> buffer_;
//     std::queue<std::string> messages_;
//     static constexpr uint32_t MAX_FRAME = 1 << 20; // 1 MB cap
//
//     // Decode 4-byte network-order length prefix
//     uint32_t decode_length_prefix(const uint8_t* in) {
//         return (uint32_t(in[0]) << 24) |
//                (uint32_t(in[1]) << 16) |
//                (uint32_t(in[2]) << 8) |
//                uint32_t(in[3]);
//     }
//
//     // Encode 4-byte network-order length prefix
//     void encode_length_prefix(uint8_t* out, uint32_t len) {
//         out[0] = (len >> 24) & 0xFF;
//         out[1] = (len >> 16) & 0xFF;
//         out[2] = (len >> 8) & 0xFF;
//         out[3] = len & 0xFF;
//     }
//
// public:
//     void on_bytes(const uint8_t* data, size_t len) override {
//         buffer_.insert(buffer_.end(), data, data + len);
//
//         while (buffer_.size() >= 4) { // enough for length prefix
//             uint32_t msg_len = decode_length_prefix(buffer_.data());
//
//             if (msg_len > MAX_FRAME)
//                 throw std::runtime_error("Message exceeds maximum allowed size");
//
//             if (buffer_.size() < 4 + msg_len)
//                 break; // wait for full message
//
//             std::string msg(buffer_.begin() + 4, buffer_.begin() + 4 + msg_len);
//             messages_.push(std::move(msg));
//
//             // erase processed bytes
//             buffer_.erase(buffer_.begin(), buffer_.begin() + 4 + msg_len);
//         }
//     }
//
//     bool has_message() const override {
//         return !messages_.empty();
//     }
//
//     std::string next_message() override {
//         if (messages_.empty())
//             throw std::runtime_error("No message available");
//
//         auto msg = std::move(messages_.front());
//         messages_.pop();
//         return msg;
//     }
//
//     std::vector<uint8_t> encode(const std::string& msg) override {
//         if (msg.size() > MAX_FRAME)
//             throw std::runtime_error("Message exceeds maximum allowed size");
//
//         std::vector<uint8_t> out(4 + msg.size());
//         encode_length_prefix(out.data(), msg.size());
//         std::copy(msg.begin(), msg.end(), out.begin() + 4);
//         return out;
//     }
// };
