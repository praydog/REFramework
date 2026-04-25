#pragma once

#include <cstddef>
#include <cstdint>

struct REPakData {
    std::uint8_t unk0[200];
    uintptr_t dstorage_file_ptr;
};

static_assert(offsetof(REPakData, dstorage_file_ptr) == 200, "REPakData::dstorage_file_ptr is not at offset 200");

struct REPakEntryData {
    REPakData* pak_data;
    uint32_t index_in_pak;
};

// Above 0xFFFF0000
static constexpr uint32_t FAKE_INVALID_INDEX_IN_PAK = 0xFFFFDEAD;

struct REUniversalStream;

#pragma pack(push, 1)
struct DStorageDataUploadState {
    std::uint64_t unk0;
    std::uint64_t dest_to_upload;
    std::uint64_t offset_source_data;
    std::uint64_t unk18;
    std::uint8_t unk20[40];
    uintptr_t upload_finish_callback;
    REUniversalStream* stream;
    std::uint8_t unk50[48];
};
#pragma pack(pop)

struct REUniversalStreamVtable {
    uintptr_t unk0[2];
    uint64_t (*read_file)(REUniversalStream *this_ptr, void *buffer, uint64_t size);
    uintptr_t unk18[8];
    uint64_t (*seek_file)(REUniversalStream *this_ptr, uint64_t offset);
};

static_assert(offsetof(REUniversalStreamVtable, read_file) == 16, "read_file offset is incorrect!");
static_assert(offsetof(REUniversalStreamVtable, seek_file) == 88, "seek_file offset is incorrect!");

static_assert(offsetof(DStorageDataUploadState, stream) == 80, "DStorageDataUploadState::stream is not at offset 80");
static_assert(sizeof(DStorageDataUploadState) == 136, "DStorageDataUploadState size is not 136 bytes");

#pragma pack(push, 1)
struct REUniversalStream {
    REUniversalStreamVtable* vtable;
    std::uint8_t unk8[80];
    uintptr_t win32_file_handle;
    DStorageDataUploadState dstorage_upload_state;
    std::uint64_t pak_entry_handle;
    uintptr_t dstorage_file_ptr;
    std::uint8_t unkF8[12];
    uintptr_t in_memory_buffer_ptr;
    std::uint64_t current_offset;
    std::uint8_t gap114[12];
    bool pending_operation_finished;
    std::uint8_t gap121[7];
    std::uint64_t submitted_bytes_count;
    std::uint64_t processed_bytes_count;

    REPakEntryData* get_pak_entry_data() {
        const auto handle_info_ptr = static_cast<uintptr_t>(pak_entry_handle >> 16);
        if (handle_info_ptr == 0) {
            return nullptr;
        }
        return reinterpret_cast<REPakEntryData*>(handle_info_ptr);
    }

    void set_pak_entry_data(REPakEntryData* entry_data) {
        const uint64_t lower_bits = pak_entry_handle & 0xFFFF;
        pak_entry_handle = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(entry_data)) << 16) | lower_bits;
    }
};
#pragma pack(pop)

static_assert(offsetof(REUniversalStream, pak_entry_handle) == 232, "pak_entry_handle offset is incorrect!");
static_assert(offsetof(REUniversalStream, current_offset) == 268, "current_offset is incorrect!");
static_assert(offsetof(REUniversalStream, pending_operation_finished) == 288, "pending_operation_finished offset is incorrect!");
static_assert(offsetof(REUniversalStream, submitted_bytes_count) == 296, "submitted_bytes_count offset is incorrect!");
static_assert(offsetof(REUniversalStream, processed_bytes_count) == 304, "processed_bytes_count offset is incorrect!");

