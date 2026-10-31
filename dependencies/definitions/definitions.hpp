#pragma once

#include "../../includes/includes.hpp"

#define rva( ptr, size ) ( ( uintptr_t )ptr + size + *( long* )( ( uintptr_t )ptr + ( size - sizeof( long ) ) ) )
#define offset( ptr, size ) ( ptr + size + *( int32_t *)( ptr + size ) + sizeof( int32_t ) )

extern "C" {
    typedef struct _ldr_data_table_entry_t {
        LIST_ENTRY m_in_load_order_links;
        LIST_ENTRY m_in_memory_order_links;
        LIST_ENTRY m_in_initialization_order_links;

        PVOID m_dll_base;
        PVOID m_entry_point;
        ULONG m_size_of_image;

        UNICODE_STRING m_full_dll_name;
        UNICODE_STRING m_base_dll_name;

        ULONG m_flags;
        WORD m_load_count;
        WORD m_tls_index;

        union {
            LIST_ENTRY m_hash_links;

            struct {
                PVOID m_section_pointer;
                ULONG m_checksum;
            };
        };
        union {
            ULONG m_time_stamp;
            PVOID m_loaded_imports;
        };

        PVOID m_entry_point_ctx;
        PVOID m_patch_info;

        LIST_ENTRY m_forwarded_links;
        LIST_ENTRY m_service_tam_links;
        LIST_ENTRY m_static_links;
    } ldr_data_table_entry_t, * pldr_data_table_entry_t;

    union cross_thread_flags_t {
        uint32_t m_value;

        struct {
            uint32_t m_terminated : 1;
            uint32_t m_thread_inserted : 1;
            uint32_t m_hide_from_debugger : 1;
            uint32_t m_active_info : 1;
            uint32_t m_hard_errors : 1;
            uint32_t m_break_on_termination : 1;
            uint32_t m_skip_creation_msg : 1;
            uint32_t m_skip_terminate_msg : 1;
            uint32_t m_copy_token : 1;
            uint32_t m_thread_io_priority : 3;
            uint32_t m_thread_page_priority : 3;
            uint32_t m_rundown_fail : 1;
            uint32_t m_force_queue : 1;
            uint32_t m_cpu_sets : 1;
            uint32_t m_dynamic_code : 1;
            uint32_t m_case_sensivity : 1;
            uint32_t m_notify_exit : 1;
            uint32_t m_report_active : 1;
            uint32_t m_forced_self_active : 1;
            uint32_t m_coverage : 1;
            uint32_t m_cross_thread_flags : 8;
        };
    };

    typedef struct _rtl_process_modules_info_t {
        HANDLE m_section;
        void* m_mapped_base;

        void* m_image_base;
        uint32_t m_image_size;

        uint32_t m_flags;

        uint16_t m_load_order_index;
        uint16_t m_init_order_index;

        uint16_t m_load_count;

        uint16_t m_offset_to_file_name;
        uint8_t m_full_path_name[ 256 ];
    } rtl_process_modules_info_t, * prtl_process_modules_info_t;

    typedef struct _rtl_process_modules_t {
        uint32_t m_number_of_modules;
        rtl_process_modules_info_t m_modules[ 1 ];
    } rtl_process_modules_t, * prtl_process_modules_t;

    typedef enum _system_information_class_t {
        system_basic_information,
        system_processor_information,
        system_performance_information,
        system_time_of_day_information,
        system_path_information,
        system_process_information,
        system_call_count_information,
        system_device_information,
        system_processor_performance_information,
        system_flags_information,
        system_call_time_information,
        system_module_information,
        system_locks_information,
        system_stack_trace_information,
        system_paged_pool_information,
        system_non_paged_pool_information,
        system_handle_information,
        system_object_information,
        system_page_file_information,
        system_vdm_instemul_information,
        system_vdm_bop_information,
        system_file_cache_information,
        system_pool_tag_information,
        system_interrupt_information,
        system_dpc_behavior_information,
        system_full_memory_information,
        system_load_gdi_driver_information,
        system_unload_gdi_driver_information,
        system_time_adjustment_information,
        system_summary_memory_information,
        system_next_event_id_information,
        system_event_ids_information,
        system_crash_dump_information,
        system_exception_information,
        system_crash_dump_state_information,
        system_kernel_debugger_information,
        system_context_switch_information,
        system_registry_quota_information,
        system_extend_service_table_information,
        system_priority_seperation,
        system_plug_play_bus_information,
        system_dock_information,
        system_processor_speed_information,
        system_current_time_zone_information,
        system_lookaside_information
    } system_information_class_t, * psystem_information_class_t;

    typedef struct _SYSTEM_MODULE {
        ULONG_PTR Reserved[ 2 ];
        PVOID Base;
        ULONG Size;
        ULONG Flags;
        USHORT Index;
        USHORT Unknown;
        USHORT LoadCount;
        USHORT ModuleNameOffset;
        CHAR ImageName[ 256 ];
    } SYSTEM_MODULE, * PSYSTEM_MODULE;

    typedef struct _SYSTEM_MODULE_INFORMATION {
        ULONG_PTR ulModuleCount;
        SYSTEM_MODULE Modules[ 1 ];
    } SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

    typedef union _virt_addr_t {
        uintptr_t m_value;

        struct {
            uintptr_t m_offset : 12;
            uintptr_t m_pt_index : 9;
            uintptr_t m_pd_index : 9;
            uintptr_t m_pdpt_index : 9;
            uintptr_t m_pml4_index : 9;
            uintptr_t m_reserved : 16;
        };
    } virt_addr_t, * pvirt_addr_t;

    typedef struct _mi_active_pfn_t {
        union {
            struct {
                struct {
                    uintptr_t m_tradable : 1;
                    uintptr_t m_non_paged_buddy : 43;
                };
            } m_leaf;

            struct {
                struct {
                    uintptr_t m_tradable : 1;
                    uintptr_t m_wsle_age : 3;
                    uintptr_t m_oldest_wsle_leaf_entries : 10;
                    uintptr_t m_oldest_wsle_leaf_age : 3;
                    uintptr_t m_non_paged_buddy : 43;
                };
            } m_page_table;
            uintptr_t m_entire_active_field;
        };
    } mi_active_pfn_t, * pmi_active_pfn_t;

    typedef struct _mmpte_hardware_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_dirty1 : 1;
            uintptr_t m_owner : 1;
            uintptr_t m_write_through : 1;
            uintptr_t m_cache_disable : 1;
            uintptr_t m_accessed : 1;
            uintptr_t m_dirty : 1;
            uintptr_t m_large_page : 1;
            uintptr_t m_global : 1;
            uintptr_t m_copy_on_write : 1;
            uintptr_t m_unused : 1;
            uintptr_t m_write : 1;
            uintptr_t m_page_frame_number : 40;
            uintptr_t m_reserved_for_software : 4;
            uintptr_t m_wsle_age : 4;
            uintptr_t m_wsle_protection : 3;
            uintptr_t m_no_execute : 1;
        };
    } mmpte_hardware_t, * pmmpte_hardware_t;

    typedef struct _mmpte_prototype_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_demand_fill_proto : 1;
            uintptr_t m_hiber_verify_converted : 1;
            uintptr_t m_read_only : 1;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_combined : 1;
            uintptr_t m_unused1 : 4;
            int64_t m_proto_address : 48;
        };
    } mmpte_prototype_t, * pmmpte_prototype_t;

    typedef struct _mmpte_software_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_page_file_reserved : 1;
            uintptr_t m_page_file_allocated : 1;
            uintptr_t m_cold_page : 1;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_transition : 1;
            uintptr_t m_page_file_low : 4;
            uintptr_t m_used_page_table_entries : 10;
            uintptr_t m_shadow_stack : 1;
            uintptr_t m_unused : 5;
            uintptr_t m_page_file_high : 32;
        };
    } mmpte_software_t, * pmmpte_software_t;

    typedef struct _mmpte_timestamp_t {
        struct {
            uintptr_t m_must_be_zero : 1;
            uintptr_t m_unused : 3;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_transition : 1;
            uintptr_t m_page_file_low : 4;
            uintptr_t m_reserved : 16;
            uintptr_t m_global_time_stamp : 32;
        };
    } mmpte_timestamp_t, * pmmpte_timestamp_t;

    typedef struct _mmpte_transition_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_write : 1;
            uintptr_t m_spare : 1;
            uintptr_t m_io_tracker : 1;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_transition : 1;
            uintptr_t m_page_frame_number : 40;
            uintptr_t m_unused : 12;
        };
    } mmpte_transition_t, * pmmpte_transition_t;

    typedef struct _mmpte_subsection_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_unused0 : 3;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_cold_page : 1;
            uintptr_t m_unused1 : 3;
            uintptr_t m_execute_privilege : 1;
            int64_t m_subsection_address : 48;
        };
    } mmpte_subsection_t, * pmmpte_subsection_t;

    typedef struct _mmpte_list_t {
        struct {
            uintptr_t m_valid : 1;
            uintptr_t m_one_entry : 1;
            uintptr_t m_filler0 : 2;
            uintptr_t m_swizzle_bit : 1;
            uintptr_t m_protection : 5;
            uintptr_t m_prototype : 1;
            uintptr_t m_transition : 1;
            uintptr_t m_filler1 : 16;
            uintptr_t m_next_entry : 36;
        };
    } mmpte_list_t, * pmmpte_list_t;

    typedef struct _mmpte_t {
        union {
            union {
                uintptr_t m_long;
                volatile uintptr_t m_volatile_long;

                struct _mmpte_hardware_t m_hard;
                struct _mmpte_prototype_t m_proto;
                struct _mmpte_software_t m_soft;
                struct _mmpte_timestamp_t m_time_stamp;
                struct _mmpte_transition_t m_trans;
                struct _mmpte_subsection_t m_subsect;
                struct _mmpte_list_t m_list;
            };
        } u;
    } mmpte_t, * pmmpte_t;

    typedef struct _mipfn_blink_t {
        union {
            struct {
                uintptr_t m_blink : 40;
                uintptr_t m_node_blink_low : 19;
                uintptr_t m_tb_flush_stamp : 3;
                uintptr_t m_page_blink_delete_bit : 1;
                uintptr_t m_page_blink_lock_bit : 1;
            };

            struct {
                uintptr_t m_share_count : 62;
                uintptr_t m_page_share_count_delete_bit : 1;
                uintptr_t m_page_share_count_lock_bit : 1;
            };

            uintptr_t m_entire_field;
            volatile int64_t m_lock;

            struct {
                uintptr_t m_lock_not_used : 62;
                uintptr_t m_delete_bit : 1;
                uintptr_t m_lock_bit : 1;
            };
        };
    } mipfn_blink_t, * pmipfn_blink_t;

    typedef struct _mmpfn_entry1_t {
        struct {
            uint8_t m_page_location : 3;
            uint8_t m_write_in_progress : 1;
            uint8_t m_modified : 1;
            uint8_t m_read_in_progress : 1;
            uint8_t m_cache_attribute : 2;
        };
    } mmpfn_entry1_t, * pmmpfn_entry1_t;

    typedef struct _mmpfn_entry3_t {
        struct {
            uint8_t m_priority : 3;
            uint8_t m_on_protected_standby : 1;
            uint8_t m_in_page_error : 1;
            uint8_t m_system_charged_page : 1;
            uint8_t m_removal_requested : 1;
            uint8_t m_parity_error : 1;
        };
    } mmpfn_entry3_t, * pmmpfn_entry3_t;

    typedef struct _mipfn_ulong5_t {
        union {
            uint32_t m_entire_field;

            struct {
                struct {
                    uint32_t m_node_blink_high : 21;
                    uint32_t m_node_flink_middle : 11;
                };
            } standby_list;

            struct {
                uint8_t m_modified_list_bucket_index : 4;
            } mapped_page_list;

            struct {
                struct {
                    uint8_t m_anchor_large_page_size : 2;
                    uint8_t m_spare1 : 6;
                };

                uint8_t m_view_count;
                uint16_t m_spare2;
            } active;
        };
    } mipfn_ulong5_t, * pmipfn_ulong5_t;

    typedef struct _mmpfn_t {
        union {
            struct _LIST_ENTRY m_list_entry;
            struct _RTL_BALANCED_NODE m_tree_node;

            struct {
                union {
                    union {
                        struct _SINGLE_LIST_ENTRY m_next_slist_pfn;
                        void* m_next;

                        struct {
                            uintptr_t m_flink : 40;
                            uintptr_t m_node_flink_low : 24;
                        };
                        struct _mi_active_pfn_t m_active;
                    };
                } u1;

                union {
                    struct _mmpte_t* m_pte_address;
                    uintptr_t m_pte_long;
                };
                struct _mmpte_t m_original_pte;
            };
        };
        struct _mipfn_blink_t u2;

        union {
            union {
                struct {
                    uint16_t m_reference_count;
                    struct _mmpfn_entry1_t e1;
                    struct _mmpfn_entry3_t e3;
                };

                struct {
                    uint16_t m_reference_count;
                } e2;

                struct {
                    uint32_t m_entire_field;
                } e4;
            };
        } u3;
        struct _mipfn_ulong5_t u5;

        union {
            union {
                struct {
                    uintptr_t m_pte_frame : 40;
                    uintptr_t m_resident_page : 1;
                    uintptr_t m_unused1 : 1;
                    uintptr_t m_unused2 : 1;
                    uintptr_t m_partition : 10;
                    uintptr_t m_file_only : 1;
                    uintptr_t m_pfn_exists : 1;
                    uintptr_t m_node_flink_high : 5;
                    uintptr_t m_page_identity : 3;
                    uintptr_t m_prototype_pte : 1;
                };

                uintptr_t m_entire_field;
            };
        } u4;
    } mmpfn_t, * pmmpfn_t;

    typedef struct _get_base_address_t {
        uint32_t m_pid;
        uintptr_t* m_address;
    } get_base_address_t, * pget_base_address_t;

    typedef struct _read_t {
        uint32_t m_pid;

        uintptr_t m_address;
        uintptr_t m_buffer;
        uintptr_t m_size;
    } read_t, * pread_t;

    typedef struct _write_t {
        uint32_t m_pid;

        uintptr_t m_address;
        uintptr_t m_buffer;
        uintptr_t m_size;
    } write_t, * pwrite_t;

    typedef struct _remove_game_protection_t {
        uint32_t m_pid;
        uintptr_t m_address;
    } remove_game_protection_t, * premove_game_protection_t;

    extern "C" PLIST_ENTRY PsLoadedModuleList;
    extern POBJECT_TYPE* IoDriverObjectType;
    extern POBJECT_TYPE* MmSectionObjectType;

    NTSTATUS NTAPI ZwQuerySystemInformation( ULONG, PVOID, ULONG, PULONG );
    PVOID PsGetProcessSectionBaseAddress( PEPROCESS );
    PVOID PsGetProcessPeb( PEPROCESS );
    VOID KeStackAttachProcess( PEPROCESS, PKAPC_STATE );
    VOID KeUnstackDetachProcess( PKAPC_STATE );
    CHAR* PsGetProcessImageFileName( PEPROCESS );
    ULONG KeCapturePersistentThreadState( PCONTEXT, PKTHREAD, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID );
    NTSTATUS ObReferenceObjectByName( PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID, PDRIVER_OBJECT* );
}