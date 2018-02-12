#ifndef __INC_PART_EFI_H_
#define __INC_PART_EFI_H_

#include <types.h>

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL   /* EFI PART */

typedef u16 efi_char16_t;

typedef struct {
        u8 b[16];
} efi_guid_t;

typedef struct _gpt_header {
        u64 signature;
        u32 revision;
        u32 header_size;
        u32 header_crc32;
        u32 reserved1;
        u64 my_lba;
        u64 alternate_lba;
        u64 first_usable_lba;
        u64 last_usable_lba;
        efi_guid_t disk_guid;
        u64 partition_entry_lba;
        u32 num_partition_entries;
        u32 sizeof_partition_entry;
        u32 partition_entry_array_crc32;
} __packed gpt_header;

#define PARTNAME_SZ     (72 / sizeof(efi_char16_t))
typedef struct _gpt_entry {
        efi_guid_t partition_type_guid;
        efi_guid_t unique_partition_guid;
        u64        starting_lba;
        u64        ending_lba;
        u64        attributes; //gpt_entry_attributes attributes;
        efi_char16_t partition_name[PARTNAME_SZ];
} __packed gpt_entry;

#endif
