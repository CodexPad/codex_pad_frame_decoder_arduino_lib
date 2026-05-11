#include "robust_frame.h"

#include "crc8.h"

namespace robust_frame {
namespace {
constexpr size_t kMinUnescapedSize = 2;

constexpr uint8_t kFrameHeader = 0xAA;
constexpr uint8_t kFrameFooter = 0x55;
constexpr uint8_t kEscapeChar = 0xDB;
constexpr uint8_t kXorEscape = 0x20;

constexpr bool CheckXorOperator() {
  return ((kFrameHeader ^ kXorEscape) != kFrameHeader && (kFrameHeader ^ kXorEscape) != kFrameFooter && (kFrameHeader ^ kXorEscape) != kEscapeChar) &&
         ((kFrameFooter ^ kXorEscape) != kFrameHeader && (kFrameFooter ^ kXorEscape) != kFrameFooter && (kFrameFooter ^ kXorEscape) != kEscapeChar) &&
         ((kEscapeChar ^ kXorEscape) != kFrameHeader && (kEscapeChar ^ kXorEscape) != kFrameFooter && (kEscapeChar ^ kXorEscape) != kEscapeChar);
}

static_assert(CheckXorOperator(), "XOR operator 0x20 is not safe");
}  // namespace

Parser::Parser(const size_t max_payload_size, FrameCallback callback, void *user_data)
    : max_payload_size_(max_payload_size + 1), callback_(callback), user_data_(user_data), buffer_(new uint8_t[max_payload_size_]) {
}

Parser::~Parser() {
  delete[] buffer_;
}

void Parser::Feed(const uint8_t byte) {
  switch (state_) {
    case State::kSyncHeader: {
      if (byte == kFrameHeader) {
        buffer_len_ = 0;
        state_ = State::kCollectPayload;
      }
      break;
    }
    case State::kCollectPayload: {
      if (byte == kFrameFooter) {
        if (buffer_len_ >= kMinUnescapedSize && crc8::Calculate(buffer_, static_cast<size_t>(buffer_len_ - 1)) == buffer_[buffer_len_ - 1]) {
          if (callback_) {
            callback_(buffer_, user_data_);
          }
        }
        Reset();
      } else if (byte == kEscapeChar) {
        state_ = State::kUnescape;
      } else if (byte == kFrameHeader || buffer_len_ == max_payload_size_) {
        Reset();
        Feed(byte);
      } else if (buffer_len_ < max_payload_size_) {
        buffer_[buffer_len_++] = byte;
      } else {
        Reset();
      }
      break;
    }
    case State::kUnescape: {
      const uint8_t unescaped = byte ^ kXorEscape;
      if (unescaped != kFrameHeader && unescaped != kFrameFooter && unescaped != kEscapeChar) {
        Reset();
        Feed(byte);
      } else if (buffer_len_ < max_payload_size_) {
        buffer_[buffer_len_++] = unescaped;
        state_ = State::kCollectPayload;
      } else {
        Reset();
      }
      break;
    }
    default: {
      Reset();
      break;
    }
  }
}

void Parser::Reset() {
  state_ = State::kSyncHeader;
  buffer_len_ = 0;
}
}  // namespace robust_frame