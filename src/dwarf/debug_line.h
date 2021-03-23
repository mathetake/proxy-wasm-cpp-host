#pragma once

#include <string>
#include <vector>

namespace proxy_wasm {
namespace DWARF {

// Minimal implementation of DWARF debug_line section.
// For detail, see section 6.2 of DWARF specification.

class LineNumberPrgoram {
  LineNumberPrgoram(std::string_view debug_line_section)
      : pos_(debug_line_section.begin()), end_(debug_line_section.end()),
        debug_line_section_(debug_line_section){};

  // Parse the header of this line program.
  bool parseHeader();

  // TODO:
  void abort();

private:
  struct FileEntry {
    std::string name;
    uint64_t directory_index;
    // ignore length andlast modification time.
  };

  struct Header {
    uint64_t unit_length = 0;
    uint16_t version = 0;
    uint64_t header_length = 0;
    uint8_t minimum_instruction_length = 0;
    uint8_t maximum_operations_per_instruction = 0;
    bool default_is_stmt = 0;
    int8_t line_base = 0;
    int8_t line_range = 0;
    uint8_t opcode_base = 0;
    std::vector<uint8_t> standard_opcode_lengths{};
    std::vector<std::string> include_directories{};
    std::vector<FileEntry> file_names{};
  };

  Header header_;

  struct StateMachineRegisters {
    StateMachineRegisters(bool default_is_stmt) : is_stmt(default_is_stmt){};
    uint32_t address = 0;
    uint32_t op_index = 0;
    uint32_t file = 1;
    uint32_t line = 1;
    uint32_t column = 1;
    bool is_stmt;
    bool basic_block = false;
    bool end_sequence = false;
    bool prologue_end = false;
    bool epilogue_begin = false;
    uint32_t isa = 0;
    uint32_t discriminator = 0;
  };

  const char* pos_ = nullptr;
  const char* end_ = nullptr;
  std::string_view debug_line_section_;
  bool is_32bit_dwarf_ = false;

  // Utility functions
  uint8_t readUint8();
  uint32_t readUint32();
  uint64_t readUint64();
  uint64_t parseULEB128();
  std::string readString();
};

} // namespace DWARF
} // namespace proxy_wasm
