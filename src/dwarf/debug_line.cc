#include "src/dwarf/debug_line.h"
#include <bits/stdint-uintn.h>

namespace proxy_wasm {
namespace DWARF {


uint64_t LineNumberPrgoram::parseULEB128() {
  uint32_t n = 0;
  uint32_t shift = 0;
  char b;
  do {
    if (pos_ + 1 > end_) {
      abort();
    }
    b = *pos_++;
    n += (b & 0x7f) << shift;
    shift += 7;
  } while ((b & 0x80) != 0);
  return n;
}

uint8_t LineNumberPrgoram::readUint8() {
  if (pos_ == end_) {
    abort();
  }
  return (*pos_++);
}

uint32_t LineNumberPrgoram::readUint32() {
  // Wasm is litte endian.
  uint32_t ret = 0;
  for (auto cnt = 0; cnt < 4; cnt++, pos_++) {
    ret |= *pos_;
    if (pos_ + 1 > end_) {
      abort();
    }
  }
  return ret;
}

uint64_t LineNumberPrgoram::readUint64() {
  // Wasm is litte endian.
  uint64_t ret = 0;
  for (auto cnt = 0; cnt < 8; cnt++, pos_++) {
    ret |= *pos_;
    if (pos_ + 1 > end_) {
      abort();
    }
  }
  return ret;
}

std::string LineNumberPrgoram::readString() {
  std::string ret;
  while (*pos_ != 0) {
    ret.push_back(*pos_++);
    if (pos_ + 1 > end_) {
      abort();
    }
  }
  return ret;
}

bool LineNumberPrgoram::parseHeader() {
  header_.unit_length = readUint32();
  is_32bit_dwarf_ = header_.unit_length == 0xffffffff;
  if (is_32bit_dwarf_) {
    header_.unit_length = readUint64();
  }
  header_.version = readUint8();
  header_.header_length = is_32bit_dwarf_ ? readUint32() : readUint64();
  header_.minimum_instruction_length = readUint8();
  header_.maximum_operations_per_instruction = readUint8();
  header_.default_is_stmt = readUint8();
  header_.line_base = readUint8();
  header_.line_range = readUint8();
  header_.opcode_base = readUint8();
  header_.standard_opcode_lengths.reserve(header_.opcode_base);
  for (auto i = 0; i < header_.opcode_base; i++) {
    header_.standard_opcode_lengths[i] = readUint8();
  }
  do {
    header_.include_directories.push_back(readString());
  } while (*pos_ != 0);
  pos_++;
  do {
    std::string name = readString();
    uint64_t directory_index = readUint64();
    header_.file_names.emplace_back(std::move(name), directory_index);
  } while (*pos_ != 0)
};

} // namespace DWARF
} // namespace proxy_wasm

