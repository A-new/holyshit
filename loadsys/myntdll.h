#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation,              // 0        Y        N
    SystemProcessorInformation,          // 1        Y        N
    SystemPerformanceInformation,        // 2        Y        N
    SystemTimeOfDayInformation,          // 3        Y        N
    SystemNotImplemented1,               // 4        Y        N
    SystemProcessInformation,            // 5        Y        N
    SystemCallCounts,                    // 6        Y        N
    SystemConfigurationInformation,      // 7        Y        N
    SystemProcessorPerformanceInformation,// 8        Y        N
    SystemGlobalFlag,                    // 9        Y        Y
    SystemNotImplemented2,               // 10       Y        N
    SystemModuleInformation,             // 11       Y        N
    SystemLockInformation,               // 12       Y        N
    SystemNotImplemented3,               // 13       Y        N
    SystemNotImplemented4,               // 14       Y        N
    SystemNotImplemented5,               // 15       Y        N
    SystemHandleInformation,             // 16       Y        N
    SystemObjectInformation,             // 17       Y        N
    SystemPagefileInformation,           // 18       Y        N
    SystemInstructionEmulationCounts,    // 19       Y        N
    SystemInvalidInfoClass1,             // 20
    SystemCacheInformation,              // 21       Y        Y
    SystemPoolTagInformation,            // 22       Y        N
    SystemInterruptInformation,          // 23       Y        N
    SystemDpcInformation,                // 24       Y        Y
    SystemNotImplemented6,               // 25       Y        N
    SystemLoadImage,                     // 26       N        Y
    SystemUnloadImage,                   // 27       N        Y
    SystemTimeAdjustment,                // 28       Y        Y
    SystemNotImplemented7,               // 29       Y        N
    SystemNotImplemented8,               // 30       Y        N
    SystemNotImplemented9,               // 31       Y        N
    SystemCrashDumpInformation,          // 32       Y        N
    SystemExceptionInformation,          // 33       Y        N
    SystemCrashDumpStateInformation,     // 34       Y        Y/N
    SystemKernelDebuggerInformation,     // 35       Y        N
    SystemContextSwitchInformation,      // 36       Y        N
    SystemRegistryQuotaInformation,      // 37       Y        Y
    SystemLoadAndCallImage,              // 38       N        Y
    SystemPrioritySeparation,            // 39       N        Y
    SystemNotImplemented10,              // 40       Y        N
    SystemNotImplemented11,              // 41       Y        N
    SystemInvalidInfoClass2,             // 42
    SystemInvalidInfoClass3,             // 43
    SystemTimeZoneInformation,           // 44       Y        N
    SystemLookasideInformation,          // 45       Y        N
    SystemSetTimeSlipEvent,              // 46       N        Y
    SystemCreateSession,                 // 47       N        Y
    SystemDeleteSession,                 // 48       N        Y
    SystemInvalidInfoClass4,             // 49
    SystemRangeStartInformation,         // 50       Y        N
    SystemVerifierInformation,           // 51       Y        Y
    SystemAddVerifier,                   // 52       N        Y
    SystemSessionProcessesInformation,   // 53       Y        N
    SystemLoadGdiDriverInSystemSpace,
    SystemNumaProcessorMap,
    SystemPrefetcherInformation,
    SystemExtendedProcessInformation,
    SystemRecommendedSharedDataAlignment,
    SystemComPlusPackage,
    SystemNumaAvailableMemory,
    SystemProcessorPowerInformation,
    SystemEmulationBasicInformation,
    SystemEmulationProcessorInformation,
    SystemExtendedHandleInformation,
    SystemLostDelayedWriteInformation,
    SystemBigPoolInformation,
    SystemSessionPoolTagInformation,
    SystemSessionMappedViewInformation,
    SystemHotpatchInformation,
    SystemObjectSecurityMode,
    SystemWatchdogTimerHandler,
    SystemWatchdogTimerInformation,
    SystemLogicalProcessorInformation,
    SystemWow64SharedInformation,
    SystemRegisterFirmwareTableInformationHandler,
    SystemFirmwareTableInformation,
    SystemModuleInformationEx,
    SystemVerifierTriageInformation,
    SystemSuperfetchInformation,
    SystemMemoryListInformation,
    SystemFileCacheInformationEx,
    MaxSystemInfoClass	// MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
                         IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
                         OUT PVOID SystemInformation,
                         IN ULONG SystemInformationLength,
                         OUT PULONG ReturnLength OPTIONAL
                         );

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)


#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct _UNICODE_STRING
{
    USHORT Length;	// Unicode字符串的长度，单位字节，不包括'\0'
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

#pragma pack(push)
#pragma pack(1)
typedef struct _LDR_DATA_TABLE_ENTRY	// 18 elements, 0x50 bytes (sizeof) 
{
    /*0x000*/ LIST_ENTRY InLoadOrderLinks;
    /*0x008*/ LIST_ENTRY InMemoryOrderLinks;
    /*0x010*/ LIST_ENTRY InInitializationOrderLinks;
    /*0x018*/ PVOID DllBase;
    /*0x01C*/ PVOID EntryPoint;
    /*0x020*/ ULONG SizeOfImage;
    /*0x024*/ UNICODE_STRING FullDllName;
    /*0x02C*/ UNICODE_STRING BaseDllName;
    /*0x034*/ ULONG Flags;
    /*0x038*/ USHORT LoadCount;
    /*0x03A*/ USHORT TlsIndex;
    union
    {                                                                                  
        /*0x03C*/     LIST_ENTRY HashLinks;
        struct
        {                                                                              
            /*0x03C*/         PVOID SectionPointer;
            /*0x040*/         ULONG CheckSum;
        };
    };
    union
    {                                                                                  
        /*0x044*/     ULONG TimeDateStamp;
        /*0x044*/     PVOID LoadedImports;
    };
    /*0x048*/ PVOID EntryPointActivationContext;
    /*0x04C*/ PVOID PatchInformation;
    /*0x050*/ //LIST_ENTRY ForwarderLinks;	// for Vista
    /*0x058*/ //LIST_ENTRY ServiceTagLinks;	// for Vista
    /*0x060*/ //LIST_ENTRY StaticLinks;		// for Vista
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
#pragma pack(pop)

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
                     PUNICODE_STRING DestinationString,
                     PCWSTR SourceString
                     );
#ifdef __cplusplus
}
#endif