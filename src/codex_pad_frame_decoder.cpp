#include "codex_pad_frame_decoder.h"

namespace {
bool HasAxisValueChangedSignificantly(const int16_t prev_value, const int16_t current_value, const uint8_t threshold) {
  return prev_value != current_value && (current_value == 0 || current_value == 255 || abs(current_value - prev_value) >= threshold);
}
}  // namespace

CodexPadFrameDecoder::CodexPadFrameDecoder(Stream &stream) : stream_(stream), parser_(sizeof(current_inputs_) + 1, OnFrameReceived, this) {
}

void CodexPadFrameDecoder::OnFrame(const uint8_t *data) {
  if (data != nullptr && data[0] == kPayloadType) {
    memcpy(&current_inputs_, data + 1, sizeof(current_inputs_));
  }
}
void CodexPadFrameDecoder::OnFrameReceived(const uint8_t *data, void *user_data) {
  const auto *decoder = static_cast<CodexPadFrameDecoder *>(user_data);
  if (decoder) {
    decoder->OnFrame(data);
  }
}

void CodexPadFrameDecoder::Update() {
  prev_inputs_ = current_inputs_;

  while (stream_.available() > 0) {
    parser_.Feed(stream_.read());
  }
}

bool CodexPadFrameDecoder::pressed(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) == 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

bool CodexPadFrameDecoder::released(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) != 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) == 0;
}

bool CodexPadFrameDecoder::holding(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) != 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

bool CodexPadFrameDecoder::button_state(const Button button) const {
  return (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

uint32_t CodexPadFrameDecoder::button_states() const {
  return current_inputs_.button_states;
}

uint8_t CodexPadFrameDecoder::axis_value(const Axis axis) const {
  return current_inputs_.axis_values[static_cast<size_t>(axis)];
}

const uint8_t *CodexPadFrameDecoder::axis_values() const {
  return current_inputs_.axis_values;
}

bool CodexPadFrameDecoder::HasAxisValueChanged(const Axis axis, const uint8_t threshold) const {
  return HasAxisValueChangedSignificantly(
      prev_inputs_.axis_values[static_cast<size_t>(axis)], current_inputs_.axis_values[static_cast<size_t>(axis)], threshold);
}