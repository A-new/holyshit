#ifndef _NTDLL_H_
#define _NTDLL_H_

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define WIN32_NO_STATUS
#include "ntstatus.h"

#define _WINTERNL_

#include <windows.h>

#ifdef _NTDDK_
#error NTDLL.H cannot be compiled together with NTDDK.H
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef long KAFFINITY;
	typedef long ULONG_PTR;
	
	typedef enum {
		PowerActionNone = 0,
			PowerActionReserved,
			PowerActionSleep,
			PowerActionHibernate,
			PowerActionShutdown,
			PowerActionShutdownReset,
			PowerActionShutdownOff,
			PowerActionWarmEject
	} POWER_ACTION, *PPOWER_ACTION;
	
	typedef enum _DEVICE_POWER_STATE {
		PowerDeviceUnspecified = 0,
			PowerDeviceD0,
			PowerDeviceD1,
			PowerDeviceD2,
			PowerDeviceD3,
			PowerDeviceMaximum
	} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;
	
	typedef enum _SYSTEM_POWER_STATE {
		PowerSystemUnspecified = 0,
			PowerSystemWorking     = 1,
			PowerSystemSleeping1   = 2,
			PowerSystemSleeping2   = 3,
			PowerSystemSleeping3   = 4,
			PowerSystemHibernate   = 5,
			PowerSystemShutdown    = 6,
			PowerSystemMaximum     = 7
	} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;
	
	typedef enum {
		SystemPowerPolicyAc,
			SystemPowerPolicyDc,
			VerifySystemPolicyAc,
			VerifySystemPolicyDc,
			SystemPowerCapabilities,
			SystemBatteryState,
			SystemPowerStateHandler,
			ProcessorStateHandler,
			SystemPowerPolicyCurrent,
			AdministratorPowerPolicy,
			SystemReserveHiberFile,
			ProcessorInformation,
			SystemPowerInformation,
			ProcessorStateHandler2,
			LastWakeTime,                                   // Compare with KeQueryInterruptTime()
			LastSleepTime,                                  // Compare with KeQueryInterruptTime()
			SystemExecutionState,
			SystemPowerStateNotifyHandler,
			ProcessorPowerPolicyAc,
			ProcessorPowerPolicyDc,
			VerifyProcessorPowerPolicyAc,
			VerifyProcessorPowerPolicyDc,
			ProcessorPowerPolicyCurrent,
			SystemPowerStateLogging,
			SystemPowerLoggingEntry,
			SetPowerSettingValue,
			NotifyUserPowerSetting,
			GetPowerTransitionVetoes,
			SetPowerTransitionVeto,
			SystemVideoState,
			TraceApplicationPowerMessage,
			TraceApplicationPowerMessageEnd,
			ProcessorPerfStates,
			ProcessorIdleStates,
			ProcessorThrottleStates,
			SystemWakeSource,
			SystemHiberFileInformation,
			TraceServicePowerMessage,
			ProcessorLoad,
			PowerShutdownNotification,
			MonitorCapabilities
} POWER_INFORMATION_LEVEL;

typedef long NTSTATUS;
typedef long KPRIORITY;

// Generic test for success on any status value (non-negative numbers indicate success).
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

// Generic test for information on any status value.
#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)

// Generic test for warning on any status value.
#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)

// Generic test for error on any status value.
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

#define MAXIMUM_SUPPORTED_EXTENSION     512

// Define the size of the 80387 save area, which is in the context frame.
#define SIZE_OF_80387_REGISTERS      80

//
// Define the create/open option flags
//

#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020

//
// Valid values for the Attributes field
//

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#define ZwCurrentProcess() NtCurrentProcess()
#define NtCurrentThread()  ((HANDLE)(LONG_PTR)-2)
#define ZwCurrentThread()  NtCurrentThread()

//
// Counted String
//

typedef USHORT RTL_STRING_LENGTH_TYPE;

typedef struct _STRING
{
	USHORT Length;
	USHORT MaximumLength;
#ifdef MIDL_PASS
	[size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
	PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef CONST STRING* PCOEM_STRING;

//
// CONSTCounted String
//

typedef struct _CSTRING
{
	USHORT Length;
	USHORT MaximumLength;
	CONST char *Buffer;
} CSTRING;
typedef CSTRING *PCSTRING;
#define ANSI_NULL ((CHAR)0)	 // winnt

typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

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
typedef const UNICODE_STRING *PCUNICODE_STRING;
#define UNICODE_NULL ((WCHAR)0) // winnt

#if _WIN32_WINNT >= 0x0501

#define UNICODE_STRING_MAX_BYTES ((USHORT) 65534) // winnt
#define UNICODE_STRING_MAX_CHARS (32767) // winnt

#define DECLARE_CONST_UNICODE_STRING(_variablename, _string) \
const WCHAR _variablename ## _buffer[] = _string; \
const UNICODE_STRING _variablename = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWSTR) _variablename ## _buffer };

#endif // _WIN32_WINNT >= 0x0501

#define RTL_CONSTANT_STRING(s) {sizeof(s) - sizeof((s)[0]), sizeof(s), s}

typedef enum _OBJECT_INFORMATION_CLASS
{
	ObjectBasicInformation,		// Result is OBJECT_BASIC_INFORMATION structure
	ObjectNameInformation,		// Result is OBJECT_NAME_INFORMATION structure
	ObjectTypeInformation,		// Result is OBJECT_TYPE_INFORMATION structure
	ObjectAllTypesInformation,	// Result is OBJECT_ALL_INFORMATION structure
	ObjectHandleInformation		// Result is OBJECT_DATA_INFORMATION structure
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;		// Points to type SECURITY_DESCRIPTOR
	PVOID SecurityQualityOfService;	// Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
typedef const OBJECT_ATTRIBUTES *PCOBJECT_ATTRIBUTES;

//++
//
// VOID
// InitializeObjectAttributes(
//     OUT POBJECT_ATTRIBUTES p,
//     IN PUNICODE_STRING n,
//     IN ULONG a,
//     IN HANDLE r,
//     IN PSECURITY_DESCRIPTOR s
//     )
//
//--

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

// Thread Information Classes
typedef enum _THREADINFOCLASS
{
	ThreadBasicInformation,          // 0
	ThreadTimes,                     // 1
	ThreadPriority,                  // 2
	ThreadBasePriority,              // 3
	ThreadAffinityMask,              // 4
	ThreadImpersonationToken,        // 5
	ThreadDescriptorTableEntry,      // 6
	ThreadEnableAlignmentFaultFixup, // 7
	ThreadEventPair_Reusable,        // 8
	ThreadQuerySetWin32StartAddress, // 9
	ThreadZeroTlsCell,               // 10
	ThreadPerformanceCount,          // 11
	ThreadAmILastThread,             // 12
	ThreadIdealProcessor,            // 13
	ThreadPriorityBoost,             // 14
	ThreadSetTlsArrayAddress,        // 15
	ThreadIsIoPending,               // 16
	ThreadHideFromDebugger,          // 17
	ThreadBreakOnTermination,        // 18
	MaxThreadInfoClass               // 19
} THREADINFOCLASS, *PTHREADINFOCLASS;

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

typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS  ExitStatus;
	PNT_TIB   TebBaseAddress;
	CLIENT_ID ClientId;
	KAFFINITY AffinityMask;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	ULONG BasePriority;
	HANDLE UniqueProcessId;
	PVOID Reserved2;
	ULONG HandleCount;
	BYTE Reserved3[4];
	PVOID Reserved4[11];
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER Reserved5[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _PEB_LDR_DATA	// 7 elements, 0x28 bytes (sizeof)
{
/*0x000*/ ULONG Length;
/*0x004*/ UCHAR Initialized;
/*0x005*/ BYTE  _PADDING0_[3];
/*0x008*/ HANDLE SsHandle;
/*0x00C*/ LIST_ENTRY InLoadOrderModuleList;
/*0x014*/ LIST_ENTRY InMemoryOrderModuleList;
/*0x01C*/ LIST_ENTRY InInitializationOrderModuleList;
/*0x024*/ PVOID EntryInProgress;
/*0x028*/ //UCHAR ShutdownInProgress;	// for Vista
/*0x029*/ //BYTE  _PADDING1_[3];		// for Vista
/*0x02C*/ //PVOID ShutdownThreadId;		// for Vista
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	HANDLE         Handle;
} CURDIR, *PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG  TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
/*0x000*/ ULONG MaximumLength;
/*0x004*/ ULONG Length;
/*0x008*/ ULONG Flags;
/*0x00C*/ ULONG DebugFlags;
/*0x010*/ PVOID ConsoleHandle;
/*0x014*/ ULONG ConsoleFlags;
/*0x018*/ PVOID StandardInput;
/*0x01C*/ PVOID StandardOutput;
/*0x020*/ PVOID StandardError;
/*0x024*/ CURDIR CurrentDirectory;
/*0x030*/ UNICODE_STRING DllPath;
/*0x038*/ UNICODE_STRING ImagePathName;
/*0x040*/ UNICODE_STRING CommandLine;
/*0x048*/ PVOID Environment;
/*0x04C*/ ULONG StartingX;
/*0x050*/ ULONG StartingY;
/*0x054*/ ULONG CountX;
/*0x058*/ ULONG CountY;
/*0x05C*/ ULONG CountCharsX;
/*0x060*/ ULONG CountCharsY;
/*0x064*/ ULONG FillAttribute;
/*0x068*/ ULONG WindowFlags;
/*0x06C*/ ULONG ShowWindowFlags;
/*0x070*/ UNICODE_STRING WindowTitle;
/*0x078*/ UNICODE_STRING DesktopInfo;
/*0x080*/ UNICODE_STRING ShellInfo;
/*0x088*/ UNICODE_STRING RuntimeData;
/*0x090*/ RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
/*0x290*/ //ULONG EnvironmentSize;	// for Vista
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS, PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct _PEB_FREE_BLOCK
{
	_PEB_FREE_BLOCK* Next;
	ULONG            Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

typedef struct _PEB *PPEB;

// Basic Process Information, NtQueryInformationProcess using ProcessBasicInfo
typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS  ExitStatus;
	PPEB      PebBaseAddress;
	ULONG_PTR AffinityMask;
	KPRIORITY BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;  // 0x01 = PROTECT_FROM_CLOSE, 0x02 = INHERIT
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef enum _PROCESS_INFORMATION_CLASS
{
	ProcessBasicInformation,
	ProcessQuotaLimits,
	ProcessIoCounters,
	ProcessVmCounters,
	ProcessTimes,
	ProcessBasePriority,
	ProcessRaisePriority,
	ProcessDebugPort,
	ProcessExceptionPort,
	ProcessAccessToken,
	ProcessLdtInformation,
	ProcessLdtSize,
	ProcessDefaultHardErrorMode,
	ProcessIoPortHandlers,
	ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch,
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass,
	ProcessWx86Information,
	ProcessHandleCount,
	ProcessAffinityMask,
	ProcessPriorityBoost,
	MaxProcessInfoClass
} PROCESSINFOCLASS, PROCESS_INFORMATION_CLASS, *PPROCESS_INFORMATION_CLASS;

typedef enum _SYSTEM_HANDLE_TYPE
{
	OB_TYPE_UNKNOWN,
	OB_TYPE_TYPE,
	OB_TYPE_DIRECTORY,
	OB_TYPE_SYMBOLIC_LINK,
	OB_TYPE_TOKEN,
	OB_TYPE_PROCESS,
	OB_TYPE_THREAD,
	OB_TYPE_UNKNOWN_7,
	OB_TYPE_EVENT,
	OB_TYPE_EVENT_PAIR,
	OB_TYPE_MUTANT,
	OB_TYPE_UNKNOWN_11,
	OB_TYPE_SEMAPHORE,
	OB_TYPE_TIMER,
	OB_TYPE_PROFILE,
	OB_TYPE_WINDOW_STATION,
	OB_TYPE_DESKTOP,
	OB_TYPE_SECTION,
	OB_TYPE_KEY,
	OB_TYPE_PORT,
	OB_TYPE_WAITABLE_PORT,
	OB_TYPE_UNKNOWN_21,
	OB_TYPE_UNKNOWN_22,
	OB_TYPE_UNKNOWN_23,
	OB_TYPE_UNKNOWN_24,
	OB_TYPE_IO_COMPLETION,
	OB_TYPE_FILE
} SYSTEM_HANDLE_TYPE, *PSYSTEM_HANDLE_TYPE;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG DataLength;
	UCHAR Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG NameLength;
	WCHAR Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

// Define the base asynchronous I/O argument types
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

// Section Information Structures.
typedef enum _SECTION_INHERIT
{
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT;

typedef struct _SECTION_IMAGE_INFORMATION
{
	PVOID EntryPoint;
	ULONG StackZeroBits;
	ULONG StackReserved;
	ULONG StackCommit;
	ULONG ImageSubsystem;
	WORD  SubsystemVersionLow;
	WORD  SubsystemVersionHigh;
	ULONG Unknown1;
	ULONG ImageCharacteristics;
	ULONG ImageMachineType;
	ULONG Unknown2[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

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

typedef enum _SHUTDOWN_ACTION
{
	ShutdownNoReboot,
	ShutdownReboot,
	ShutdownPowerOff
} SHUTDOWN_ACTION, *PSHUTDOWN_ACTION;

typedef enum _DEBUG_CONTROL_CODE
{
	DebugGetTraceInformation = 1,
	DebugSetInternalBreakpoint,
	DebugSetSpecialCall,
	DebugClearSpecialCalls,
	DebugQuerySpecialCalls,
	DebugDbgBreakPoint
} DEBUG_CONTROL_CODE, *PDEBUG_CONTROL_CODE;

typedef enum _POOL_TYPE
{
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS,
	MaxPoolType,

	//
	// Note these per session types are carefully chosen so that the appropriate
	// masking still applies as well as MaxPoolType above.
	//

	NonPagedPoolSession = 32,
	PagedPoolSession = NonPagedPoolSession + 1,
	NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
	DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
	NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
	PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
	NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,
} POOL_TYPE, *PPOOL_TYPE;

typedef struct _OBJECT_BASIC_INFORMATION
{
	ULONG Attributes;
	ACCESS_MASK GrantedAccess;
	ULONG HandleCount;
	ULONG PointerCount;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
	ULONG Reserved[3];
	ULONG NameInformationLength;
	ULONG TypeInformationLength;
	ULONG SecurityDescriptorLength;
	LARGE_INTEGER CreateTime;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION
{
	UNICODE_STRING Name;
	ULONG ObjectCount;
	ULONG HandleCount;
	ULONG Reserved1[4];
	ULONG PeakObjectCount;
	ULONG PeakHandleCount;
	ULONG Reserved2[4];
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	UCHAR Unknown;
	BOOLEAN MaintainHandleDatabase;
	UCHAR Reserved3[2];
	POOL_TYPE PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_TYPES_INFORMATION
{
	ULONG NumberOfTypes;
	OBJECT_TYPE_INFORMATION TypeInformation;
} OBJECT_ALL_TYPES_INFORMATION, *POBJECT_ALL_TYPES_INFORMATION;

typedef struct _OBJECT_HANDLE_ATTRIBUTE_INFORMATION
{
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_ATTRIBUTE_INFORMATION, *POBJECT_HANDLE_ATTRIBUTE_INFORMATION;

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetList,
	MemorySectionName,
	MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS, *PMEMORY_INFORMATION_CLASS;

typedef enum _SECTION_INFORMATION_CLASS
{
	SectionBasicInformation,
	SectionImageInformation
} SECTION_INFORMATION_CLASS, *PSECTION_INFORMATION_CLASS;

typedef struct _USER_STACK
{
	PVOID FixedStackBase;
	PVOID FixedStackLimit;
	PVOID ExpandableStackBase;
	PVOID ExpandableStackLimit;
	PVOID ExpandableStackBottom;
} USER_STACK, *PUSER_STACK;

typedef VOID (*PKNORMAL_ROUTINE)(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
	);

typedef struct _DEBUG_BUFFER
{
	HANDLE SectionHandle;
	PVOID SectionBase;
	PVOID RemoteSectionBase;
	ULONG SectionBaseDelta;
	HANDLE EventPairHandle;
	ULONG Unknown[2];
	HANDLE RemoteThreadHandle;
	ULONG InfoClassMask;
	ULONG SizeOfInfo;
	ULONG AllocatedSize;
	ULONG SectionSize;
	PVOID ModuleInformation;
	PVOID BackTraceInformation;
	PVOID HeapInformation;
	PVOID LockInformation;
	PVOID Reserved[8];
} DEBUG_BUFFER, *PDEBUG_BUFFER;

typedef enum _WAIT_TYPE
{
	WaitAll,
	WaitAny
} WAIT_TYPE, *PWAIT_TYPE;

typedef enum _TIMER_TYPE
{
	NotificationTimer,
	SynchronizationTimer
} TIMER_TYPE, *PTIMER_TYPE;

typedef VOID (*PTIMER_APC_ROUTINE)(
	IN PVOID TimerContext,
	IN ULONG TimerLowValue,
	IN LONG TimerHighValue
	);

typedef enum _TIMER_INFORMATION_CLASS
{
	TimerBasicInformation
} TIMER_INFORMATION_CLASS, *PTIMER_INFORMATION_CLASS;

typedef enum _EVENT_TYPE
{
	NotificationEvent,
	SynchronizationEvent
} EVENT_TYPE, *PEVENT_TYPE;

typedef enum _SEMAPHORE_INFORMATION_CLASS
{
	SemaphoreBasicInformation
} SEMAPHORE_INFORMATION_CLASS, *PSEMAPHORE_INFORMATION_CLASS;

typedef enum _MUTANT_INFORMATION_CLASS
{
	MutantBasicInformation
} MUTANT_INFORMATION_CLASS, *PMUTANT_INFORMATION_CLASS;

typedef enum _IO_COMPLETION_INFORMATION_CLASS
{
	IoCompletionBasicInformation
} IO_COMPLETION_INFORMATION_CLASS, *PIO_COMPLETION_INFORMATION_CLASS;

//
// Profile source types
//
typedef enum _KPROFILE_SOURCE
{
	ProfileTime,
	ProfileAlignmentFixup,
	ProfileTotalIssues,
	ProfilePipelineDry,
	ProfileLoadInstructions,
	ProfilePipelineFrozen,
	ProfileBranchInstructions,
	ProfileTotalNonissues,
	ProfileDcacheMisses,
	ProfileIcacheMisses,
	ProfileCacheMisses,
	ProfileBranchMispredictions,
	ProfileStoreInstructions,
	ProfileFpInstructions,
	ProfileIntegerInstructions,
	Profile2Issue,
	Profile3Issue,
	Profile4Issue,
	ProfileSpecialInstructions,
	ProfileTotalCycles,
	ProfileIcacheIssues,
	ProfileDcacheAccesses,
	ProfileMemoryBarrierCycles,
	ProfileLoadLinkedIssues,
	ProfileMaximum
} KPROFILE_SOURCE, *PKPROFILE_SOURCE;

typedef struct _PORT_SECTION_WRITE
{
	ULONG Length;
	HANDLE SectionHandle;
	ULONG SectionOffset;
	ULONG ViewSize;
	PVOID ViewBase;
	PVOID TargetViewBase;
} PORT_SECTION_WRITE, *PPORT_SECTION_WRITE;

typedef struct _PORT_SECTION_READ
{
	ULONG Length;
	ULONG ViewSize;
	ULONG ViewBase;
} PORT_SECTION_READ, *PPORT_SECTION_READ;

typedef struct _PORT_MESSAGE
{
	USHORT DataSize;
	USHORT MessageSize;
	USHORT MessageType;
	USHORT VirtualRangesOffset;
	CLIENT_ID ClientId;
	ULONG MessageId;
	ULONG SectionSize;
	//UCHAR Data[];
} PORT_MESSAGE, *PPORT_MESSAGE;

typedef enum _PORT_INFORMATION_CLASS
{
	PortBasicInformation
} PORT_INFORMATION_CLASS, *PPORT_INFORMATION_CLASS;

typedef VOID (NTAPI *PIO_APC_ROUTINE)(
	IN PVOID ApcContext,
	IN PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG Reserved
	);

typedef struct _FILE_FULL_EA_INFORMATION
{
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

typedef struct _FILE_GET_EA_INFORMATION
{
	ULONG NextEntryOffset;
	UCHAR EaNameLength;
	CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;

typedef enum _FSINFOCLASS
{
	FileFsVolumeInformation       = 1,
	FileFsLabelInformation,      // 2
	FileFsSizeInformation,       // 3
	FileFsDeviceInformation,     // 4
	FileFsAttributeInformation,  // 5
	FileFsControlInformation,    // 6
	FileFsFullSizeInformation,   // 7
	FileFsObjectIdInformation,   // 8
	FileFsDriverPathInformation, // 9
	FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_USER_QUOTA_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG SidLength;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER QuotaUsed;
	LARGE_INTEGER QuotaThreshold;
	LARGE_INTEGER QuotaLimit;
	SID Sid[1];
} FILE_USER_QUOTA_INFORMATION, *PFILE_USER_QUOTA_INFORMATION;

typedef struct _FILE_QUOTA_LIST_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG SidLength;
	SID Sid[1];
} FILE_QUOTA_LIST_INFORMATION, *PFILE_QUOTA_LIST_INFORMATION;

//
// Define the various structures which are returned on query operations
//
typedef struct _FILE_BASIC_INFORMATION
{
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

typedef enum _FILE_INFORMATION_CLASS
{
	FileDirectoryInformation         = 1,
	FileFullDirectoryInformation,   // 2
	FileBothDirectoryInformation,   // 3
	FileBasicInformation,           // 4  wdm
	FileStandardInformation,        // 5  wdm
	FileInternalInformation,        // 6
	FileEaInformation,              // 7
	FileAccessInformation,          // 8
	FileNameInformation,            // 9
	FileRenameInformation,          // 10
	FileLinkInformation,            // 11
	FileNamesInformation,           // 12
	FileDispositionInformation,     // 13
	FilePositionInformation,        // 14 wdm
	FileFullEaInformation,          // 15
	FileModeInformation,            // 16
	FileAlignmentInformation,       // 17
	FileAllInformation,             // 18
	FileAllocationInformation,      // 19
	FileEndOfFileInformation,       // 20 wdm
	FileAlternateNameInformation,   // 21
	FileStreamInformation,          // 22
	FilePipeInformation,            // 23
	FilePipeLocalInformation,       // 24
	FilePipeRemoteInformation,      // 25
	FileMailslotQueryInformation,   // 26
	FileMailslotSetInformation,     // 27
	FileCompressionInformation,     // 28
	FileObjectIdInformation,        // 29
	FileCompletionInformation,      // 30
	FileMoveClusterInformation,     // 31
	FileQuotaInformation,           // 32
	FileReparsePointInformation,    // 33
	FileNetworkOpenInformation,     // 34
	FileAttributeTagInformation,    // 35
	FileTrackingInformation,        // 36
	FileIdBothDirectoryInformation, // 37
	FileIdFullDirectoryInformation, // 38
	FileValidDataLengthInformation, // 39
	FileShortNameInformation,       // 40
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef enum _KEY_SET_INFORMATION_CLASS
{
	KeyWriteTimeInformation,
	KeyUserFlagsInformation,
	MaxKeySetInfoClass  // MaxKeySetInfoClass should always be the last enum
} KEY_SET_INFORMATION_CLASS, *PKEY_SET_INFORMATION_CLASS;

typedef enum _KEY_INFORMATION_CLASS
{
	KeyBasicInformation,
	KeyNodeInformation,
	KeyFullInformation,
	KeyNameInformation,
	KeyCachedInformation,
	KeyFlagsInformation,
	MaxKeyInfoClass  // MaxKeyInfoClass should always be the last enum
} KEY_INFORMATION_CLASS, *PKEY_INFORMATION_CLASS;

typedef struct _KEY_VALUE_ENTRY
{
	PUNICODE_STRING ValueName;
	ULONG           DataLength;
	ULONG           DataOffset;
	ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef LANGID *PLANGID;

typedef enum _ATOM_INFORMATION_CLASS
{
	AtomBasicInformation,
	AtomListInformation
} ATOM_INFORMATION_CLASS, *PATOM_INFORMATION_CLASS;

typedef NTSTATUS (NTAPI * PRTL_QUERY_REGISTRY_ROUTINE)(
	IN PWSTR ValueName,
	IN ULONG ValueType,
	IN PVOID ValueData,
	IN ULONG ValueLength,
	IN PVOID Context,
	IN PVOID EntryContext
	);

typedef struct _RTL_QUERY_REGISTRY_TABLE
{
	PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
	ULONG Flags;
	PWSTR Name;
	PVOID EntryContext;
	ULONG DefaultType;
	PVOID DefaultData;
	ULONG DefaultLength;
} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;

typedef struct _RTL_SPLAY_LINKS
{
	_RTL_SPLAY_LINKS *Parent;
	_RTL_SPLAY_LINKS *LeftChild;
	_RTL_SPLAY_LINKS *RightChild;
} RTL_SPLAY_LINKS, *PRTL_SPLAY_LINKS;

typedef enum _RTL_GENERIC_COMPARE_RESULTS
{
	GenericLessThan,
	GenericGreaterThan,
	GenericEqual
} RTL_GENERIC_COMPARE_RESULTS, *PRTL_GENERIC_COMPARE_RESULTS;

typedef RTL_GENERIC_COMPARE_RESULTS (NTAPI *PRTL_GENERIC_COMPARE_ROUTINE)(
	struct _RTL_GENERIC_TABLE *Table,
	PVOID FirstStruct,
	PVOID SecondStruct
	);

typedef PVOID (NTAPI *PRTL_GENERIC_ALLOCATE_ROUTINE)(
	struct _RTL_GENERIC_TABLE *Table,
	ULONG ByteSize
	);

typedef VOID (NTAPI *PRTL_GENERIC_FREE_ROUTINE)(
	struct _RTL_GENERIC_TABLE *Table,
	PVOID Buffer
	);

typedef struct _RTL_GENERIC_TABLE
{
	PRTL_SPLAY_LINKS TableRoot;
	LIST_ENTRY InsertOrderList;
	PLIST_ENTRY OrderedPointer;
	ULONG WhichOrderedElement;
	ULONG NumberGenericTableElements;
	PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine;
	PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine;
	PRTL_GENERIC_FREE_ROUTINE FreeRoutine;
	PVOID TableContext;
} RTL_GENERIC_TABLE, *PRTL_GENERIC_TABLE;

typedef struct _RTL_HANDLE_TABLE_ENTRY
{
	_RTL_HANDLE_TABLE_ENTRY *Next;    /* pointer to next free handle */
	PVOID  Object;
} RTL_HANDLE_TABLE_ENTRY, *PRTL_HANDLE_TABLE_ENTRY;

typedef struct _RTL_HANDLE_TABLE
{
	ULONG MaximumNumberOfHandles;
	ULONG SizeOfHandleTableEntry;
	ULONG Unknown01;
	ULONG Unknown02;
	PRTL_HANDLE_TABLE_ENTRY FreeHandles;
	PRTL_HANDLE_TABLE_ENTRY CommittedHandles;
	PRTL_HANDLE_TABLE_ENTRY UnCommittedHandles;
	PRTL_HANDLE_TABLE_ENTRY MaxReservedHandles;
} RTL_HANDLE_TABLE, *PRTL_HANDLE_TABLE;

typedef struct RTL_HEAP_PARAMETERS
{
	ULONG Length;	//sizeof(RTL_HEAP_PARAMETERS)
	ULONG SegmentReserve;
	ULONG SegmentCommit;
	ULONG DeCommitFreeBlockThreshold;
	ULONG DeCommitTotalFreeThreshold;
	ULONG MaximumAllocationSize;
	ULONG VirtualMemoryThreshold;
	ULONG InitialCommit;
	ULONG InitialReserve;
	PVOID CommitRoutine;
	ULONG Reserved;
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;

//namespace NT {

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetSystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    IN OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemEnvironmentValue(
    IN PUNICODE_STRING Name,
    OUT PVOID Value,
    IN ULONG ValueLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetSystemEnvironmentValue(
    IN PUNICODE_STRING Name,
    IN PUNICODE_STRING Value
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwShutdownSystem(
    IN SHUTDOWN_ACTION Action
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSystemDebugControl(
    IN DEBUG_CONTROL_CODE ControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryObject(
    IN HANDLE ObjectHandle,
    IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
    OUT PVOID ObjectInformation,
    IN ULONG ObjectInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationObject(
    IN HANDLE ObjectHandle,
    IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
    IN PVOID ObjectInformation,
    IN ULONG ObjectInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDuplicateObject(
    IN HANDLE SourceProcessHandle,
    IN HANDLE SourceHandle,
    IN HANDLE TargetProcessHandle,
    OUT PHANDLE TargetHandle OPTIONAL,
    IN ACCESS_MASK DesiredAccess,
    IN ULONG Attributes,
    IN ULONG Options
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMakeTemporaryObject(
    IN HANDLE Handle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwClose(
    IN HANDLE Handle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySecurityObject(
    IN HANDLE Handle,
    IN SECURITY_INFORMATION RequestedInformation,
    OUT PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN ULONG SecurityDescriptorLength,
    OUT PULONG ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetSecurityObject(
    IN HANDLE Handle,
    IN SECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryDirectoryObject(
    IN HANDLE DirectoryHandle,
    OUT PVOID Buffer,
    IN ULONG BufferLength,
    IN BOOLEAN ReturnSingleEntry,
    IN BOOLEAN RestartScan,
    IN OUT PULONG Context,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateSymbolicLinkObject(
    OUT PHANDLE SymbolicLinkHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PUNICODE_STRING TargetName
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSymbolicLinkObject(
    OUT PHANDLE SymbolicLinkHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySymbolicLinkObject(
    IN HANDLE SymbolicLinkHandle,
    IN OUT PUNICODE_STRING TargetName,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAllocateVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN OUT PULONG AllocationSize,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFreeVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG FreeSize,
    IN ULONG FreeType
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryVirtualMemory(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress,
    IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
    OUT PVOID MemoryInformation,
    IN ULONG MemoryInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwLockVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG LockSize,
    IN ULONG LockType
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnlockVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG LockSize,
    IN ULONG LockType
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReadVirtualMemory(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress,
    OUT PVOID Buffer,
    IN ULONG BufferLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteVirtualMemory(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwProtectVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG ProtectSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG FlushSize,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAllocateUserPhysicalPages(
    IN HANDLE ProcessHandle,
    IN PULONG NumberOfPages,
    OUT PULONG PageFrameNumbers
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFreeUserPhysicalPages(
    IN HANDLE ProcessHandle,
    IN OUT PULONG NumberOfPages,
    IN PULONG PageFrameNumbers
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMapUserPhysicalPages(
    IN PVOID BaseAddress,
    IN PULONG NumberOfPages,
    IN PULONG PageFrameNumbers
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMapUserPhysicalPagesScatter(
    IN PVOID *BaseAddresses,
    IN PULONG NumberOfPages,
    IN PULONG PageFrameNumbers
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwGetWriteWatch(
    IN HANDLE ProcessHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN ULONG RegionSize,
    OUT PULONG Buffer,
    IN OUT PULONG BufferEntries,
    OUT PULONG Granularity
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwResetWriteWatch(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress,
    IN ULONG RegionSize
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PLARGE_INTEGER SectionSize OPTIONAL,
    IN ULONG Protect,
    IN ULONG Attributes,
    IN HANDLE FileHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySection(
    IN HANDLE SectionHandle,
    IN SECTION_INFORMATION_CLASS SectionInformationClass,
    OUT PVOID SectionInformation,
    IN ULONG SectionInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwExtendSection(
    IN HANDLE SectionHandle,
    IN PLARGE_INTEGER SectionSize
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN ULONG CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PULONG ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAreMappedFilesTheSame(
    IN PVOID Address1,
    IN PVOID Address2
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN HANDLE ProcessHandle,
    OUT PCLIENT_ID ClientId,
    IN PCONTEXT ThreadContext,
    IN PUSER_STACK UserStack,
    IN BOOLEAN CreateSuspended
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwTerminateThread(
    IN HANDLE ThreadHandle OPTIONAL,
    IN NTSTATUS ExitStatus
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    OUT PVOID ThreadInformation,
    IN ULONG ThreadInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    IN PVOID ThreadInformation,
    IN ULONG ThreadInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSuspendThread(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwResumeThread(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwGetContextThread(
    IN HANDLE ThreadHandle,
    OUT PCONTEXT Context
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetContextThread(
    IN HANDLE ThreadHandle,
    IN PCONTEXT Context
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueueApcThread(
    IN HANDLE ThreadHandle,
    IN PKNORMAL_ROUTINE ApcRoutine,
    IN PVOID ApcContext OPTIONAL,
    IN PVOID Argument1 OPTIONAL,
    IN PVOID Argument2 OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwTestAlert();

NTSYSAPI
NTSTATUS
NTAPI
ZwAlertThread(
    IN HANDLE ThreadHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAlertResumeThread(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRegisterThreadTerminatePort(
    IN HANDLE PortHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwImpersonateThread(
    IN HANDLE ThreadHandle,
    IN HANDLE TargetThreadHandle,
    IN PSECURITY_QUALITY_OF_SERVICE SecurityQos
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwImpersonateAnonymousToken(
    IN HANDLE ThreadHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateProcess(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN HANDLE InheritFromProcessHandle,
    IN BOOLEAN InheritHandles,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcess(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwTerminateProcess(
    IN HANDLE ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationProcess(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationProcess(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    IN PVOID ProcessInformation,
    IN ULONG ProcessInformationLength
    );

NTSTATUS
NTAPI
RtlCreateProcessParameters(
    OUT PPROCESS_PARAMETERS *ProcessParameters,
    IN PUNICODE_STRING ImageFile,
    IN PUNICODE_STRING DllPath OPTIONAL,
    IN PUNICODE_STRING CurrentDirectory OPTIONAL,
    IN PUNICODE_STRING CommandLine OPTIONAL,
    IN ULONG CreationFlags,
    IN PUNICODE_STRING WindowTitle OPTIONAL,
    IN PUNICODE_STRING Desktop OPTIONAL,
    IN PUNICODE_STRING Reserved OPTIONAL,
    IN PUNICODE_STRING Reserved2 OPTIONAL
    );

NTSTATUS
NTAPI
RtlDestroyProcessParameters(
    IN PPROCESS_PARAMETERS ProcessParameters
    );

PDEBUG_BUFFER
NTAPI
RtlCreateQueryDebugBuffer(
    IN ULONG Size,
    IN BOOLEAN EventPair
    );

NTSTATUS
NTAPI
RtlQueryProcessDebugInformation(
    IN ULONG ProcessId,
    IN ULONG DebugInfoClassMask,
    IN OUT PDEBUG_BUFFER DebugBuffer
    );

NTSTATUS
NTAPI
RtlDestroyQueryDebugBuffer(
    IN PDEBUG_BUFFER DebugBuffer
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateJobObject(
    OUT PHANDLE JobHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenJobObject(
    OUT PHANDLE JobHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwTerminateJobObject(
    IN HANDLE JobHandle,
    IN NTSTATUS ExitStatus
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAssignProcessToJobObject(
    IN HANDLE JobHandle,
    IN HANDLE ProcessHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationJobObject(
    IN HANDLE JobHandle,
    IN JOBOBJECTINFOCLASS JobInformationClass,
    OUT PVOID JobInformation,
    IN ULONG JobInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationJobObject(
    IN HANDLE JobHandle,
    IN JOBOBJECTINFOCLASS JobInformationClass,
    IN PVOID JobInformation,
    IN ULONG JobInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateToken(
    OUT PHANDLE TokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN TOKEN_TYPE Type,
    IN PLUID AuthenticationId,
    IN PLARGE_INTEGER ExpirationTime,
    IN PTOKEN_USER User,
    IN PTOKEN_GROUPS Groups,
    IN PTOKEN_PRIVILEGES Privileges,
    IN PTOKEN_OWNER Owner,
    IN PTOKEN_PRIMARY_GROUP PrimaryGroup,
    IN PTOKEN_DEFAULT_DACL DefaultDacl,
    IN PTOKEN_SOURCE Source
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcessToken(
    IN HANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    OUT PHANDLE TokenHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenThreadToken(
    IN HANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN BOOLEAN OpenAsSelf,
    OUT PHANDLE TokenHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDuplicateToken(
    IN HANDLE ExistingTokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN BOOLEAN EffectiveOnly,
    IN TOKEN_TYPE TokenType,
    OUT PHANDLE NewTokenHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFilterToken(
    IN HANDLE ExistingTokenHandle,
    IN ULONG Flags,
    IN PTOKEN_GROUPS SidsToDisable,
    IN PTOKEN_PRIVILEGES PrivilegesToDelete,
    IN PTOKEN_GROUPS SidsToRestricted,
    OUT PHANDLE NewTokenHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAdjustPrivilegesToken(
    IN HANDLE TokenHandle,
    IN BOOLEAN DisableAllPrivileges,
    IN PTOKEN_PRIVILEGES NewState,
    IN ULONG BufferLength,
    OUT PTOKEN_PRIVILEGES PreviousState OPTIONAL,
    OUT PULONG ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAdjustGroupsToken(
    IN HANDLE TokenHandle,
    IN BOOLEAN ResetToDefault,
    IN PTOKEN_GROUPS NewState,
    IN ULONG BufferLength,
    OUT PTOKEN_GROUPS PreviousState OPTIONAL,
    OUT PULONG ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationToken(
    IN HANDLE TokenHandle,
    IN TOKEN_INFORMATION_CLASS TokenInformationClass,
    OUT PVOID TokenInformation,
    IN ULONG TokenInformationLength,
    OUT PULONG ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationToken(
    IN HANDLE TokenHandle,
    IN TOKEN_INFORMATION_CLASS TokenInformationClass,
    IN PVOID TokenInformation,
    IN ULONG TokenInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject(
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSignalAndWaitForSingleObject(
    IN HANDLE HandleToSignal,
    IN HANDLE HandleToWait,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForMultipleObjects(
    IN ULONG HandleCount,
    IN PHANDLE Handles,
    IN WAIT_TYPE WaitType,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN TIMER_TYPE TimerType
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCancelTimer(
    IN HANDLE TimerHandle,
    OUT PBOOLEAN PreviousState OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetTimer(
    IN HANDLE TimerHandle,
    IN PLARGE_INTEGER DueTime,
    IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
    IN PVOID TimerContext,
    IN BOOLEAN Resume,
    IN LONG Period,
    OUT PBOOLEAN PreviousState OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryTimer(
    IN HANDLE TimerHandle,
    IN TIMER_INFORMATION_CLASS TimerInformationClass,
    OUT PVOID TimerInformation,
    IN ULONG TimerInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateEvent(
    OUT PHANDLE EventHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN EVENT_TYPE EventType,
    IN BOOLEAN InitialState
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenEvent(
    OUT PHANDLE EventHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetEvent(
    IN HANDLE EventHandle,
    OUT PULONG PreviousState OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwPulseEvent(
    IN HANDLE EventHandle,
    OUT PULONG PreviousState OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwResetEvent(
    IN HANDLE EventHandle,
    OUT PULONG PreviousState OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwClearEvent(
    IN HANDLE EventHandle
    );

typedef enum _EVENT_INFORMATION_CLASS {
    EventBasicInformation
} EVENT_INFORMATION_CLASS;

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryEvent(
    IN HANDLE EventHandle,
    IN EVENT_INFORMATION_CLASS EventInformationClass,
    OUT PVOID EventInformation,
    IN ULONG EventInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateSemaphore(
    OUT PHANDLE SemaphoreHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN LONG InitialCount,
    IN LONG MaximumCount
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSemaphore(
    OUT PHANDLE SemaphoreHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReleaseSemaphore(
    IN HANDLE SemaphoreHandle,
    IN LONG ReleaseCount,
    OUT PLONG PreviousCount OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySemaphore(
    IN HANDLE SemaphoreHandle,
    IN SEMAPHORE_INFORMATION_CLASS SemaphoreInformationClass,
    OUT PVOID SemaphoreInformation,
    IN ULONG SemaphoreInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateMutant(
    OUT PHANDLE MutantHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN BOOLEAN InitialOwner
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenMutant(
    OUT PHANDLE MutantHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReleaseMutant(
    IN HANDLE MutantHandle,
    OUT PULONG PreviousState
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryMutant(
    IN HANDLE MutantHandle,
    IN MUTANT_INFORMATION_CLASS MutantInformationClass,
    OUT PVOID MutantInformation,
    IN ULONG MutantInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateIoCompletion(
    OUT PHANDLE IoCompletionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG NumberOfConcurrentThreads
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenIoCompletion(
    OUT PHANDLE IoCompletionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetIoCompletion(
    IN HANDLE IoCompletionHandle,
    IN ULONG CompletionKey,
    IN ULONG CompletionValue,
    IN NTSTATUS Status,
    IN ULONG Information
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRemoveIoCompletion(
    IN HANDLE IoCompletionHandle,
    OUT PULONG CompletionKey,
    OUT PULONG CompletionValue,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryIoCompletion(
    IN HANDLE IoCompletionHandle,
    IN IO_COMPLETION_INFORMATION_CLASS IoCompletionInformationClass,
    OUT PVOID IoCompletionInformation,
    IN ULONG IoCompletionInformationLength,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateEventPair(
    OUT PHANDLE EventPairHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenEventPair(
    OUT PHANDLE EventPairHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitLowEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitHighEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetLowWaitHighEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetHighWaitLowEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetLowEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetHighEventPair(
    IN HANDLE EventPairHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemTime(
    OUT PLARGE_INTEGER CurrentTime
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetSystemTime(
    IN PLARGE_INTEGER NewTime,
    OUT PLARGE_INTEGER OldTime OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryPerformanceCounter(
    OUT PLARGE_INTEGER PerformanceCount,
    OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetTimerResolution(
    IN ULONG RequestedResolution,
    IN BOOLEAN Set,
    OUT PULONG ActualResolution
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryTimerResolution(
    OUT PULONG CoarsestResolution,
    OUT PULONG FinestResolution,
    OUT PULONG ActualResolution
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDelayExecution(
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Interval
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwYieldExecution();

NTSYSAPI
ULONG
NTAPI
ZwGetTickCount();

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateProfile(
    OUT PHANDLE ProfileHandle,
    IN HANDLE ProcessHandle,
    IN PVOID Base,
    IN ULONG Size,
    IN ULONG BucketShift,
    IN PULONG Buffer,
    IN ULONG BufferLength,
    IN KPROFILE_SOURCE Source,
    IN ULONG ProcessorMask
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetIntervalProfile(
    IN ULONG Interval,
    IN KPROFILE_SOURCE Source
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryIntervalProfile(
    IN KPROFILE_SOURCE Source,
    OUT PULONG Interval
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwStartProfile(
    IN HANDLE ProfileHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwStopProfile(
    IN HANDLE ProfileHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreatePort(
    OUT PHANDLE PortHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG MaxDataSize,
    IN ULONG MaxMessageSize,
    IN ULONG Reserved
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateWaitablePort(
    OUT PHANDLE PortHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG MaxDataSize,
    IN ULONG MaxMessageSize,
    IN ULONG Reserved
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwConnectPort(
    OUT PHANDLE PortHandle,
    IN PUNICODE_STRING PortName,
    IN PSECURITY_QUALITY_OF_SERVICE SecurityQos,
    IN OUT PPORT_SECTION_WRITE WriteSection OPTIONAL,
    IN OUT PPORT_SECTION_READ ReadSection OPTIONAL,
    OUT PULONG MaxMessageSize OPTIONAL,
    IN OUT PVOID ConnectData OPTIONAL,
    IN OUT PULONG ConnectDataLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSecureConnectPort(
    OUT PHANDLE PortHandle,
    IN PUNICODE_STRING PortName,
    IN PSECURITY_QUALITY_OF_SERVICE SecurityQos,
    IN OUT PPORT_SECTION_WRITE WriteSection OPTIONAL,
    IN PSID ServerSid OPTIONAL,
    IN OUT PPORT_SECTION_READ ReadSection OPTIONAL,
    OUT PULONG MaxMessageSize OPTIONAL,
    IN OUT PVOID ConnectData OPTIONAL,
    IN OUT PULONG ConnectDataLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwListenPort(
    IN HANDLE PortHandle,
    OUT PPORT_MESSAGE Message
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAcceptConnectPort(
    OUT PHANDLE PortHandle,
    IN ULONG PortIdentifier,
    IN PPORT_MESSAGE Message,
    IN BOOLEAN Accept,
    IN OUT PPORT_SECTION_WRITE WriteSection OPTIONAL,
    IN OUT PPORT_SECTION_READ ReadSection OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCompleteConnectPort(
    IN HANDLE PortHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRequestPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE RequestMessage
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRequestWaitReplyPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE RequestMessage,
    OUT PPORT_MESSAGE ReplyMessage
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReplyPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE ReplyMessage
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReplyWaitReplyPort(
    IN HANDLE PortHandle,
    IN OUT PPORT_MESSAGE ReplyMessage
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReplyWaitReceivePort(
    IN HANDLE PortHandle,
    OUT PULONG PortIdentifier OPTIONAL,
    IN PPORT_MESSAGE ReplyMessage OPTIONAL,
    OUT PPORT_MESSAGE Message
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReplyWaitReceivePortEx(
    IN HANDLE PortHandle,
    OUT PULONG PortIdentifier OPTIONAL,
    IN PPORT_MESSAGE ReplyMessage OPTIONAL,
    OUT PPORT_MESSAGE Message,
    IN PLARGE_INTEGER Timeout
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReadRequestData(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE Message,
    IN ULONG Index,
    OUT PVOID Buffer,
    IN ULONG BufferLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteRequestData(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE Message,
    IN ULONG Index,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationPort(
    IN HANDLE PortHandle,
    IN PORT_INFORMATION_CLASS PortInformationClass,
    OUT PVOID PortInformation,
    IN ULONG PortInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwImpersonateClientOfPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE Message
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteFile(
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushBuffersFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCancelIoFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReadFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReadFileScatter(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_SEGMENT_ELEMENT Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteFileGather(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_SEGMENT_ELEMENT Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwLockFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PULARGE_INTEGER LockOffset,
    IN PULARGE_INTEGER LockLength,
    IN ULONG Key,
    IN BOOLEAN FailImmediately,
    IN BOOLEAN ExclusiveLock
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnlockFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PULARGE_INTEGER LockOffset,
    IN PULARGE_INTEGER LockLength,
    IN ULONG Key
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeviceIoControlFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFsControlFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG FsControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwNotifyChangeDirectoryFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PFILE_NOTIFY_INFORMATION Buffer,
    IN ULONG BufferLength,
    IN ULONG NotifyFilter,
    IN BOOLEAN WatchSubtree
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryEaFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PFILE_FULL_EA_INFORMATION Buffer,
    IN ULONG BufferLength,
    IN BOOLEAN ReturnSingleEntry,
    IN PFILE_GET_EA_INFORMATION EaList OPTIONAL,
    IN ULONG EaListLength,
    IN PULONG EaIndex OPTIONAL,
    IN BOOLEAN RestartScan
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetEaFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_FULL_EA_INFORMATION Buffer,
    IN ULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateNamedPipeFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN BOOLEAN TypeMessage,
    IN BOOLEAN ReadmodeMessage,
    IN BOOLEAN Nonblocking,
    IN ULONG MaxInstances,
    IN ULONG InBufferSize,
    IN ULONG OutBufferSize,
    IN PLARGE_INTEGER DefaultTimeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateMailslotFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG CreateOptions,
    IN ULONG Unknown,
    IN ULONG MaxMessageSize,
    IN PLARGE_INTEGER ReadTimeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryVolumeInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID VolumeInformation,
    IN ULONG VolumeInformationLength,
    IN FS_INFORMATION_CLASS VolumeInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetVolumeInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    IN FS_INFORMATION_CLASS VolumeInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryQuotaInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PFILE_USER_QUOTA_INFORMATION Buffer,
    IN ULONG BufferLength,
    IN BOOLEAN ReturnSingleEntry,
    IN PFILE_QUOTA_LIST_INFORMATION QuotaList OPTIONAL,
    IN ULONG QuotaListLength,
    IN PSID ResumeSid OPTIONAL,
    IN BOOLEAN RestartScan
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetQuotaInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_USER_QUOTA_INFORMATION Buffer,
    IN ULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryAttributesFile(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PFILE_BASIC_INFORMATION FileInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryFullAttributesFile(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PFILE_NETWORK_OPEN_INFORMATION FileInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG FileInformationLength,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG FileInformationLength,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryDirectoryFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG FileInformationLength,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN BOOLEAN ReturnSingleEntry,
    IN PUNICODE_STRING FileName OPTIONAL,
    IN BOOLEAN RestartScan
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSaveKey(
    IN HANDLE KeyHandle,
    IN HANDLE FileHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSaveMergedKeys(
    IN HANDLE KeyHandle1,
    IN HANDLE KeyHandle2,
    IN HANDLE FileHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRestoreKey(
    IN HANDLE KeyHandle,
    IN HANDLE FileHandle,
    IN ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwLoadKey(
    IN POBJECT_ATTRIBUTES KeyObjectAttributes,
    IN POBJECT_ATTRIBUTES FileObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwLoadKey2(
    IN POBJECT_ATTRIBUTES KeyObjectAttributes,
    IN POBJECT_ATTRIBUTES FileObjectAttributes,
    IN ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnloadKey(
    IN POBJECT_ATTRIBUTES KeyObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReplaceKey(
    IN POBJECT_ATTRIBUTES NewFileObjectAttributes,
    IN HANDLE KeyHandle,
    IN POBJECT_ATTRIBUTES OldFileObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationKey(
    IN HANDLE KeyHandle,
    IN KEY_SET_INFORMATION_CLASS KeyInformationClass,
    IN PVOID KeyInformation,
    IN ULONG KeyInformationLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG KeyInformationLength,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG KeyInformationLength,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwNotifyChangeKey(
    IN HANDLE KeyHandle,
    IN HANDLE EventHandle OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG NotifyFilter,
    IN BOOLEAN WatchSubtree,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    IN BOOLEAN Asynchronous
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwNotifyChangeMultipleKeys (
    IN HANDLE KeyHandle,
    IN ULONG Flags,
    IN POBJECT_ATTRIBUTES KeyObjectAttributes,
    IN HANDLE EventHandle OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG NotifyFilter,
    IN BOOLEAN WatchSubtree,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    IN BOOLEAN Asynchronous
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG KeyValueInformationLength,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG KeyValueInformationLength,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryMultipleValueKey(
    IN HANDLE KeyHandle,
    IN OUT PKEY_VALUE_ENTRY ValueList,
    IN ULONG NumberOfValues,
    OUT PVOID Buffer,
    IN OUT PULONG Length,
    OUT PULONG ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwPrivilegeCheck(
    IN HANDLE TokenHandle,
    IN PPRIVILEGE_SET RequiredPrivileges,
    OUT PBOOLEAN Result
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwPrivilegeObjectAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN HANDLE TokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN PPRIVILEGE_SET Privileges,
    IN BOOLEAN AccessGranted
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwPrivilegedServiceAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PUNICODE_STRING ServiceName,
    IN HANDLE TokenHandle,
    IN PPRIVILEGE_SET Privileges,
    IN BOOLEAN AccessGranted
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheck(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN HANDLE TokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN PGENERIC_MAPPING GenericMapping,
    IN PPRIVILEGE_SET PrivilegeSet,
    IN PULONG PrivilegeSetLength,
    OUT PACCESS_MASK GrantedAccess,
    OUT PBOOLEAN AccessStatus
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckAndAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN PUNICODE_STRING ObjectTypeName,
    IN PUNICODE_STRING ObjectName,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN ACCESS_MASK DesiredAccess,
    IN PGENERIC_MAPPING GenericMapping,
    IN BOOLEAN ObjectCreation,
    OUT PACCESS_MASK GrantedAccess,
    OUT PBOOLEAN AccessStatus,
    OUT PBOOLEAN GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckByType(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID PrincipalSelfSid,
    IN HANDLE TokenHandle,
    IN ULONG DesiredAccess,
    IN POBJECT_TYPE_LIST ObjectTypeList,
    IN ULONG ObjectTypeListLength,
    IN PGENERIC_MAPPING GenericMapping,
    IN PPRIVILEGE_SET PrivilegeSet,
    IN PULONG PrivilegeSetLength,
    OUT PACCESS_MASK GrantedAccess,
    OUT PULONG AccessStatus
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckByTypeAndAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN PUNICODE_STRING ObjectTypeName,
    IN PUNICODE_STRING ObjectName,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID PrincipalSelfSid,
    IN ACCESS_MASK DesiredAccess,
    IN AUDIT_EVENT_TYPE AuditType,
    IN ULONG Flags,
    IN POBJECT_TYPE_LIST ObjectTypeList,
    IN ULONG ObjectTypeListLength,
    IN PGENERIC_MAPPING GenericMapping,
    IN BOOLEAN ObjectCreation,
    OUT PACCESS_MASK GrantedAccess,
    OUT PULONG AccessStatus,
    OUT PBOOLEAN GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckByTypeResultList(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID PrincipalSelfSid,
    IN HANDLE TokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_TYPE_LIST ObjectTypeList,
    IN ULONG ObjectTypeListLength,
    IN PGENERIC_MAPPING GenericMapping,
    IN PPRIVILEGE_SET PrivilegeSet,
    IN PULONG PrivilegeSetLength,
    OUT PACCESS_MASK GrantedAccessList,
    OUT PULONG AccessStatusList
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckByTypeResultListAndAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN PUNICODE_STRING ObjectTypeName,
    IN PUNICODE_STRING ObjectName,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID PrincipalSelfSid,
    IN ACCESS_MASK DesiredAccess,
    IN AUDIT_EVENT_TYPE AuditType,
    IN ULONG Flags,
    IN POBJECT_TYPE_LIST ObjectTypeList,
    IN ULONG ObjectTypeListLength,
    IN PGENERIC_MAPPING GenericMapping,
    IN BOOLEAN ObjectCreation,
    OUT PACCESS_MASK GrantedAccessList,
    OUT PULONG AccessStatusList,
    OUT PULONG GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAccessCheckByTypeResultListAndAuditAlarmByHandle(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN HANDLE TokenHandle,
    IN PUNICODE_STRING ObjectTypeName,
    IN PUNICODE_STRING ObjectName,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID PrincipalSelfSid,
    IN ACCESS_MASK DesiredAccess,
    IN AUDIT_EVENT_TYPE AuditType,
    IN ULONG Flags,
    IN POBJECT_TYPE_LIST ObjectTypeList,
    IN ULONG ObjectTypeListLength,
    IN PGENERIC_MAPPING GenericMapping,
    IN BOOLEAN ObjectCreation, 
    OUT PACCESS_MASK GrantedAccessList,
    OUT PULONG AccessStatusList,
    OUT PULONG GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenObjectAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID *HandleId,
    IN PUNICODE_STRING ObjectTypeName,
    IN PUNICODE_STRING ObjectName,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN HANDLE TokenHandle,
    IN ACCESS_MASK DesiredAccess,
    IN ACCESS_MASK GrantedAccess,
    IN PPRIVILEGE_SET Privileges OPTIONAL,
    IN BOOLEAN ObjectCreation,
    IN BOOLEAN AccessGranted,
    OUT PBOOLEAN GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCloseObjectAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN BOOLEAN GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteObjectAuditAlarm(
    IN PUNICODE_STRING SubsystemName,
    IN PVOID HandleId,
    IN BOOLEAN GenerateOnClose
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRequestWakeupLatency(
    IN LATENCY_TIME Latency
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRequestDeviceWakeup(
    IN HANDLE DeviceHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCancelDeviceWakeupRequest(
    IN HANDLE DeviceHandle
    );

NTSYSAPI
BOOLEAN
NTAPI
ZwIsSystemResumeAutomatic();

typedef EXECUTION_STATE *PEXECUTION_STATE;

NTSYSAPI
NTSTATUS
NTAPI
ZwSetThreadExecutionState(
    IN EXECUTION_STATE ExecutionState,
    OUT PEXECUTION_STATE PreviousExecutionState
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwGetDevicePowerState(
    IN HANDLE DeviceHandle,
    OUT PDEVICE_POWER_STATE DevicePowerState
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetSystemPowerState(
    IN POWER_ACTION SystemAction,
    IN SYSTEM_POWER_STATE MinSystemState,
    IN ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwInitiatePowerAction(
    IN POWER_ACTION SystemAction,
    IN SYSTEM_POWER_STATE MinSystemState,
    IN ULONG Flags,
    IN BOOLEAN Asynchronous
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwPowerInformation(
    IN POWER_INFORMATION_LEVEL PowerInformationLevel,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwGetPlugPlayEvent(
    IN ULONG Reserved1,
    IN ULONG Reserved2,
    OUT PVOID Buffer,
    IN ULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRaiseException(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT Context,
    IN BOOLEAN SearchFrames
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwContinue(
    IN PCONTEXT Context,
    IN BOOLEAN TestAlert
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwW32Call(
    IN ULONG RoutineIndex,
    IN PVOID Argument,
    IN ULONG ArgumentLength,
    OUT PVOID *Result OPTIONAL,
    OUT PULONG ResultLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCallbackReturn(
    IN PVOID Result OPTIONAL,
    IN ULONG ResultLength,
    IN NTSTATUS Status
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetLowWaitHighThread();

NTSYSAPI
NTSTATUS
NTAPI
ZwSetHighWaitLowThread();

NTSYSAPI
NTSTATUS
NTAPI
ZwLoadDriver(
    IN PUNICODE_STRING DriverServiceName
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnloadDriver(
    IN PUNICODE_STRING DriverServiceName
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushInstructionCache(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress OPTIONAL,
    IN ULONG FlushSize
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushWriteBuffer();

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryDefaultLocale(
    IN BOOLEAN ThreadOrSystem,
    OUT PLCID Locale
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetDefaultLocale(
    IN BOOLEAN ThreadOrSystem,
    IN LCID Locale
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryDefaultUILanguage(
    OUT PLANGID LanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetDefaultUILanguage(
    IN LANGID LanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInstallUILanguage(
    OUT PLANGID LanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAllocateLocallyUniqueId(
    OUT PLUID Luid
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAllocateUuids(
    OUT PLARGE_INTEGER UuidLastTimeAllocated,
    OUT PULONG UuidDeltaTime,
    OUT PULONG UuidSequenceNumber,
    OUT PUCHAR UuidSeed
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetUuidSeed(
    IN PUCHAR UuidSeed
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwRaiseHardError(
    IN NTSTATUS Status,
    IN ULONG NumberOfArguments,
    IN ULONG StringArgumentsMask,
    IN PULONG Arguments,
    IN ULONG MessageBoxType,
    OUT PULONG MessageBoxResult
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetDefaultHardErrorPort(
    IN HANDLE PortHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDisplayString(
    IN PUNICODE_STRING String
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreatePagingFile(
    IN PUNICODE_STRING FileName,
    IN PULARGE_INTEGER InitialSize,
    IN PULARGE_INTEGER MaximumSize,
    IN ULONG Reserved
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAddAtom(
    IN PWSTR String,
    IN ULONG StringLength,
    OUT PUSHORT Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFindAtom(
    IN PWSTR String,
    IN ULONG StringLength,
    OUT PUSHORT Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteAtom(
    IN USHORT Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationAtom(
    IN USHORT Atom,
    IN ATOM_INFORMATION_CLASS AtomInformationClass,
    OUT PVOID AtomInformation,
    IN ULONG AtomInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetLdtEntries(
    IN ULONG Selector1,
    IN LDT_ENTRY LdtEntry1,
    IN ULONG Selector2,
    IN LDT_ENTRY LdtEntry2
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwVdmControl(
    IN ULONG ControlCode,
    IN PVOID ControlData
    );

NTSYSAPI
VOID
NTAPI
RtlAssert(
    IN PVOID VoidFailedAssertion,
    IN PVOID VoidFileName,
    IN ULONG LineNumber,
    IN PSTR MutableMessage
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValues(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PRTL_QUERY_REGISTRY_TABLE QueryTable,
    IN PVOID Context,
    IN PVOID Environment OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlWriteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

NTSYSAPI                                            
NTSTATUS                                            
NTAPI                                               
RtlCharToInteger (
    PCSTR String,
    ULONG Base,
    PULONG Value
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToUnicodeString (
    ULONG Value,
    ULONG Base,
    PUNICODE_STRING String
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlInt64ToUnicodeString (
    IN ULONGLONG Value,
    IN ULONG Base OPTIONAL,
    IN OUT PUNICODE_STRING String
    );

#ifdef _WIN64
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlInt64ToUnicodeString(Value, Base, String)
#else
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlIntegerToUnicodeString(Value, Base, String)
#endif

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeStringFromAsciiz(
    OUT PUNICODE_STRING Destination,
    IN PCSTR Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDuplicateUnicodeString(
    IN  BOOLEAN AllocateNew,
    IN  PUNICODE_STRING SourceString,
    OUT PUNICODE_STRING TargetString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    PUNICODE_STRING Destination,
    PCWSTR Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToInteger (
    IN PUNICODE_STRING String,
    IN ULONG Base OPTIONAL,
    OUT PULONG Value
    );

NTSYSAPI
VOID
NTAPI
RtlInitString(
    PSTRING DestinationString,
    PCSTR SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlInitAnsiString(
    PANSI_STRING DestinationString,
    PCSTR SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

#define RtlInitEmptyUnicodeString(_ucStr,_buf,_bufSize) \
    ((_ucStr)->Buffer = (_buf), \
     (_ucStr)->Length = 0, \
     (_ucStr)->MaximumLength = (USHORT)(_bufSize))


NTSYSAPI
VOID
NTAPI
RtlCopyString(
    PSTRING DestinationString,
    const STRING * SourceString
    );

NTSYSAPI
CHAR
NTAPI
RtlUpperChar (
    CHAR Character
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCompareString(
    const STRING * String1,
    const STRING * String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualString(
    const STRING * String1,
    const STRING * String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
VOID
NTAPI
RtlUpperString(
    PSTRING DestinationString,
    const STRING * SourceString
    );

//
// NLS String functions
//

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteSize(
    OUT PULONG BytesInMultiByteString,
    IN PWCH UnicodeString,
    IN ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCompareUnicodeString(
    PCUNICODE_STRING String1,
    PCUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    PCUNICODE_STRING String1,
    PCUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlHashUnicodeString(
    IN const UNICODE_STRING *String,
    IN BOOLEAN CaseInSensitive,
    IN ULONG HashAlgorithm,
    OUT PULONG HashValue
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    IN PCUNICODE_STRING String1,
    IN PCUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PCUNICODE_STRING Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    PUNICODE_STRING Destination,
    PCWSTR Source
    );

NTSYSAPI
WCHAR
NTAPI
RtlUpcaseUnicodeChar(
    WCHAR SourceCharacter
    );

NTSYSAPI
WCHAR
NTAPI
RtlDowncaseUnicodeChar(
    WCHAR SourceCharacter
    );

NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
    );

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
    );

NTSYSAPI
ULONG
NTAPI
RtlxAnsiStringToUnicodeSize(
    PCANSI_STRING AnsiString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    IN REFGUID Guid,
    OUT PUNICODE_STRING GuidString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGUIDFromString(
    IN PUNICODE_STRING GuidString,
    OUT GUID* Guid
    );

NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemory (
    const VOID *Source1,
    const VOID *Source2,
    SIZE_T Length
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlConvertSidToUnicodeString (
    PUNICODE_STRING UnicodeString,
    PSID Sid,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlIsNameLegalDOS8Dot3 (
    IN PUNICODE_STRING Name,
    IN OUT POEM_STRING OemName OPTIONAL,
    IN OUT PBOOLEAN NameContainsSpaces OPTIONAL
    );

NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError (
   NTSTATUS Status
   );

NTSYSAPI
NTSTATUS
NTAPI
RtlLocalTimeToSystemTime (
    IN PLARGE_INTEGER LocalTime,
    OUT PLARGE_INTEGER SystemTime
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeToSecondsSince1970 (
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    );

NTSYSAPI
VOID
NTAPI
RtlInitializeGenericTable (
    IN PRTL_GENERIC_TABLE Table,
    IN PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine,
    IN PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine,
    IN PRTL_GENERIC_FREE_ROUTINE FreeRoutine,
    IN PVOID TableContext
    );


NTSYSAPI
VOID
NTAPI
RtlInitializeHandleTable(
    IN ULONG MaximumNumberOfHandles,
    IN ULONG SizeOfHandleTableEntry,
    OUT PRTL_HANDLE_TABLE HandleTable
    );


NTSYSAPI
PRTL_HANDLE_TABLE_ENTRY
NTAPI
RtlAllocateHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    OUT PULONG HandleIndex OPTIONAL
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    IN PRTL_HANDLE_TABLE_ENTRY Handle
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlIsValidIndexHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    IN ULONG HandleIndex,
    OUT PRTL_HANDLE_TABLE_ENTRY *Handle
    );


NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTable (
    IN PRTL_GENERIC_TABLE Table,
    IN PVOID Buffer,
    IN LONG BufferSize,
    OUT PBOOLEAN NewElement OPTIONAL
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlIsGenericTableEmpty (
    IN PRTL_GENERIC_TABLE Table
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlIsGenericTableEmpty (
    IN PRTL_GENERIC_TABLE Table
    );
 

NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTable (
    IN PRTL_GENERIC_TABLE Table,
    IN PVOID Buffer
    );


NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableWithoutSplaying(
    IN  PRTL_GENERIC_TABLE Table,
    IN  PVOID *RestartKey
    );

NTSYSAPI
HANDLE
NTAPI
RtlCreateHeap (
    IN ULONG Flags,
    IN PVOID BaseAddress OPTIONAL,
    IN ULONG SizeToReserve,
    IN ULONG SizeToCommit,
    IN BOOLEAN Lock OPTIONAL,
    IN PRTL_HEAP_PARAMETERS Definition OPTIONAL
    );


NTSYSAPI
ULONG
NTAPI
RtlDestroyHeap (
    IN HANDLE HeapHandle
    );


NTSYSAPI
PVOID
NTAPI 
RtlAllocateHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags,
    IN ULONG Size
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags,
    IN PVOID Address
    );


NTSYSAPI
ULONG
NTAPI
RtlCompactHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlLockHeap (
    IN HANDLE HeapHandle
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlUnlockHeap (
    IN HANDLE HeapHandle
    );


NTSYSAPI
PVOID
NTAPI
RtlReAllocateHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags,
    IN PVOID Address,
    IN ULONG Size
    );


NTSYSAPI
ULONG
NTAPI
RtlSizeHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags,
    IN PVOID Address
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlValidateHeap (
    IN HANDLE HeapHandle,
    IN ULONG Flags,
    IN PVOID Address OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor (
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN ULONG Revision
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN BOOLEAN DaclPresent,
    IN PACL Dacl OPTIONAL,
    IN BOOLEAN DaclDefaulted OPTIONAL
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlSetOwnerSecurityDescriptor (
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID Owner OPTIONAL,
    IN BOOLEAN OwnerDefaulted OPTIONAL
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlAllocateAndInitializeSid(
    IN PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    IN UCHAR SubAuthorityCount,
    IN ULONG SubAuthority0,
    IN ULONG SubAuthority1,
    IN ULONG SubAuthority2,
    IN ULONG SubAuthority3,
    IN ULONG SubAuthority4,
    IN ULONG SubAuthority5,
    IN ULONG SubAuthority6,
    IN ULONG SubAuthority7,
    OUT PSID *Sid
    );


NTSYSAPI
ULONG
NTAPI
RtlLengthSid (
    IN PSID Sid
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlEqualSid (
    IN PSID Sid1,
    IN PSID Sid2
    );


NTSYSAPI
PVOID
NTAPI
RtlFreeSid(
    IN PSID Sid
    ); 


NTSYSAPI
NTSTATUS
NTAPI
RtlCreateAcl(
    IN PACL Acl,
    IN ULONG AclLength,
    IN ULONG AclRevision
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedAce(
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedAceEx(
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ULONG AccessMask,
    IN PSID Sid
    );

NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    IN NTSTATUS Status
    );


NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosErrorNoTeb(
    IN NTSTATUS Status
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlGetLastNtStatus();


NTSYSAPI
ULONG
NTAPI
RtlGetLastWin32Error();


NTSYSAPI
VOID
NTAPI
RtlSetLastWin32Error(
    IN ULONG WinError
    );


NTSYSAPI
VOID
NTAPI
RtlSetLastWin32ErrorAndNtStatusFromNtStatus(
    IN NTSTATUS Status
    );

NTSYSAPI
VOID
NTAPI
DbgBreakPoint();

NTSYSAPI
VOID
NTAPI
DbgBreakPointWithStatus(
    IN ULONG Status
    );

NTSYSAPI
ULONG
__cdecl
DbgPrint (
    IN PCH Format,
    ...
    );

NTSYSAPI
ULONG
__cdecl
DbgPrintEx (
    IN ULONG ComponentId,
    IN ULONG Level,
    IN PCH Format,
    ...
    );

NTSYSAPI
ULONG
__cdecl
DbgPrintReturnControlC (
    IN PCHAR Format,
    ...
    );

NTSYSAPI
NTSTATUS
NTAPI
DbgQueryDebugFilterState (
    IN ULONG ComponentId,
    IN ULONG Level
    );

NTSYSAPI
NTSTATUS
NTAPI
DbgSetDebugFilterState (
    IN ULONG ComponentId,
    IN ULONG Level,
    IN BOOLEAN State
    );

NTSYSAPI 
NTSTATUS
NTAPI
LdrLoadDll(
  IN PWCHAR          PathToFile OPTIONAL,
  IN ULONG           Flags OPTIONAL,
  IN PUNICODE_STRING ModuleFileName,
  OUT PHANDLE        ModuleHandle
  );

NTSYSAPI 
NTSTATUS
NTAPI
LdrUnloadDll(
  IN HANDLE ModuleHandle
  );

NTSYSAPI
NTSTATUS
NTAPI
LdrGetDllHandle(
    IN PWSTR DllPath OPTIONAL,
    IN PULONG DllCharacteristics OPTIONAL,
    IN PUNICODE_STRING DllName,
    OUT PVOID * DllHandle
    );


NTSYSAPI
NTSTATUS
NTAPI
LdrGetProcedureAddress(
    IN PVOID DllHandle,
    IN PANSI_STRING ProcedureName OPTIONAL,
    IN ULONG ProcedureNumber OPTIONAL,
    OUT PVOID *ProcedureAddress
    );

#ifdef __cplusplus
}
#endif

//}

#endif /* _NTDLL_H_ */
