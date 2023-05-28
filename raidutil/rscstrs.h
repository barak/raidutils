/* Copyright (c) 1996-2004, Adaptec Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Adaptec Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __RSCSTRS_H
#define __RSCSTRS_H
/******************************************
 ** FILE: RSCENUM.H
 **
 ** DATE: 6/23/1999
 ******************************************/

/******************************************
 ** STRINGS                              **
 ******************************************/

char * EventStrings[] =
{
//    "Distributed Processing Technology",     // STR_DPT = 0,
//    "Copyright 1993-2000 All Rights Reserved",// STR_COPYRIGHT = 1,
//    "DPT Utility",                           // STR_PROG_TITLE = 2,
//    "Version "                               // STR_VERSION = 3,
   "dptutil", // STR_DPTUTIL
   "raidutil", // STR_RAIDUTIL
   "DPT Util", // STR_DPT_UTIL
   "Raid Util", // STR_RAID_UTIL
   "DPTUTIL  ", // STR_DPTUTIL_ALL_CAPS
   "RAIDUTIL  ", // STR_RAIDUTIL_ALL_CAPS
   "DPT ENGINE  ", // STR_DPT_ENGINE
   "Adaptec ENGINE  ", // STR_ADPT_ENGINE
   "Version: %d.%c%c  Date: %d/%d/%d  %s", // STR_VERSION_DATE
   "%2d/%02d/%04d", // STR_FILLIN_DATE
   "V%s%s%s", // STR_FILLIN_VERSION
   "The dptutil allows command-line access to administer the RAID configuration.", // STR_DPTUTIL_SHORT_DESCRIP
   "The raidutil allows command-line access to administer the RAID configuration.", // STR_RAIDUTIL_SHORT_DESCRIP
   "Program Description:", // STR_USAGE_PROGRAM_DESC_TITLE
   "Switch definitions:  ~", // STR_USAGE_SW_TITLE
   "-g d,d[,d][+d,d][d]...  @Create logical drive, where \"d\" is a device id.  When creating RAID 10 or 50, separate parity groups with +", // STR_USAGE_SW_CREATE_LOGICAL
   "-g b [n] [d[,d,..]]@Create logical drive, where \"b\" is an hba or an hba\\bus; \"n\" is the number of drives in each array; \"d\" is a list of device ids to exclude from the array(s).", // STR_USAGE_SW_CREATE_LOGICAL2
   "-i@Causes non-fatal errors to be ignored when creating logical drives", // STR_USAGE_SW_IGNORE_NONFATAL_ERRORS
   "-D [d|all]@Delete logical drive, where \"d\" is a device id or all", // STR_USAGE_SW_DELETE_LOGICAL
	"-h d@Create hot-spare, where \"d\" is a device id", // STR_USAGE_SW_CREATE_HOT_SPARE
	"-H d@Delete hot-spare, where \"d\" is a device id", // STR_USAGE_SW_DELETE_HOT_SPARE
	"-A [on|off|enable|disable|?|-?]@Alarm Status and Control", //STR_USAGE_SW_ALARM
	"-a [build|rebuild|verify|stop|list|?|-?] d[,d]@Task status and control", //STR_USAGE_SW_ACTION_TASK_CONTROL
	"-E d d[,d]...@Expand array d with drives d[,d]... Valid only under Windows NT and Windows 2000", //STR_USAGE_EXPAND_ARRAY
	"-C load|save filename@Load/Save Configuration from/to file", //STR_USAGE_LOAD_SAVE_CONFIGURATION
   "-w [on|off|?|-?] d,d[,d]@Write Caching control (on = Write-back, off = write-through)", //STR_USAGE_SW_WRITE_CACHING
	"-Z d[,d]...  @Clear the RAID table information on selected components, where \"d\" is a device id", //STR_USAGE_SW_RESET_CLEAR_RAID_STORE
   "-X @Reset the firmware's configuration in NVRAM to defaults", //STR_USAGE_SW_RESET_NVRAM_CONFIG
	"-L physical@List physical devices", //STR_USAGE_SW_LIST_DEVICES_PHYSICAL
	"-L logical@List logical devices", //STR_USAGE_SW_LIST_DEVICES_LOGICAL
 	"-L controller@List controllers", //STR_USAGE_SW_LIST_DEVICES_CONTROLLER
	"-L spare@List hot spare devices", //STR_USAGE_SW_LIST_DEVICES_SPARE
	"-L raid|array@List array organization", //STR_USAGE_SW_LIST_DEVICES_RAID
	"-L speed@List speed of devices", //STR_USAGE_SW_LIST_DEVICES_SPEED
	"-L cache@List write cache mode of LSUs", //STR_USAGE_SW_LIST_DEVICES_CACHE
	"-L version@List controller FW/NVRAM/BIOS/SMOR versions", //STR_USAGE_SW_LIST_DEVICES_VERSION
  	"-L redirect@List redirected devices", //STR_USAGE_SW_LIST_DEVICES_REDIRECT
	"-L battery@List battery information", //STR_USAGE_SW_LIST_BATTERY
	"-L all@List all types", //STR_USAGE_SW_LIST_DEVICES_ALL
   "-I [d]@Display inquiry info on the controller specified.  If \"d\" is specified, it is the device id of the device whose inquiry info is to be displayed", //STR_USAGE_SW_DISPLAY_INQ_INFO
	"-q@Inhibits output (must precede output-generating command)", //STR_USAGE_SW_QUIET_MODE
	"-P@Forces display to pagenate output (must precede output-generating command)", //STR_USAGE_SW_PAGENATION
	"-?@displays this screen", //STR_USAGE_SW_SHOW_UTIL_USAGE
	"-d n@The utility-relative controller number on which the following commands apply", //STR_USAGE_SW_SPECIFY_DPT
	"-c n@The solaris controller number on which the following commands apply", //STR_USAGE_SW_SPECIFY_CTLR
	"-l [0|1|5|10|50]@The logical to create is of this raid level", //STR_USAGE_SW_RAID_LEVEL
   "-s n@Specify the capacity in megabytes of the generated array", //STR_USAGE_SW_LOGICAL_DRIVE_CAPACITY
 	"-z n@The stripe size to use in the creation of the next logical drive", //STR_USAGE_SW_LOGICAL_DRIVE_STRIPE_SIZE
	"-r [slow|medslow|med|medfast|fast|?|-?] d[,d...]@Set the rebuild rate", //STR_USAGE_SW_TASK_RATE
	"-e [soft|recov|nonrecov|status|delete|board|?|-?] d@View the controller's event log", //STR_USAGE_VIEW_LOG
	"-f [optimal|fail|?|-?] d@Force an array member drive to Failed state, or force an array to Optimal state", //STR_USAGE_FORCE_STATE
	"-K@Report on cluster support enabled/disabled", //STR_USAGE_CLUSTER_SUPPORT
	"Command Usage Examples:                ~", //STR_USAGE_SAMPLE_TITLE
	"-q@Quiet mode", //STR_USAGE_SAMPLE_QUIET_MODE
	"-?@Display usage screen", //STR_USAGE_SAMPLE_SHOW_USAGE
#if !defined _DPT_SOLARIS
   "-d n [-s n][-z n][-l n]-g d[,d[,d...]]@Create logical", //STR_USAGE_SAMPLE_CREATE_LOGICAL
	"-d n -D d@Delete logical", //STR_USAGE_SAMPLE_DELETE_LOGICAL
	"-d n [-r n] -h d@Create hot spare", //STR_USAGE_SAMPLE_CREATE_HOT_SPARE
	"-d n -H d@Delete hot spare", //STR_USAGE_SAMPLE_DELETE_HOT_SPARE
   "-d n -L physical@List all physical devices", //STR_USAGE_SAMPLE_LIST_DEVICES
 	"-d n -I [d]@Display inquiry information", //STR_USAGE_SAMPLE_DISPLAY_INQUIRY_INFO
	"-d n -X@Reset NVRAM to firmware defaults on the selected HBA", //STR_USAGE_SAMPLE_RESET_NVRAM
	"\td#b#t#d#, which corresponds to controller, bus, target id and lun respectively.  Note:  The target id may also be specified as  digits (d#b#t##d#).", //STR_USAGE_LEGEND_2
   "\nOperation successful, a reboot is required before the actions will take affect.\n", // STR_REBOOT
	"-F filename@Flash the referenced controller's firmware, fcode, bios, smor or nvram flash regions with the supplied image - for a path use / instead of \\", //STR_USAGE_SW_FLASH
#else
	"-c n [-s n][-z n][-l n]-g d[,d[,d...]]@Create logical", //STR_USAGE_SAMPLE_CREATE_LOGICAL
   "-c n -D n@Delete logical", //STR_USAGE_SAMPLE_DELETE_LOGICAL
	"-c n [-r n] -h n@Create hot spare", //STR_USAGE_SAMPLE_CREATE_HOT_SPARE
	"-c n -H n@Delete hot spare", //STR_USAGE_SAMPLE_DELETE_HOT_SPARE
	"-c n -L physical@List all physical devices", //STR_USAGE_SAMPLE_LIST_DEVICES
 	"-c n -I [d]@Display inquiry information", //STR_USAGE_SAMPLE_DISPLAY_INQUIRY_INFO
   "-c n -X@Reset NVRAM to firmware defaults on the selected HBA", //STR_USAGE_SAMPLE_RESET_NVRAM
	"\tc#t#d#, which corresponds to controller, target id and lun respectively", //STR_USAGE_LEGEND_2
   "\nOperation successful, a reboot is required before the actions will take affect.\nPlease use `boot -r' at Ok boot prompt.\n", // STR_REBOOT
	"-F image@Flash the referenced controller's firmware, fcode, bios, smor or nvram flash regions with the supplied image", // STR_USAGE_SW_FLASH
#endif
   "Notes:", //STR_USAGE_SYNTAX
	"Device id is of the form:", //STR_USAGE_LEGEND_1
   "~#  ~b0 ~b1 ~b2    ~Controller      ~Alarm Status  ~Alarm Enable\n", // STR_ALARM_STATUS_HEADER
	"~Address    ~Type              ~Manufactu~rer/Model         ~Capacity  ~Status\n", // STR_LIST_DEV_PHYS_HEADER
	"~Address       ~Type              ~Manufactu~rer/Model      ~Capacity  ~Status\n", // STR_LIST_DEV_ARRAY_HEADER
	"~Address    ~Type              ~Status\n", // STR_LIST_DEV_STATUS_HEADER
	"~Address    ~Capacity  ~\n", // STR_LIST_DEV_HOT_SPARE_HEADER
// 	"~#  ~Logical Drive Name     ~Address   ~Capacity   ~Status  ~Type\n", // STR_LIST_DEV_LOG_HEADER
 	"~#  ~b0 ~b1 ~b2  ~Cont~roller     ~Cache  ~FW    ~NVRAM     ~Serial     ~Status\n", // STR_LIST_DEV_CTLR_HEADER
 	"~#  ~Cont~roller      ~Status     ~Voltage  ~Current  ~Full Cap  ~Rem Cap  R~em Time\n", // STR_LIST_BATTERY_HEADER
 	"~#  ~Cont~roller     ~Cache  ~FW    ~NVRAM     ~BIOS   ~SMOR      ~Serial\n", // STR_LIST_DEV_VERSION_HEADER
 	"~Address    ~Type                        ~Rate\n", // STR_LIST_DEV_SET_RATE_HEADER
 	"~Tag   ~Address    ~Type\n", // STR_LIST_TAGS_HEADER
 	"~#  ~Channel  ~Cont~roller        ~Speed\n", // STR_BUS_SPEED_HEADER
   "~Address    ~Max Speed  ~Actual Rate / ~Width\n", // STR_LIST_DEV_SPEED_HEADER
 	"~Address      ~Task Type / Status\n", // STR_LIST_TASKS
   "~#  ~Cont~roller     ~FW    ~Cache Stale Period (secs)\n", // STR_LIST_CACHE_STALE_HEADER
 	"~Address    ~Manufactu~rer/Model        ~Write Cache Mode\n", // STR_LIST_CACHE_HEADER
 	"~Address    ~Manufactu~rer/Model        ~FW   ~Cluster Support\n", // STR_LIST_CLUSTER_HEADER
 	"~Address    ~Manufactu~rer/Model        ~FW   ~Cluster Support ~FUA\n", // STR_LIST_CLUSTER_FUA_HEADER
   "~Address    ~Manufactu~rer/Model        ~FW   ~Hot Spare Same Channel\n", // STR_LIST_HS_SAME_CHANNEL_HEADER
   "~Address    ~Manufactu~rer/Model        ~FW       ~   Serial        ~123456789012\n", // STR_LIST_INQUIRY_HEADER
   "~LUN Address    ~SIZE(MB)        ~AVAILABLE (MB)   ~Start Block     ~End Block\n", // STR_LIST_SEGMENT_HEADER
 	"Capabilities Map:  Column 1 = Soft Reset\n", // STR_LIST_INQUIRY_HEADER_MAP1
 	"                   Column 2 = Cmd Queuing\n", // STR_LIST_INQUIRY_HEADER_MAP2
 	"                   Column 3 = Linked Cmds\n", // STR_LIST_INQUIRY_HEADER_MAP3
 	"                   Column 4 = Synchronous\n", // STR_LIST_INQUIRY_HEADER_MAP4
 	"                   Column 5 = Wide 16\n", // STR_LIST_INQUIRY_HEADER_MAP5
 	"                   Column 6 = Wide 32\n", // STR_LIST_INQUIRY_HEADER_MAP6
 	"                   Column 7 = Relative Addr\n", // STR_LIST_INQUIRY_HEADER_MAP7
 	"                   Column 8 = SCSI II\n", // STR_LIST_INQUIRY_HEADER_MAP8
 	"                   Column 9 = S.M.A.R.T.\n", // STR_LIST_INQUIRY_HEADER_MAP9
 	"                   Column 0 = SCAM\n", // STR_LIST_INQUIRY_HEADER_MAP10
 	"                   Column 1 = SCSI-3\n", // STR_LIST_INQUIRY_HEADER_MAP11
 	"                   Column 2 = SAF-TE\n", // STR_LIST_INQUIRY_HEADER_MAP12
  	"   X = Capability Exists, - = Capability does not exist, O = Not Supported\n", // STR_LIST_INQUIRY_HEADER_KEY

 	"~Inquiry Data   ~", // STR_INQ_TITLE
   "NVRAM Reset", // STR_NVRAM_RESET
   "OK", // STR_OK
   "Yes", // STR_YES
   "No", // STR_NO
   "abort", // STR_ABORT
//     STR_RETRY                         = 8,
//     STR_CONTINUE                      = 9,
//     STR_CANCEL                        = 10,
//     STR_ACCEPT                        = 11,
//     STR_RESET                         = 12,
//     STR_SKIP                          = 13,
//     STRT_PREVIOUS                     = 14,
//     STRT_FILE                         = 15,
//     STRT_OPTIONS                      = 16,
//     STRT_INSTALLATION                 = 17,
//     STRT_COMMUNICATIONS               = 18,
//     STRT_HELP                         = 19,
//     STRT_READ_SYS_CONFIG              = 20,
//     STRT_SET_SYS_CONFIG               = 21,
//     STRT_LOAD_CONFIG                  = 22,
//     STRT_SAVE_CONFIG                  = 23,
//     STRT_EXIT                         = 24,
//     STRT_SET_HBA_THROTTLE             = 25,
//     STRT_DISABLE_ALARM                = 26,
//     STRT_QUIET_BUS                    = 27,
//     STRT_INITIAL_SETUP                = 28,
//     STRT_DOS_SETUP                    = 29,
//     STRT_MAKE_CONNECTION              = 30,
//     STRT_LEGEND_ICONS                 = 31,
//     STRT_INDEX                        = 32,
//     STRT_CONTACT                      = 33,
//     STRT_SYS_INFO                     = 34,
//     STRT_ABOUT                        = 35,
//     STRT_CREATE_RAID                  = 36,
//     STRT_MODIFY_RAID                  = 37,
//     STRT_SWITCH_VIEW                  = 38,
//     STRT_INCLUDE_DRIVE                = 39,
//     STRT_DONE                         = 40,
//     STRT_REMOVE_DRIVE                 = 41,
//     STRT_EXIT_INSTALL                 = 42,
//     STR_DECIMAL                       = 43,
//     STR_PROG_EXIT                     = 44,
	"Failed",         // STR_FAILED     = 45,
//     STR_RAID_5_GROUP                  = 46,
   "Warning",              // STR_WARNING    = 47,
//     STR_RAID_0_GROUP                  = 48,
//     STR_RAID_1_GROUP                  = 49,
   "Missing", // STR_MISSING
   "Removed", // STR_REMOVED
   "Added", // STR_ADDED
//     STR_LOG_SU                        = 51,
//     STR_HOTSPARE                      = 52,
//     STR_LSU                           = 53,
//     STR_TAPE                          = 54,
   "Printer", // STR_SCSI_PRINTER
   "Processor", // STR_SCSI_PROCESSOR
   "WORM", // STR_SCSI_WORM
   "CD-ROM", // STR_SCSI_CD_ROM
   "Scanner", // STR_SCSI_SCANNER
   "Optical", // STR_SCSI_OPTICAL
   "Jukebox", // STR_SCSI_JUKEBOX
   "Pro Root", // STR_SCSI_PRO_ROOT
	"Pro Connection", // STR_SCSI_PRO_CONNECTION
//     STR_COMMUN                        = 62,
   "Unknown",   //     STR_UNKNOWN
   "Unchanged",   // STR_UNCHANGED
   "Unsupported",   // STR_UNSUPPORTED
//     STR_DISK                          = 64,
//     STR_FLOPTICAL                     = 65,
//     STR_KB                            = 66,
   "MB", // STR_MB
//     STR_DESCRIPTION                   = 68,
//     STR_LEGEND_ICONS                  = 69,
//     STR_LEGEND_FLAGS                  = 70,
//     STR_HOST_ADAPTER                  = 71,
//     STR_CHIPSET                       = 72,
//     STR_RAID_MOD                      = 73,
//     STR_CACHE_MOD                     = 74,
//     STR_MEMORY_MOD                    = 75,
//     STR_CACHE_MEM_MOD                 = 76,
//     STR_HARD_DRIVE                    = 77,
//     STR_SETUP_FOR                     = 78,
//     STR_RAID_GROUP                    = 79,
//     STR_FAULT_TOL                     = 80,
//     STR_DRIVE_FT                      = 81,
//     STR_NO_FT                         = 82,
//     STR_OPTIMIZATION                  = 83,
//     STR_OPT_CAPACITY                  = 84,
//     STR_OPT_PERFORM                   = 85,
//     STR_CHOSEN_RAID                   = 86,
   "Stripe Size", // STR_STRIPE_SIZE,
//     STR_OVERRIDE                      = 88,
//     STR_OVERRIDE_RAID                 = 89,
//     STR_RAID_LEVEL                    = 90,
//     STR_NONE                          = 91,
//     STR_8_KB                          = 92,
//     STR_16_KB                         = 93,
//     STR_32_KB                         = 94,
//     STR_64_KB                         = 95,
//     STR_128_KB                        = 96,
//     STR_256_KB                        = 97,
//     STR_512_KB                        = 98,
//     STR_1_MB                          = 99,
//     STR_RAID_DASH                     = 100,
   "Hot Spare", // STR_RAID_HOT_SPARE
   "RAID", // STR_RAID
   "RAID 0 (Striped)", // STR_RAID_0
   "RAID 1 (Mirrored)", // STR_RAID_1
   "RAID 5 (Redundant Parity)", // STR_RAID_5
   "RAID 10 (Mirrored&Striped)", // STR_RAID_10
   "RAID 50 (Parity&Striped)", // STR_RAID_50
//     STR_8_KB_RA                       = 104,
//     STR_16_KB_RA                      = 105,
//     STR_32_KB_RA                      = 106,
//     STR_64_KB_RA                      = 107,
//     STR_128_KB_RA                     = 108,
//     STR_256_KB_RA                     = 109,
//     STR_512_KB_RA                     = 110,
//     STR_1_MB_RA                       = 111,
//     STR_NOT_INSTALLED                 = 112,
//     STR_SYSTEM_INFO                   = 113,
//     STR_CMOS_SETTINGS                 = 114,
//     STR_LOGICAL_PARMS                 = 115,
//     STR_DRIVE                         = 116,
//     STR_DRIVE_0                       = 117,
//     STR_DRIVE_1                       = 118,
//     STR_SROM_VERSION                  = 119,
//     STR_ENGINE_VERSION                = 120,
//     STR_DRIVER_NAME                   = 121,
//     STR_DRIVER_VERSION                = 122,
//     STR_ADDRESS                       = 123,
   "Device", // STR_DEVICE
   "Not Present", // STR_NOT_PRESENT
//     STR_BASE_MEMORY                   = 125,
//     STR_EXTEND_MEMORY                 = 126,
//     STR_HELP                          = 127,
//     STR_SCANNING_1                    = 128,
//     STR_SCANNING_2                    = 129,
//     STR_DOS_SETUP                     = 130,
//     STR_NO_DOS                        = 131,
//     STR_NO_CDROM_DRIVERS              = 132,
//     STR_CHG_CONFIG                    = 133,
//     STR_TOO_MANY                      = 134,
//     STR_CHG_AUTOEXEC                  = 135,
//     STR_NO_CAPACITY                   = 136,
 	"Manufacturer", // STR_MANUFACTURER
   "Model", // STR_MODEL
//     STR_CACHE                         = 138,
//     STR_FIRMWARE_COLON                = 139,
//     STR_IRQ                           = 140,
//     STR_HEX                           = 141,
//     STR_HBA_INFO                      = 142,
//     STR_DMA                           = 143,
//     STR_PRIMARY                       = 144,
//     STR_SECONDARY                     = 145,
//     STR_EDGE                          = 146,
//     STR_LEVEL                         = 147,
//     STR_BUS_MASTER                    = 148,
//     STR_8_BIT                         = 149,
//     STR_16_BIT                        = 150,
//     STR_SINGLE_END                    = 151,
//     STR_DIFFERENTIAL                  = 152,
//     STR_LVD_SINGLE                    = 153,
   "fibre",       // STR_FIBRE
//     STR_ADDR_HBA                      = 155,
   "BUS ", // STR_BUS
   "ID", // STR_ID
//     STR_LUN                           = 158,
//     STR_CAPACITY                      = 159,
//     STR_STATUS                        = 160,
//     STR_RAID_INFO                     = 161,
//     STR_SCSI_INFO                     = 162,
   "Removable", // STR_REMOVABLE
//     STR_CAPABILITIES                  = 164,
//     STR_REL_ADDR                      = 165,
   "wide",       // STR_WIDE
   "narrow",     // STR_NARROW
//     STR_WIDE_16                       = 166,
//     STR_WIDE_32                       = 167,
//     STR_SYNC                          = 168,
//     STR_LINKED                        = 169,
//     STR_CMD_Q                         = 170,
//     STR_SOFT_RESET                    = 171,
//     STR_ESTIMATED                     = 172,
//     STR_RAID_LEVEL_COLON              = 173,
//     STR_TRANSFER                      = 174,
//     STR_MB_SECOND                     = 175,
   "Format", // STR_FORMAT
//     STRT_MAKE_HOTSPARE                = 177,
//     STRT_PRINT                        = 178,
//     STRT_DIAGS                        = 179,
//     STRT_VIEW_LOG                     = 180,
//     STRT_REPORTS                      = 181,
//     STRT_STATISTICS                   = 182,
//     STRT_MOD_CONFIG                   = 183,
//     STRT_MOD_AUTOEXEC                 = 184,
//     STRT_CREATE_BAT                   = 185,
//     STR_INCOMPLETE                    = 186,
   "Incomplete entry\n\n", // STR_INCOMPLETE_ENTRY
//     STR_NONE_CHOSEN                   = 187,
//     STR_SCSI_BUS                      = 188,
//     STR_BRIDGE                        = 189,
//     STR_REREAD                        = 190,
//     STR_COPY_FILES                    = 191,
//     STR_PHYSICAL                      = 192,
   "Blocks", // STR_BLOCKS
//     STR_SIZE                          = 194,
//     STR_BYTES                         = 195,
//     STR_NA                            = 196,
   "Rev", // STR_REV
//     STR_MEMBER_OF                     = 198,
//     STR_CONTROLLER                    = 199,
//     STR_HOST_BUS                      = 200,
 	"Type", // STR_TYPE
//     STR_MB_SEC_MAX                    = 202,
//     STR_WIDTH                         = 203,
//     STR_SCSI_ID                       = 204,
//     STR_FIBRE_ID                      = 205,
//     STR_ATTACHED_MOD                  = 206,
//     STR_PHYSICAL_CONFIG               = 207,
//     STR_LOGICAL_CONFIG                = 208,
//     STR_CREATING_ARRAY                = 209,
   "Array Modify ", // STR_ARRAY_MODIFY
   "Raid Type: ", // STR_RAID_TYPE
//     STR_SELECT_ARRAY_TYPE             = 211,
//     STR_SCSI_DIAGS                    = 212,
//     STR_TEST_DURATION                 = 213,
//     STR_ITERATIONS                    = 214,
//     STR_CONTINUOUS                    = 215,
//     STR_TESTS_PERFORMED               = 216,
//     STR_DEVICE_BUFFER                 = 217,
//     STR_MEDIA_READ                    = 218,
//     STR_MEDIA_RW                      = 219,
//     STR_TARGET_SECTORS                = 220,
//     STR_ALL_SECTORS                   = 221,
//     STR_RANDOM_SECTORS                = 222,
//     STR_STOP_ON_ERROR                 = 223,
//     STR_PASS                          = 224,
//     STR_ERRORS                        = 225,
//     STR_TOTAL_TIME                    = 226,
//     STRT_RUN_TEST                     = 227,
//     STRT_STOP_TEST                    = 228,
//     STR_DIAGPCT                       = 229,
//     STR_1KB_STAT                      = 230,
//     STR_2KB_STAT                      = 231,
//     STR_4KB_STAT                      = 232,
//     STR_8KB_STAT                      = 233,
//     STR_16KB_STAT                     = 234,
//     STR_32KB_STAT                     = 235,
//     STR_64KB_STAT                     = 236,
//     STR_128KB_STAT                    = 237,
//     STR_256KB_STAT                    = 238,
//     STR_512KB_STAT                    = 239,
//     STR_1MB_STAT                      = 240,
//     STR_TOTAL_STAT                    = 241,
//     STR_FORMAT_WARN                   = 242,
//     STR_FORMAT_OPTIONS                = 243,
//     STR_CREATE_DISK                   = 244,
//     STR_FORMAT_DISKETTE               = 245,
//     STR_PROTECTED_RAID0               = 246,
//     STR_PROTECTED_RAID1               = 247,
//     STR_PROTECTED_RAID5               = 248,
//     STRT_HOTSPARES                    = 249,
//     STRT_REMOVE_HOTSPARE              = 250,
//     STR_CURRENT_MEDIA                 = 251,
//     STR_NO_MEDIA                      = 252,
//     STR_NOW_QUIET                     = 253,
//     STR_EVENT_LOGS                    = 254,
//     STRT_EVENT_LOG                    = 255,
//     STR_NAME_COLON                    = 256,
//     STR_NAME                          = 257,
//     STRT_BUILD                        = 258,
//     STR_NAME_ARRAY                    = 259,
//     STR_CHOOSE_NAME                   = 260,
//     STR_NAME_USED                     = 261,
//     STR_DPT_INSTALL                   = 262,
//     STR_INSTALL_6                     = 263,
//     STR_INSTALL_11                    = 264,
//     STR_SCSI_BUSES                    = 265,
//     STR_NO_CMOS_FLOPPY                = 266,
//     STR_NO_DRIVERS                    = 267,
//     STR_CHOOSE_TARGET                 = 268,
//     STR_INSERT_BLANK                  = 269,
//     STR_FORMAT_FAILURE                = 270,
//     STR_WRITING                       = 271,
//     STR_HDW_ERROR_DESC                = 272,
//     STR_HDW_ERROR                     = 273,
//     STR_SET_CONFIG_WARN               = 274,
//     STR_NO_OS2_DOS_BOX                = 275,
//     STR_REQ_SMARTROM                  = 276,
//     STR_OFFER_REBOOT                  = 277,
//     STR_WHICH_VERSION                 = 278,
//     STR_SELECT_OS                     = 279,
//     STR_CHOOSE_OS_FAMILY              = 280,
//     STR_AVAIL_CONN                    = 281,
//     STR_UNSAVED                       = 282,
//     STR_INTERNAL_ERROR                = 283,
//     STR_IE_CONTACT                    = 284,
//     STR_SEARCH_ENGINES                = 285,
//     STR_NUM_STRIPES                   = 286,
//     STR_COMPONENTS                    = 287,
//     STRT_VERIFY_ARRAY                 = 288,
//     STR_DESTINATION                   = 289,
//     STR_LPT1                          = 290,
//     STR_LPT2                          = 291,
//     STR_PRINT_SETTINGS                = 292,
//     STR_FILE_COLON                    = 293,
//    STR_APPEND_FILE                   = 294,
//    STRP_DEVICE_TYPE                  = 295,
//    STR_PRINTER_NOT_READY             = 296,
//    STR_HLP_MISSING                   = 297,
//    STR_NO_SPECIFIC_HELP              = 298,
//    STRT_CONTINUE_C                   = 299,
//    STR_NO_RAID_CAP                   = 300,
//    STR_LOAD_CLASS_DRV                = 301,
//    STR_CHOOSE_DONE                   = 302,
 	"Invalid stripe size: ", // STR_PARSE_ERR_INVALID_STRIPE_SIZE
 	"Invalid task rate: ", // STR_PARSE_ERR_INVALID_TASK_RATE
   "Invalid SCSI MHz rate: ", // STR_PARSE_ERR_INVALID_SPEED
 	"A switch was specified that is not supported: ", // STR_PARSE_ERR_UNDEFINED_SWITCH
 	"Logical drive's capacity must be an integer value: ", // STR_PARSE_ERR_DRIVE_CAPACITY_MUST_BE_INTEGER
 	"HBA designation incorrect: ", // STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED
 	"Switch specified multiple times in same command: ", // STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED
 	"Arguments must be integer value: ", // STR_PARSE_ERR_MUST_BE_INTEGER_VALUE
 	"Unrecognized device type to list: ", // STR_PARSE_ERR_NOT_A_VALID_LIST_TYPE
 	"Must specify HBA for this command: ", // STR_PARSE_ERR_MUST_SPECIFY_HBA
 	"Must specify an address for this command: ", // STR_PARSE_ERR_MUST_SPECIFY_ADDRESS
 	"Must specify a list of logicals or \"all\":", // STR_PARSE_ERR_MUST_SPECIFY_ALL_OR_LIST
 	"This command is unimplemented: ", // STR_PARSE_ERR_UNIMPLEMENTED_COMMAND
 	"This is an invalid command or option to a command: ", // STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD
   "Must specify an image file", // STR_PARSE_ERR_MUST_SUPPLY_IMAGE
   "Must specify a file name", // STR_PARSE_ERR_MUST_SUPPLY_FILE_NAME
   "Must specify a replacement string", // STR_PARSE_ERR_MUST_SUPPLY_ASCII_DATA
   "Replacement string too long", // STR_PARSE_ERR_ASCII_DATA_TOO_LONG
   "Parser error", // STR_CMD_PARSER_ERROR
   "Invalid list type", // STR_CMD_ERR_INVALID_LIST_TYPE
   "Can't find component by address", // STR_CMD_ERR_CANT_FIND_COMPONENT
   "Can't find HBA index", // STR_CMD_ERR_CANT_FIND_HBA_INDEX
   "Can't find HBA index (in NVRAM)", // STR_CMD_ERR_CANT_FIND_HBA_INDEX_NVRAM
   "Component is currently busy", // STR_CMD_ERR_COMPONENT_BUSY
   "Invalid flash image", // STR_CMD_ERR_INVALID_FLASH_IMAGE
   "Value out of range", // STR_ERR_VALUE_OUT_OF_RANGE
   "Invalid file", // STR_ERR_INVALID_FILE
   "Invalid raid type for this command", // STR_ERR_INVALID_RAID_TYPE
   "Controller does not have enough memory available", // STR_ERR_NOT_ENOUGH_MEMORY
   "Invalid command on RAID", // STR_CMD_ERR_CMD_NOT_POSS_ON_RAID
   "Invalid command on HBA", // STR_CMD_ERR_CMD_NOT_POSS_ON_HBA
   "Invalid command on this device", // STR_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE
   "Possible buffer corruption", // STR_ERR_POSS_BUFFER_CORRUPTION
   "Message: rtn completed", // STR_MSG_RTN_COMPLETED
   "Message: rtn started", // STR_MSG_RTN_STARTED
   "Message: rtn failed", // STR_MSG_RTN_FAILED
   "Message: rtn data overflow", // STR_MSG_RTN_DATA_OVERFLOW
   "Message: rtn data underflow", // STR_MSG_RTN_DATA_UNDERFLOW
   "Message: rtn ignored", // STR_MSG_RTN_IGNORED
   "Message: rtn disconnect", // STR_MSG_RTN_DISCONNECT
   "Error: conn list alloc", // STR_ERR_CONN_LIST_ALLOC
   "Error: semaphore alloc", // STR_ERR_SEMAPHORE_ALLOC
   "Error: osd open engine", // STR_ERR_OSD_OPEN_ENGINE
   "Error: invalid io method", // STR_ERR_INVALID_IO_METHOD
   "Error: no smartrom", // STR_ERR_NO_SMARTROM
   "Error: engine init", // STR_ERR_ENGINE_INIT
   "Error: invalid conn tag", // STR_ERR_INVALID_CONN_TAG
   "Error: semaphore timeout", // STR_ERR_SEMAPHORE_TIMEOUT
   "Error: null io buffer", // STR_ERR_NULL_IO_BUFFER
   "Error: invalid tgt tag", // STR_ERR_INVALID_TGT_TAG
   "Error: destroy semaphore", // STR_ERR_DESTROY_SEMAPHORE
   "Error: mem alloc", // STR_ERR_MEM_ALLOC
   "Error: invalid dev addr", // STR_ERR_INVALID_DEV_ADDR
   "Error: duplicate name", // STR_ERR_DUPLICATE_NAME
   "Error: get ccb", // STR_ERR_GET_CCB
   "Error: no raid devices", // STR_ERR_NO_RAID_DEVICES
   "Error: reserve blk sig", // STR_ERR_RESERVE_BLK_SIG
   "Error: format blk size", // STR_ERR_FORMAT_BLK_SIZE
   "Error: raid refnum", // STR_ERR_RAID_REFNUM
   "Error: RAID component duplicate", // STR_ERR_RAID_COMP_DUPLICATE
   "Error: RAID component restrict", // STR_ERR_RAID_COMP_RESTRICT
   "Error: RAID component used", // STR_ERR_RAID_COMP_USED
   "Error: RAID component ghost", // STR_ERR_RAID_COMP_GHOST
   "Error: RAID component failed", // STR_ERR_RAID_COMP_FAILED
   "Error: raid too few", // STR_ERR_RAID_TOO_FEW
   "Error: raid too many", // STR_ERR_RAID_TOO_MANY
   "Error: raid even", // STR_ERR_RAID_EVEN
   "Error: raid odd", // STR_ERR_RAID_ODD
   "Error: raid power 2 plus", // STR_ERR_RAID_POWER_2_PLUS
   "Error: raid chan count", // STR_ERR_RAID_CHAN_COUNT
   "Error: raid min stripe", // STR_ERR_RAID_MIN_STRIPE
   "Error: raid max stripe", // STR_ERR_RAID_MAX_STRIPE
   "Error: raid zero stripes", // STR_ERR_RAID_ZERO_STRIPES
   "Error: raid too large", // STR_ERR_RAID_TOO_LARGE
   "Error: raid start chan", // STR_ERR_RAID_START_CHAN
   "Error: raid seq chan", // STR_ERR_RAID_SEQ_CHAN
   "Error: raid diff stripes", // STR_ERR_RAID_DIFF_STRIPES
   "Error: raid diff num str", // STR_ERR_RAID_DIFF_NUM_STR
   "Error: raid over stripe", // STR_ERR_RAID_OVER_STRIPE
   "Error: RAID component remove", // STR_ERR_RAID_COMP_REMOVE
   "Error: RAID component emulated", // STR_ERR_RAID_COMP_EMULATED
   "Error: RAID component devtype", // STR_ERR_RAID_COMP_DEVTYPE
   "Error: RAID component non 512", // STR_ERR_RAID_COMP_NON_512
   "Error: raid diff blocks", // STR_ERR_RAID_DIFF_BLOCKS
   "Error: raid diff capacity", // STR_ERR_RAID_DIFF_CAPACITY
   "Error: raid diff vendor", // STR_ERR_RAID_DIFF_VENDOR
   "Error: raid diff product", // STR_ERR_RAID_DIFF_PRODUCT
   "Error: raid diff revision", // STR_ERR_RAID_DIFF_REVISION
   "Error: raid not supported", // STR_ERR_RAID_NOT_SUPPORTED
   "Error: raid invalid hba", // STR_ERR_RAID_INVALID_HBA
	"Error: raid table required", // STR_ERR_RAID_TABLE_REQUIRED
	"Error: RAID component tag", // STR_ERR_RAID_COMP_TAG
	"Error: raid max arrays", // STR_ERR_RAID_MAX_ARRAYS
	"Error: RAID component size", // STR_ERR_RAID_COMP_SIZE
	"Error: raid fw level", // STR_ERR_RAID_FW_LEVEL
	"Error: invalid hba addr", // STR_ERR_INVALID_HBA_ADDR
	"Error: isa addr only", // STR_ERR_ISA_ADDR_ONLY
   "Error: primary hba exists", // STR_ERR_PRIMARY_HBA_EXISTS
   "Error: no more slots", // STR_ERR_NO_MORE_SLOTS
   "Error: dup isa addr", // STR_ERR_DUP_ISA_ADDR
   "Error: dup eisa slot", // STR_ERR_DUP_EISA_SLOT
   "Error: primary isa addr", // STR_ERR_PRIMARY_ISA_ADDR
   "Error: secondary isa addr", // STR_ERR_SECONDARY_ISA_ADDR
   "Error: abs no more ids", // STR_ERR_ABS_NO_MORE_IDS
   "Error: abs non zero lun", // STR_ERR_ABS_NON_ZERO_LUN
   "Error: abs addr limits", // STR_ERR_ABS_ADDR_LIMITS
   "Error: abs addr occupied", // STR_ERR_ABS_ADDR_OCCUPIED
   "Error: abs no more luns",     //STR_ERR_ABS_NO_MORE_LUNS   
   "Error: new artificial",       //STR_ERR_NEW_ARTIFICIAL     
   "Error: io not supported",     //STR_ERR_IO_NOT_SUPPORTED   
   "Error: rw exceeds capacity",  //STR_ERR_RW_EXCEEDS_CAPACITY
   "Error: data in out",          //STR_ERR_DATA_IN_OUT        
   "Error: scsi cmd failed", //STR_ERR_SCSI_CMD_FAILED
   "Error: artificial io",   //STR_ERR_ARTIFICIAL_IO  
   "Error: scsi io",         //STR_ERR_SCSI_IO        
   "Error: blink led io",    //STR_ERR_BLINK_LED_IO   
   "Error: osd mem alloc",   //STR_ERR_OSD_MEM_ALLOC  
   "Error: formatting",      //STR_ERR_FORMATTING      
   "Error: hba busy",        //STR_ERR_HBA_BUSY        
   "Error: hba initializing",//STR_ERR_HBA_INITIALIZING
   "Error: del old raid",    //STR_ERR_DEL_OLD_RAID    
   "Error: enable new raid", //STR_ERR_ENABLE_NEW_RAID 
   "Error: update os config",  //STR_ERR_UPDATE_OS_CONFIG  
   "Error: scsi addr bounds",  //STR_ERR_SCSI_ADDR_BOUNDS  
   "Error: scsi addr conflict",//STR_ERR_SCSI_ADDR_CONFLICT
   "Error: cannot delete",     //STR_ERR_CANNOT_DELETE     
   "Error: fwd no space",      //STR_ERR_FWD_NO_SPACE
   "Error: fwd not reserved",   //STR_ERR_FWD_NOT_RESERVED   
   "Error: fwd not initialized",//STR_ERR_FWD_NOT_INITIALIZED 
   "Error: fwd blk mismatch",   //STR_ERR_FWD_BLK_MISMATCH   
   "Error: fwd blk overflow",   //STR_ERR_FWD_BLK_OVERFLOW   
   "Error: rsv removable",     //STR_ERR_RSV_REMOVABLE     
   "Error: rsv not dasd",  //STR_ERR_RSV_NOT_DASD  
   "Error: rsv non zero",  //STR_ERR_RSV_NON_ZERO  
   "Error: rsv hba unable",//STR_ERR_RSV_HBA_UNABLE
   "Error: rsv other",     //STR_ERR_RSV_OTHER     
   "Error: scan physicals",//STR_ERR_SCAN_PHYSICALS
   "Error: init physicals",    // STR_ERR_INIT_PHYSICALS  
   "Error: scan logicals",     // STR_ERR_SCAN_LOGICALS   
   "Error: init logicals",     // STR_ERR_INIT_LOGICALS   
   "Error: comm xmit buffer",  // STR_ERR_COMM_XMIT_BUFFER
   "Error: comm rcve buffer",  // STR_ERR_COMM_RCVE_BUFFER
   "Error: comm disconnected", //STR_ERR_COMM_DISCONNECTED 
   "Error: comm data overflow",//STR_ERR_COMM_DATA_OVERFLOW
   "Error: c t open",          //STR_ERRC_T_OPEN           
   "Error: c t bind",          //STR_ERRC_T_BIND           
   "Error: c t alloc",         //STR_ERRC_T_ALLOC          
   "Error: c t connect",   //STR_ERRC_T_CONNECT   
   "Error: c t listen",    //STR_ERRC_T_LISTEN    
   "Error: c t accept",    //STR_ERRC_T_ACCEPT    
   "Error: c comm nw init",//STR_ERRC_COMM_NW_INIT
   "Error: c comm ws init",//STR_ERRC_COMM_WS_INIT
   "Error: c semaphore timeout",//STR_ERRC_SEMAPHORE_TIMEOUT
   "Error: c connection tag",   //STR_ERRC_CONNECTION_TAG   
   "Error: c not null termed",  //STR_ERRC_NOT_NULL_TERMED  
   "Error: c mem alloc",        //STR_ERRC_MEM_ALLOC        
   "Error: c null io buffer",   //STR_ERRC_NULL_IO_BUFFER   
   "Error: c invalid password",//STR_ERRC_INVALID_PASSWORD
   "Error: c not logged in",   //STR_ERRC_NOT_LOGGED_IN   
   "Error: c engine load",     //STR_ERRC_ENGINE_LOAD     
   "Error: c not supported",   //STR_ERRC_NOT_SUPPORTED   
   "Error: c icrs active",     //STR_ERRC_ICRS_ACTIVE     
   "Error: c icrs inactive",     //STR_ERRC_ICRS_INACTIVE     
   "Error: c icrs req posted",   //STR_ERRC_ICRS_REQ_POSTED   
   "Error: c icrs thread start", //STR_ERRC_ICRS_THREAD_START 
   "Error: c icrs start request",//STR_ERRC_ICRS_START_REQUEST
   "Error: c icrs init",         //STR_ERRC_ICRS_INIT
   "Error: c accepting icr",     //STR_ERRC_ACCEPTING_ICR
   "Error: c tx Message: sync",  //STR_ERRC_TX_MSG_SYNC  
   "Error: c rx Message: ack",   //STR_ERRC_RX_MSG_ACK   
   "Error: c rx Message: header",//STR_ERRC_RX_MSG_HEADER
   "Error: c tx Message: header",//STR_ERRC_TX_MSG_HEADER
   "Error: c tx to eng data",  //STR_ERRC_TX_TO_ENG_DATA  
   "Error: c rx to eng data",  //STR_ERRC_RX_TO_ENG_DATA  
   "Error: c rx status header",//STR_ERRC_RX_STATUS_HEADER
   "Error: c tx status header",//STR_ERRC_TX_STATUS_HEADER
   "Error: c rx from eng data",//STR_ERRC_RX_FROM_ENG_DATA
   "Error: c tx from eng data",//STR_ERRC_TX_FROM_ENG_DATA
   "Error: c from eng size",   //STR_ERRC_FROM_ENG_SIZE   
   "Error: c to eng size",     //STR_ERRC_TO_ENG_SIZE     
   "Error: c serial init",     //STR_ERRC_SERIAL_INIT     
   "Error: c baud rate",       //STR_ERRC_BAUD_RATE       
   "Error: c comm busy",       //STR_ERRC_COMM_BUSY       
   "Error: c invalid protocol",//STR_ERRC_INVALID_PROTOCOL
   "Error: c port conflict",   //STR_ERRC_PORT_CONFLICT   
   "Error: c modem init",      //STR_ERRC_MODEM_INIT      
   "Error: c dial abort",      //STR_ERRC_DIAL_ABORT      
   "Error: c dial timeout",      //STR_ERRC_DIAL_TIMEOUT      
   "Error: c dial busy",         //STR_ERRC_DIAL_BUSY         
   "Error: c dial beeper ok",    //STR_ERRC_DIAL_BEEPER_OK    
   "Error: c dial unexpected cd",//STR_ERRC_DIAL_UNEXPECTED_CD
   "Error: c dial no tone",      //STR_ERRC_DIAL_NO_TONE      
   "Error: c dial no answer",  //STR_ERRC_DIAL_NO_ANSWER
   "Error: c dial Error: or",  //STR_ERRC_DIAL_ERROR    
   "Error: c negotiation",     //STR_ERRC_NEGOTIATION   
   "Error: c Message: timeout",//STR_ERRC_MSG_TIMEOUT   
   "Error: c user abort",      //STR_ERRC_USER_ABORT    
   "Error: spx rd property", //STR_ERRSPX_RD_PROPERTY
   "Error: spx sap",         //STR_ERRSPX_SAP        
   "Error: c socket alloc",  //STR_ERRC_SOCKET_ALLOC 
   "Error: c socket bind",   //STR_ERRC_SOCKET_BIND  
   "Error: c socket accept", //STR_ERRC_SOCKET_ACCEPT
   "Error: c socket connect",  //STR_ERRC_SOCKET_CONNECT   
   "Error: c user validation", //STR_ERRC_USER_VALIDATION  
   "Error: flash erase",       //STR_ERR_FLASH_ERASE       
   "Error: flash switch modes",//STR_ERR_FLASH_SWITCH_MODES
   "Error: flash write 512",   //STR_ERR_FLASH_WRITE_512   
   "Error: flash eng verify",//STR_ERR_FLASH_ENG_VERIFY
   "Error: flash init req",  //STR_ERR_FLASH_INIT_REQ  
   "Error: exclusion time",  //STR_ERR_EXCLUSION_TIME  
   "Error: diag scheduled",  //STR_ERR_DIAG_SCHEDULED  
   "Error: diag not active", //STR_ERR_DIAG_NOT_ACTIVE 
   "Error: elog not loaded",//STR_ERR_ELOG_NOT_LOADED
   "Error: elog loaded",    //STR_ERR_ELOG_LOADED    
   "Error: elog events",    //STR_ERR_ELOG_EVENTS    
   "Error: elog paused",    //STR_ERR_ELOG_PAUSED    
   "Error: elog not paused",//STR_ERR_ELOG_NOT_PAUSED
   "Error: slog invalid time",         //STR_ERR_SLOG_INVALID_TIME  
   "Error: slog stat group",           //STR_ERR_SLOG_STAT_GROUP    
   "Error: alms already linked",       //STR_ERR_ALMS_ALREADY_LINKED
   "Error: alms not linked",           //STR_ERR_ALMS_NOT_LINKED    
   "Error: alms invalid resource type",//STR_ERR_ALMS_INVALID_RESOURCE_TYPE
   "Error: unknown error", // STR_ERR_UNKNOWN_ERROR
   "No Error", // STR_NO_ERROR
   "Er", // STR_ER
   "Selection Timeout", // STR_SEL_TIMEOUT
   "Command Timeout", // STR_CMD_TIMEOUT
   "SCSI Bus Reset",   // STR_BUS_RESET
   "Initial Controller Power-Up",  //     STR_POWER_UP
   "Unexpected Bus Phase", //     STR_UNX_PHASE
   "Unexpected Bus Free",  //     STR_UNX_FREE
   "Bus Parity Error", //     STR_BUS_PARITY
   "SCSI Hung",    //     STR_SCSI_HUNG
   "Unexpected Message Rejected",  //     STR_UNX_MESSAGE
   "SCSI Bus Reset Stuck", //     STR_BUS_STUCK
   "Auto Request-Sense Failed",    //     STR_AUTO_FAILED
   "Controller RAM Parity Error",  //     STR_RAM_PARITY
   "Power Supply", // STR_POWER_SUPPLY
   "Cooling Element", // STR_COOLING_ELEM
   "Fan", // STR_FAN
   "Failure", // STR_FAILURE
//     STR_ABT_NON_ACT                   = 316,
//     STR_ABT_ACT                       = 317,
//     STR_RST_NON_ACT                   = 318,
//     STR_RST_ACT                       = 319,
//     STR_RAM_ECC                       = 320,
//     STR_PCI_PRTY                      = 321,
//     STR_PCI_MST_ABORT                 = 322,
//     STR_PCI_TGT_ABORT                 = 323,
//     STR_PCI_SIG_ABORT                 = 324,
//     STR_HBAERR_RESERVED_0             = 325,
//     STR_HBAERR_RESERVED_1             = 326,
//     STR_HBAERR_RESERVED_2             = 327,
//     STR_HBAERR_RESERVED_3             = 328,
//     STR_HBAERR_RESERVED_4             = 329,
//     STR_HBAERR_RESERVED_5             = 330,
//     STR_HBAERR_RESERVED_6             = 331,
//     STR_HBAERR_RESERVED_7             = 332,
//     STR_HBAERR_RESERVED_8             = 333,
//     STR_HBAERR_RESERVED_9             = 334,
   "Good",  //   STR_GOOD                          = 335,
   "Check Condition",   //     STR_CHECK_COND                    = 336,
   "Condition Met", //     STR_COND_MET                      = 337,
   "Busy",  //     STR_BUSY                          = 338,
   "Intermediate",  //     STR_INTERMEDIATE                  = 339,
   "Intermediate - Condition Met",  //     STR_INT_COND_MET                  = 340,
   "Reservation Conflict",  //     STR_RESERV_CONFLICT               = 341,
   "Command Terminated",    //     STR_CMD_TERMINATED                = 342,
   "Queue Full",  // STR_QUEUE_FULL
   "Queue Frozen", // STR_QUEUE_FROZEN
//    STRT_REBUILD                      = 344,
//    STR_BUILD                         = 345,
//    STRT_VERIFY                       = 346,
   "\nLog Overflow\n\n",    //  STR_LOG_OVERFLOW                  = 347,
   "Bad SCSI Status",  //     STR_BAD_SCSI                      = 348,
   "Request Sense\n",  //     STR_REQ_SENSE                     = 349,
   "HBA Error",    //     STR_HBA_ERROR                     = 350,
   "Block reassigned", //     STR_BLOCK_REASSIGN                = 351,
   "RAM Error - Address=", //     STR_RAM_ERR_ADDR                  = 352,
   "HBA Time Changed", //     STR_HBA_TIME_CHG                  = 353,
   "Old",  //     STR_OLD                           = 354,
   "New",  //     STR_NEW                           = 355,
   "Array Configuration Update",   //     STR_CONFIG_UPDATE                 = 356,
   "Firmware", //     STR_FIRMWARE                      = 357,
   "Software", //     STR_SOFTWARE                      = 358,
//    STR_DRIVE_CRASH                   = 359,
   "Started",  //     STR_STARTED                       = 360,
   "Stopped",  //     STR_STOPPED                       = 361,
   "Completed",  // STR_COMPLETED
   "Data Inconsistency",   //     STR_DATA_INC                      = 362,
   "Block",    //     STR_BLOCK                         = 363,
   "Count",    //     STR_COUNT                         = 364,
   "Lock", //     STR_LOCK                          = 365,
   "No entries in the log\n", // STR_NO_LOG_ENTRIES
   "Drive Failure",    //     STR_DRIVE_FAILURE                 = 367,
//    STR_RESTART_MAC                   = 368,
//     STR_CFG_HBA                       = 369,
//     STR_SCSI_TERMPWR                  = 370,
//     STR_SCAM                          = 371,
//     STR_TERMINATION                   = 372,
   "On",  // STR_ON
   "Off", // STR_OFF
   "On (Write Back)", // STR_ON_WRITE_BACK
   "Off (Write Through)", // STR_OFF_WRITE_THRU
   "Write Back", // STR_WRITE_BACK
   "Write Through", // STR_WRITE_THRU
//     STR_HIGH_ONLY                     = 375,
//     STR_SCSI_XFER                     = 376,
//     STR_XFER_RATE                     = 377,
   "Async", // STR_ASYNCH
   " 5 MHz", // STR_5MHZ
   " 8 MHz", // STR_8MHZ
   "10 MHz", // STR_10MHZ
   "20 MHz", // STR_20MHZ
   "40 MHz", // STR_40MHZ
   "80 MHz", // STR_80MHZ
   "%d MHz", // STR_FILLIN_MHZ
   "%d GHz", // STR_FILLIN_GHZ
   "%d MB/sec", // STR_FILLIN_MB_PER_SEC
   "%d mV", // STR_FILLIN_MV
   "%d mA", // STR_FILLIN_MA
   "%d mAH", // STR_FILLIN_MAH
   "%d hrs", // STR_FILLIN_HRS
//     STR_PCI_BURST                     = 385,
//     STR_AUTO                          = 386,
//     STR_32LW                          = 387,
//     STR_64LW                          = 388,
//     STR_128LW                         = 389,
//     STR_EXT_CABLE                     = 390,
//     STR_ENABLED_5                     = 391,
//     STR_DISABLED_10                   = 392,
//     STR_DISABLED_20                   = 393,
//     STR_BUS_ON_TIME                   = 394,
 	"slow", // STR_CMD_LINE_TASK_RATE_SLOW
 	"medslow", // STR_CMD_LINE_TASK_RATE_MEDSLOW
 	"med", // STR_CMD_LINE_TASK_RATE_MED
 	"medfast", // STR_CMD_LINE_TASK_RATE_MEDFAST
 	"fast", // STR_CMD_LINE_TASK_RATE_FAST
 	"soft", // STR_CMD_LINE_LOG_SOFT
 	"recov", // STR_CMD_LINE_LOG_RECOV
 	"nonrecov", // STR_CMD_LINE_LOG_NONRECOV
 	"status", // STR_CMD_LINE_LOG_STATUS
 	"delete", // STR_CMD_LINE_LOG_DELETE
   "board", // STR_CMD_LINE_LOG_BOARD
  	"build", // STR_CMD_LINE_TASK_BUILD
 	"rebuild", // STR_CMD_LINE_TASK_REBUILD
 	"stop", // STR_CMD_LINE_TASK_STOP
 	"list", // STR_CMD_LINE_TASK_LIST
   "verify", // STR_CMD_LINE_TASK_VERIFY
 	"verifynofix", // HIDDEN!!!!!! STR_CMD_LINE_TASK_VERIFY_NO_FIX
   "optimal", // STR_CMD_LINE_OPTIMAL
   "fail", // STR_CMD_LINE_FAIL
   "load", // STR_CMD_LINE_LOAD
   "save", // STR_CMD_LINE_SAVE
// note that this contains the singular and plural of each list type.  This is due
// to the inconsistency in the command-line spec, it specifies some items as singular,
// some as plural.
// If your language only needs singular or plural, then set both singular and plural
// strings to the same value.
   "physical", // STR_CMD_LINE_PHYSICAL
   "physicals", // STR_CMD_LINE_PHYSICALS
   "logical", // STR_CMD_LINE_LOGICAL
   "logicals", // STR_CMD_LINE_LOGICALS
   "controller", // STR_CMD_LINE_CONTROLLER
   "controllers", // STR_CMD_LINE_CONTROLLERS
   "spare", // STR_CMD_LINE_SPARE
   "spares", // STR_CMD_LINE_SPARES
   "hotspare", // STR_CMD_LINE_HOTSPARE
   "hotspares", // STR_CMD_LINE_HOTSPARES
   "raid", // STR_CMD_LINE_ARRAY
   "array", // STR_CMD_LINE_ARRAYS
   "all", // STR_CMD_LINE_ALL
   "allvendor", // STR_CMD_LINE_ALL_VENDOR
   "speed", // STR_CMD_LINE_SPEED
   "redirect", // STR_CMD_LINE_REDIRECT
   "cache", // STR_CMD_LINE_CACHE
   "version", // STR_CMD_LINE_VERSION
   "inquiry", // STR_CMD_LINE_INQUIRY
   "inquiryvendor", // STR_CMD_LINE_INQUIRY_VENDOR
   "battery", // STR_CMD_LINE_BATTERY
   "drive", // STR_CMD_LINE_DRIVE
   "listtagsfordebug", // STR_CMD_LINE_TAGS  <- Karla's debug
   "arrayname", //STR_CMD_LINE_ARRAYNAME
//     STR_IO_ADDR                       = 397,
//     STR_HBA_ADDR                      = 398,
//     STR_DMA_CHANNEL                   = 399,
//     STR_DMA_SPEED                     = 400,
//     STR_PCI_PARITY                    = 401,
//     STR_SCSI_CMD_Q                    = 402,
//     STR_EXT_PCI_REQ                   = 403,
//     STR_GREEN_MODE                    = 404,
//     STR_FLOPPY_ENABLED                = 405,
//     STR_LA17_CRAP                     = 406,
//     STR_CACHING                       = 407,
//     STRT_FLASH                        = 408,
//     STRT_APPLY                        = 409,
//     STRT_DEFAULTS                     = 410,
//     STR_10MBS                         = 411,
//     STR_8MBS                          = 412,
//     STR_6_6MBS                        = 413,
//     STR_5_7MBS                        = 414,
//     STR_5_0MBS                        = 415,
//     STR_4_4MBS                        = 416,
//     STR_4_0MBS                        = 417,
//     STR_3_0MBS                        = 418,
//     STR_8K                            = 419,
//     STR_16K                           = 420,
//     STR_32K                           = 421,
//     STR_64K                           = 422,
//     STR_ALL                           = 423,
//     STR_HBA_CACHING                   = 424,
//     STR_NO_HBAS                       = 425,
//     STR_IRQ_12                        = 426,
//     STR_IRQ_14                        = 427,
//     STR_IRQ_15                        = 428,
      "Disabled", // STR_DISABLED
//     STR_DRQ_5                         = 430,
//     STR_DRQ_6                         = 431,
//     STR_DRQ_7                         = 432,
//     STR_START_NT_SERVICE              = 433,
//     STR_VIEW_NT_EVENTS                = 434,
//     STR_ZAP_INSTRUCT                  = 435,
//     STR_OUT_OF_RANGE                  = 436,
//     STR_COLD_BOOT                     = 437,
//     STR_DRIVES_ASSIGNED               = 438,
//     STR_AND                           = 439,
//     STR_ACCESS_DENIED                 = 440,
//     STR_PASSWORD                      = 441,
//     STR_ENTER_PASSWORD                = 442,
//     STR_LOCAL                         = 443,
//     STR_RAIDBCD_INFO                  = 444,
//     STR_VERSION_COLON                 = 445,
//     STR_HARDWARE                      = 446,
//     STR_IDENTIFIER                    = 447,
//     STR_REDUNDANT_CONT                = 448,
//     STR_DATE                          = 449,
//     STR_STRIPE                        = 450,
   "Formatting", // STR_FORMATTING
//     STR_RAID1_BUILD                   = 452,
//     STR_COPY_DIRECTION                = 453,
//     STR_CHG_DIRECTION                 = 454,
//     STR_START_COPY                    = 455,
//     STR_COPY                          = 456,
//     STR_CLEAR_BOTH                    = 457,
//     STR_PHYSICAL_MAP                  = 458,
//     STR_LOGICAL_MAP                   = 459,
//     STR_ONBOARD                       = 460,
//     STR_BANK                          = 461,
//     STRT_ALARM_OFF                    = 462,
   "Alarm", // STR_ALARM
//     STR_SCSIBCD_INFO                  = 464,
//     STR_NOT_CONNECTED                 = 465,
//     STR_NEED_CONNECTION               = 466,
//     STR_CONNECTION_LOST               = 467,
//     STR_NO_LONGER_CONNECTED           = 468,
//     STR_RETURN_TO_AVAILABLE           = 469,
//     STR_RETURN_TO_PHONEBOOK           = 470,
//     STR_SET_HBA_THROTTLE              = 471,
//     STR_FOREGROUND                    = 472,
//     STR_BACKGROUND                    = 473,
//     STR_DOS_VERSION                   = 474,
//     STR_LOADED_HIGH                   = 475,
//     STR_DPMI_VERSION                  = 476,
//     STR_MEMORY_EXP                    = 477,
//     STR_SIMM_MODULE                   = 478,
   "SCSI Bridge Controller", // STR_SCSI_BRIDGE_CTLR
   "RAID Bridge Controller", // STR_RAID_BRIDGE_CTLR
//     STR_NO_DISPLAY                    = 480,
//     STR_READING_IMAGE                 = 481,
//     STR_IS_OS                         = 482,
//     STR_CDD_CANT_HANDLE               = 483,
//     STR_CDD_READ_ERROR                = 484,
//     STR_CDD_CORRUPT                   = 485,
//     STR_FMT_WRITE_PROTECT             = 486,
//     STR_FMT_NOT_READY                 = 487,
//     STR_FMT_FAILURE                   = 488,
//     STR_TRY_AGAIN                     = 489,
//     STR_WRITING_DISKETTE              = 490,
//     STR_ERROR_WRITING_DD              = 491,
//     STR_REPLACE_SMDISK                = 492,
//     STRT_MAKE_OPTIMAL                 = 493,
//     STRT_STOP_VERIFY                  = 494,
//     STRT_STOP_BUILD                   = 495,
//     STR_DD_INSTRUCTIONS               = 496,
//     STRT_USING_NO_MOUSE               = 497,
//     STRT_PULL_DOWNS                   = 498,
//     STRT_FUNCTION_BTNS                = 499,
//     STRT_DELETE_ARRAY                 = 500,
//     STR_PLEASE_WAIT                   = 501,
//     STR_ALL_DATA_LOST                 = 502,
//     STR_LOAD_CONFIG_WARN              = 503,
//     STR_INSTALL_REMOTE                = 504,
//     STR_CANNOT_DETERMINE              = 505,
//     STR_PROCESSOR_COLON               = 506,
//     STR_OPERATING_SYSTEM              = 507,
   "Impacted",          // STR_IMPACTED      = 508,
//     STR_CANNOT_OPEN_FILE              = 509,
//     STR_FILE_OVERWRITE                = 510,
//     STR_FILENAME                      = 511,
//     STR_FILENAME_COLON                = 512,
//     STR_WARNING_COLON                 = 513,
//     STR_DEFAULT_SETTINGS              = 514,
//     STR_DRIVER_ROLLED_IN              = 515,
//     STR_UPON_RETURN                   = 516,
//     STR_FOUND_OS_ERRORS               = 517,
//     STR_OS_CONF_ERRORS                = 518,
//     STR_BAD_ENGINE_VERSION            = 519,
//     STR_DIAG_STOP_USER                = 520,
//     STR_DIAG_RUNNING                  = 521,
//     STR_DIAG_PAUSED                   = 522,
//     STR_DIAG_COMPLETED                = 523,
//     STR_DIAG_STOP_ERROR               = 524,
//     STR_DIAG_INVALID                  = 525,
   " ",    //     STR_BLANK                         = 526,
//     STRT_NEW_TEST                     = 527,
//     STR_BUFFER_READ                   = 528,
//     STR_BUFFER_RW                     = 529,
//     STR_VERIFY_INFO                   = 530,
//     STR_ITERATION_COLON               = 531,
//     STR_ARRAY_STATS                   = 532,
//     STR_HD_STATS                      = 533,
//     STR_READ_STATS                    = 534,
//     STR_TOTAL_SECTORS                 = 535,
//     STR_CACHE_HITS                    = 536,
//     STR_CACHE_MISSES                  = 537,
//     STR_READ_AHEAD_HITS               = 538,
//     STR_WRITE_STATS                   = 539,
//     STR_WRITE_BACKS                   = 540,
//     STR_WRITE_THRUS                   = 541,
//     STR_PROP_HEAD_STUFF               = 542,
//     STR_STRIPES_CROSSED               = 543,
//     STR_IO_COMMANDS                   = 544,
//     STR_READS                         = 545,
//     STR_WRITES                        = 546,
//     STR_TOTAL                         = 547,
//     STRT_CLEAR                        = 548,
//     STR_HBA_STATS                     = 549,
//     STR_CACHE_STATS                   = 550,
//     STR_TOTAL_PAGES                   = 551,
//     STR_USED_PAGES                    = 552,
//     STR_DIRTY_PAGES                   = 553,
//     STR_READ_AHEAD_PAGES              = 554,
//     STR_ECC_FAULT_PAGES               = 555,
//     STR_COMMANDS                      = 556,
//     STR_TOTAL_COMMANDS                = 557,
//     STR_MISALIGNED                    = 558,
//     STR_SCSI_BUS_RESETS               = 559,
//     STR_ADD_ADDL_DEVICES              = 560,
//     STR_ADD_ADDL_DONE                 = 561,
//     STR_NO_INITIAL                    = 562,
//     STRT_VIEW_README                  = 563,
//     STR_OS_SETTINGS                   = 564,
//     STR_VIEWING_FILE                  = 565,
   "Status Change\n",  //     STR_STATUS_CHANGE                 = 566,
//     STR_OLD_COLON                     = 567,
//     STR_NEW_COLON                     = 568,
//     STR_LOGGER_NOT_LOADED             = 569,
//     STR_EVENT_LOGGING_NOTIFICATION    = 570,
//     STRT_EVENT_LOGGING                = 571,
//     STR_FIRST_DONE                    = 572,
//     STR_LOG_LOAD_BAD                  = 573,
//     STR_COPY_SM_FILES                 = 574,
//     STR_OVERWRITE_SM_FILES            = 575,
//     STR_MSCDEX_MSG                    = 576,
//     STR_CANT_CONNECT                  = 577,
   "Building", //     STR_BUILDING                      = 578,
   "Build Pending", // STR_BUILD_PENDING
//     STR_FUTURE_DW_CONFIG              = 579,
//     STR_DW_CONFIG_NOT_DONE            = 580,
//     STR_FLOPT_TOO_SMALL               = 581,
//     STR_FLOPT_TOO_BIG                 = 582,
//     STR_FLOPT_OPTIONS                 = 583,
//     STR_FORMAT_MEDIA                  = 584,
//     STR_DOS_FORMAT                    = 585,
//     STR_FLOPT_BOOTABLE                = 586,
//     STR_FLOPT_NON_BOOTABLE            = 587,
//     STR_USE_SYS                       = 588,
//     STR_EVENT_LOGGING                 = 589,
//     STR_TIME_INTERVAL                 = 590,
//     STR_MINUTES_120                   = 591,
//     STR_RECORD_EVENTS_TO_FILE         = 592,
//     STR_NOTIFICATION                  = 593,
//     STR_BROADCAST_TO_USERS            = 594,
//     STR_LOG_TO_SYSTEM_FILE            = 595,
//     STR_EMAIL_TO_USERS                = 596,
//     STR_SEND_TO_DEVICES               = 597,
//     STR_REDIRECT_SCSI_ID              = 598,
//     STR_NEW_SCSI_ID                   = 599,
//     STR_MOD_CREATED_CONFLICT          = 600,
//     STR_DW_COMPLETE                   = 601,
//     STR_ORG_CONFIG                    = 602,
//     STR_ORG_AUTOEXEC                  = 603,
//     STR_ORG_SYSTEMINI                 = 604,
//     STRT_REDIRECT                     = 605,
//     STR_DW_ANYTIME                    = 606,
//     STRT_VIEW_README2                 = 607,
//     STR_PROMPT_REF                    = 608,
//     STR_FLOPT_NO_FORMAT               = 609,
   "Optimal", // STR_OPTIMAL
   "Created", // STR_CREATED
//     STR_MISSING_NO_CONFIG             = 611,
   "Creating/Mod",       // STR_CURRENT_CREATE = 612,
//     STR_MISSING_NO_ATTACH             = 613,
//     STR_NO_INFORMATION                = 614,
//     STR_LOG_NOT_REMOTE                = 615,
//     STRT_STOP_LOGGER                  = 616,
//     STRT_START_LOGGER                 = 617,
//     STRT_UNLOAD_LOGGER                = 618,
//     STR_PP_EMU_ONLY                   = 619,
//     STR_PP_NO_SMARTROM                = 620,
//     STR_PP_REQ_ALL_SECONDARY          = 621,
//     STR_PP_NO_LEVEL                   = 622,
//     STR_PP_NO_LUNS                    = 623,
//     STR_PP_EISA_ONLY                  = 624,
//     STR_PP_NO_ISA_EISA                = 625,
//     STR_TAPE_DRIVE                    = 626,
//     STR_PP_NO_SUPPORT                 = 627,
//     STR_PP_DEV_TOO_MANY               = 628,
//     STR_PP_DEV_WRONG_ID               = 629,
//     STR_PP_NO_STRIPING                = 630,
//     STR_SEND_LOGGER                   = 631,
   "Devices", // STR_DEVICES
//     STR_NOTE_THRESHOLD                = 633,
//     STR_USERS                         = 634,
   "Redirect", // STR_RAID_REDIRECT
//     STR_PP_EISA_SLOTS                 = 636,
//     STR_PP_TOO_MANY_HBAS              = 637,
//     STR_PP_UNLESS_RAID                = 638,
//     STR_PP_HBA_REQ_ADDR               = 639,
//     STR_PP_BAD_HBA_ADDR               = 640,
//     STR_PP_1F0_PRIMARY                = 641,
//     STR_THRESHOLD_1                   = 642,
//     STR_THRESHOLD_2                   = 643,
//     STR_THRESHOLD_3                   = 644,
//     STR_THRESHOLD_4                   = 645,
//     STR_LOG_INSTRUCTIONS              = 646,
//     STR_DISPLAY_THRESHOLD             = 647,
//     STRT_CLEAR_LOG                    = 648,
//     STR_PROCESSOR_EXEP                = 649,
//     STR_FIRMWARE_EXEP                 = 650,
//     STR_POWER_UP_FAIL                 = 651,
//     STR_FIRMWARE_ERROR                = 652,
//     STR_BLINKLED                      = 653,
//     STR_BLINK_80                      = 654,
//     STR_BLINK_81                      = 655,
//     STR_BLINK_82                      = 656,
//     STR_BLINK_83                      = 657,
//     STR_BLINK_84                      = 658,
//     STR_BLINK_86                      = 659,
//     STR_BLINK_87                      = 660,
//     STR_BLINK_RESET_STUCK             = 661,
//     STR_BLINK_71                      = 662,
//     STR_BLINK_72                      = 663,
//     STR_BLINK_73                      = 664,
//     STR_BLINK_74                      = 665,
//     STR_BLINK_UNDEF                   = 666,
//     STR_BLINK_HWERR                   = 667,
//     STR_BLINK_0X12                    = 668,
//     STR_BLINK_0X15                    = 669,
//     STR_BLINK_0X16                    = 670,
//     STR_BLINK_0X17                    = 671,
//     STR_BLINK_PCIXFERERR              = 672,
//     STR_BLINK_PCIPARERR               = 673,
//     STR_BLINK_PCICFGERR               = 674,
//     STR_BLINK_ECCMISMATCH             = 675,
//     STR_BLINK_MEMPM3XXX               = 676,
//     STR_BLINK_MEMPM2XXX               = 677,
//     STR_BLINK_DOMMISMATCH             = 678,
//     STR_BLINK_DOMINVALID              = 679,
//     STR_BLINK_UNSUPP_SG               = 680,
//     STR_BLINK_NVRAM_CLR               = 681,
//     STR_BLINK_CACHE_INV               = 682,
//     STR_BLINK_CACHE_MIS               = 683,
//     STR_BLINK_CACHE_OUT               = 684,
//     STR_BLINK_HW_FAIL1                = 685,
//     STR_BLINK_HW_FAIL2                = 686,
//     STR_BLINK_HW_FAIL3                = 687,
//     STR_BLINK_HW_UNKNOWN              = 688,
//     STR_BLINK_OS_TASK_FAIL            = 689,
//     STR_BLINK_OS_HISR_FAIL            = 690,
//     STR_BLINK_OS_QUECR_FAIL           = 691,
//     STR_BLINK_OS_SEM_FAIL             = 692,
//     STR_BLINK_OS_LISR_FAIL            = 693,
//     STR_BLINK_OS_QUEUE_FAIL           = 694,
//     STR_BLINK_OS_ERR_LOOP             = 695,
//     STR_BLINK_OS_GEN_ERR              = 696,
//     STR_COULD_NOT_CREATE              = 697,
//     STR_STAT_INVALID                  = 698,
   "- security stamp missing",      //     STR_STAT_1                        = 699,
   "- Non-supported channel",       //     STR_STAT_2                        = 700,
   "- Non-supported ID",            //     STR_STAT_3                        = 701,
   "- Non-supported channel and ID",//     STR_STAT_4                        = 702,
   "- Component failure",           //     STR_STAT_5                        = 703,
   "- Test Unit Ready failure",     //     STR_STAT_6                        = 704,
   "- Format failure",              //     STR_STAT_7                        = 705,
   "- Write failure",               //     STR_STAT_8                        = 706,
   "- User failed via Mode Select", //     STR_STAT_9                        = 707,
   "- Start-of-Day failure",        //     STR_STAT_10                       = 708,
   "No action in progress",         //     STR_STAT_11                       = 709,
   "Format initiated",              //     STR_STAT_12                       = 710,
   "Rebuild initiated",             //     STR_STAT_13                       = 711,
   "No information available",      //     STR_STAT_14                       = 712,
   "Wrong sector size",             //     STR_STAT_15                       = 713,
   "Wrong capacity",                //     STR_STAT_16                       = 714,
   "Incorrect mode parameters",     //     STR_STAT_17                       = 715,
   "Wrong controller serial number",//     STR_STAT_18                       = 716,
   "Channel mismatch",              //     STR_STAT_19                       = 717,
   "ID mismatch",                   //     STR_STAT_20                       = 718,
   "is at the channel ID",          //     STR_STAT_21                       = 719,
   "drive format initiated",        //     STR_STAT_22                       = 720,
   "Wrong drive replaced",          //     STR_STAT_23                       = 721,
   "awaiting build",                         //     STR_STAT_24                       = 722,
   "integration in progress",                //     STR_STAT_25                       = 723,
   " in progress",                           //     STR_STAT_26                       = 724,
   "/Fix in progress",                       //     STR_STAT_27                       = 725,
   "Parameter mismatch",               // STR_PARAMETER_MISMATCH                       = 726,
   "Replaced RAID 0 drive formatting", // STR_REPLACED_FORMAT
   "Drive Failed",                     // STR_DRIVE_FAILED
   "Replaced Drive being formatted",         //     STR_STAT_31                       = 729,
   "Multiple drive failures",                //     STR_STAT_32                       = 730,
   "Format in progress",                     //     STR_STAT_33                       = 731,
   "Awaiting build",                         //     STR_STAT_34                       = 732,
   "Verify found data inconsistent",   //     STR_STAT_35                       = 733,
//     STR_OSS_OS2                       = 734,
//     STR_OSS_UNIX                      = 735,
//     STR_OSS_NETWARE_286               = 736,
//     STR_OSS_NETWARE_386               = 737,
//     STR_OSS_SCO_UNIX                  = 738,
//     STR_PROC_8086                     = 739,
//     STR_PROC_80286                    = 740,
//     STR_PROC_80386                    = 741,
//     STR_PROC_80486                    = 742,
//     STR_PROC_PENTIUM                  = 743,
//     STR_PROC_PENTIUM_PRO              = 744,
//     STR_NEW_ARRAY_NO_COVERAGE         = 745,
//     STR_HS_NO_PROTECT                 = 746,
//     STR_HS_SOME_PROTECT               = 747,
//     STR_WINDOWS_PATH                  = 748,
//     STR_INT24_WRITE_PROTECT           = 749,
//     STR_INT24_UNKNOWN_UNIT            = 750,
//     STR_INT24_NOT_READY               = 751,
//     STR_INT24_GENERIC                 = 752,
//     STR_INT24_DATA_ERROR              = 753,
//     STR_INT24_SEEK_ERROR              = 754,
//     STR_INT24_UNKNOWN_MEDIA_TYPE      = 755,
//     STR_INT24_SECTOR_NOT_FOUND        = 756,
//     STR_PRINT_NO_FILENAME             = 757,
//     STR_PRINT_TO_CONSOLE              = 758,
   "System Console", // STR_SYS_CONSOLE
//    STR_NO_SPACE_FOR_RAID_TABLE       = 759,
//    STR_HBA_NO_RAID_MODULE            = 760,
//    STR_NO_WINDOWS_IN_PATH            = 761,
//    STR_FORMAT_UNSUCCESSFUL           = 762,
//    STR_PRINTED_BY                    = 763,
//    STR_TEXT_MISSING                  = 764,
//    STR_TEXT_IMPACTED                 = 765,
//    STR_TEXT_WARNING                  = 766,
//    STR_TEXT_FAILED                   = 767,
//    STR_TEXT_BUILDING                 = 768,
   "No Sense\n",   //     STR_SENSE_0                       = 769,
   "Recovered Error\n",    //     STR_SENSE_1                       = 770,
   "Not Ready\n",  //     STR_SENSE_2                       = 771,
   "Medium Error\n",   //     STR_SENSE_3                       = 772,
   "Hardware Error\n", //     STR_SENSE_4                       = 773,
   "Illegal Request\n",    //     STR_SENSE_5                       = 774,
   "Unit Attention\n", //     STR_SENSE_6                       = 775,
   "Data Protect\n",   //     STR_SENSE_7                       = 776,
   "Blank Check\n",    //     STR_SENSE_8                       = 777,
   "Vendor Specific\n",    //     STR_SENSE_9                       = 778,
   "Copy Aborted\n",   //     STR_SENSE_A                       = 779,
   "Aborted Command\n",    //     STR_SENSE_B                       = 780,
   "Equal\n",  //     STR_SENSE_C                       = 781,
   "Volume Overflow\n",    //     STR_SENSE_D                       = 782,
   "Miscompare\n", //     STR_SENSE_E                       = 783,
   "Reserved\n",   //     STR_SENSE_F                       = 784,
   "isa", // STR_ISA
   "eisa", // STR_EISA
   "pci", // STR_PCI
   "Verify",   //     STR_VERIFY                        = 788,
   "Degraded -",   //     STR_STAT_DEGRADED                 = 789,
   "Reconstructing -", //     STR_STAT_RECONSTRUCT              = 790,
   "Expanding",    //     STR_STAT_EXPAND                   = 791,
   "Dead -",   //     STR_STAT_DEAD                     = 792,
   "Array In Warning -",   //     STR_ARRAY_WARNING                 = 793,
   "Non-Arrayed",  //     STR_NON_ARRAYED                   = 794,
   "Non-existent drive",   //     STR_NON_EXISTANT                  = 795,
   "Failed drive", //     STR_FAILED_DRIVE                  = 796,
   "Replaced drive -", //     STR_REPLACED_DRIVE                = 797,
   "Drive in warning condition",   //     STR_DRIVE_WARNING                 = 798,
   "Parameter mismatch (drive) -", //     STR_PARAM_MISMATCH                = 799,
   "Format Unit -",    //     STR_FORMAT_UNIT                   = 800,
   "Uninitialized -",  //     STR_UNINITIALIZED_HYPH            = 801,
   "Drive Verify", //     STR_DRIVE_VERIFY                  = 802,
//    STR_DUMMY_CDD                     = 803,
//    STR_DPTDDL_PATH                   = 804,
//    STR_DPT_DEVICE_ENTRY              = 805,
   "\n%sSCSI Bus ECC Error\n", //     STR_SCSI_BUS_ECC_ERR              = 806,
   "Correctable ", //     STR_CORRECTABLE                   = 807,
   "Non-Correctable ", //     STR_NONCORRECTABLE                = 808,
//    STR_CORRECTED                     = 809,
   "Parity",   //     STR_PARITY                        = 810,
   "ECC",  //     STR_ECC,                           = 811,
   "Storage Subsystem - Component Failed\n",   //     STR_SS_COMP_FAILED                = 812,
   "Storage Subsystem - Fully Functional\n",   //     STR_SS_FULLY_FUNC                 = 813,
//     STR_ALL_IDS_IN_USE                = 814,
//     STR_CURRENT_ADDR                  = 815,
//     STR_NEW_SCSI_ADDR                 = 816,
//     STRT_SEARCH_AGAIN                 = 817,
//     STR_DPT_ENGINE                    = 818,
//     STR_DSM_INCOMPATIBLE              = 819,
//     STR_NOT_COMPATIBLE                = 820,
//     STRT_GLOSSARY                     = 821,
//     STRT_BACK                         = 822,
//     STR_OS_MISMATCH                   = 823,
//     STRT_PRINTER_SETUP                = 824,
//     STR_LOAD_CONFIG                   = 825,
//     STR_SAVE_CONFIG                   = 826,
   "Host Command Received",    //     STR_HOST_CMD_REC                  = 827,
   "Host Command Sent",    //     STR_HOST_CMD_SENT                 = 828,
//     STR_ARRAY_DIAGS                   = 829,
//     STR_ECC_ENABLED                   = 830,
//     STR_ENABLE_DPT_ECC                = 831,
//     STR_SLOT                          = 832,
//     STR_DRIVER_NO_STRIPE              = 833,
//     STR_STRIPE_CHANGE                 = 834,
//     STR_CANNOT_FIND_SERVER            = 835,
//     STR_COMM_ENGINE                   = 836,
//     STR_INCOMPATIBLE_COMM             = 837,
//     STR_IMAGE_WRONG_SIZE              = 838,
//     STR_ONE_FLOPPY_RIGHT_SIZE         = 839,
//     STR_IMAGE_SIZE_MISMATCH           = 840,
   "The specified file does not match the controller.\n", // STR_FILE_NOT_MATCH_HBA
//     STR_INVALID_IMAGE_FILE            = 841,
//     STR_WRITE_ERROR                   = 842,
//     STR_ZAPPED                        = 843,
//     STR_OST_DOS                       = 844,
//     STR_OST_WINDOWS                   = 845,
//     STR_OST_WINDOWS95                 = 846,
//     STR_OST_WINDOWS_NT                = 847,
//     STR_OST_MSOS2                     = 848,
//     STR_OST_IBMOS2                    = 849,
//     STR_OST_NOVELL                    = 850,
//     STR_OST_SCO                       = 851,
//     STR_OST_USL                       = 852,
//     STR_OST_SVR42                     = 853,
//     STR_OST_UNIXWARE                  = 854,
//     STR_OST_SOLARIS                   = 855,
//     STR_OST_INTERACTIVE               = 856,
//     STR_OST_ATT_UNIX                  = 857,
//     STR_OST_NEXT                      = 858,
//     STR_OST_UNIX                      = 859,
//     STR_OST_VINES                     = 860,
//     STR_OST_PICK                      = 861,
//     STR_OST_THEOS                     = 862,
//     STR_OST_LINUX                     = 863,
//     STR_OST_BSDI                      = 864,
//     STR_OST_FREEBSD                   = 865,
//     STR_OST_AIX                       = 866,
//     STR_OST_NETBSD                    = 867,
//     STR_OST_OTHER                     = 868,
//     STR_OS_DOS                        = 869,
//     STR_OS_DOS5                       = 870,
//     STR_OS_DRDOS                      = 871,
//     STR_OS_CONDOS                     = 872,
//     STR_OS_ZENITH                     = 873,
//     STR_OS_PCMOS                      = 874,
//     STR_OS_OS2MMS                     = 875,
//     STR_OS_OS2MIBM                    = 876,
//     STR_OS_OS2L                       = 877,
//     STR_OS_OS220                      = 878,
//     STR_OS_OS221                      = 879,
//     STR_OS_OS221_AND_UP               = 880,
//     STR_OS_OS2OTHER                   = 881,
//     STR_OS_NW311                      = 882,
//     STR_OS_NW40                       = 883,
//     STR_OS_NW21X                      = 884,
//     STR_OS_NW22                       = 885,
//     STR_OS_NW20                       = 886,
//     STR_OS_CONSENSYS                  = 887,
//     STR_OS_IF_SVR4                    = 888,
//     STR_OS_MICROPORT                  = 889,
//     STR_OS_UHC                        = 890,
//     STR_OS_SCO_UNIX_320               = 891,
//     STR_OS_SCO_UNIX_322               = 892,
//     STR_OS_SCO_UNIX_324               = 893,
//     STR_OS_SCO_UNIX_3242              = 894,
//     STR_OS_SCO_XENIX                  = 895,
//     STR_OS_INT_UNIX_1X                = 896,
//     STR_OS_INT_UNIX_2X                = 897,
//     STR_OS_INT_UNIX_3X                = 898,
//     STR_OS_ESIX                       = 899,
//     STR_OS_INTEL_UNIX                 = 900,
//     STR_OS_MICROPORT_286              = 901,
//     STR_OS_MICROPORT_386              = 902,
//     STR_OS_VINES_411                  = 903,
//     STR_OS_VINES_55                   = 904,
//     STR_OS_VINES_6X                   = 905,
//     STR_OS_VINES_7X                   = 906,
//     STR_OS_MAGIX                      = 907,
//     STR_OS_PICK22                     = 908,
//     STR_OS_PICK21                     = 909,
//     STR_OS_QNX                        = 910,
//     STR_OS_WYSE                       = 911,
//     STR_OS_INT13                      = 912,
//     STR_OS_WD1003                     = 913,
//     STR_OS_NOTLISTED                  = 914,
//     STR_YOUR_CHOSEN_OS                = 915,
//     STR_OST_OLIVETTI                  = 916,
//     STR_OS_NEXT                       = 917,
//     STR_OS_NEXT33                     = 918,
//     STR_OS_UNIXWARE1X                 = 919,
//     STR_OS_UNIXWARE2X                 = 920,
//     STR_OS_LINUX                      = 921,
//     STR_OS_BSDI                       = 922,
//     STR_OS_FREEBSD                    = 923,
//     STR_OS_AIX                        = 924,
//     STR_OS_NETBSD                     = 925,
//     STR_OS_DGUNIX                     = 926,
//     STR_OS_MACOS                      = 927,
//     STR_QUIET_TITLE                   = 928,
//     STR_QUIET_DESCRIP                 = 929,
//     STR_SCSI_II                       = 930,
//     STRT_CREATE_DISK                  = 931,
//     STR_BAD_EXCLUSION                 = 932,
//     STR_PRIORITY                      = 933,
//     STR_EX_PERIOD                     = 934,
//     STR_START_COLON                   = 935,
//     STR_END_COLON                     = 936,
   "Enabled", // STR_ENABLED
   "Current Sensor", // STR_CURRENT_SENSOR
   "Voltage Sensor", // STR_VOLT_SENSOR
   "Temperature Sensor", // STR_TEMP_SENSOR
   "enclosure temperature sensor #", // STR_ENCL_TEMPERATURE_SENSOR
   "Normal temperature restored", // STR_NORMAL_TEMP_RESTORED
   "Temperature Normal", // STR_TEMP_NORMAL
   "High temperature",  // STR_HIGH_TEMP
   "Low temperature",  // STR_LOW_TEMP
   " detected\n",  // STR_DETECTED
   "Very high temperature detected\n", //     STR_VERY_HIGH_TEMP
   "Temperature Out Of Range", // STR_TEMP_OUT_OF_RANGE
   "Low voltage detected\n",   //     STR_LOW_VOLTAGE
   "Normal voltage restored\n",    //     STR_NORMAL_VOLTAGE
   "Low voltage detected\n",   //     STR_HIGH_VOLTAGE
   "Diagnostic ",  //     STR_DIAGNOSTIC
   "Stopped: Drive doesn't support command\n", //     STR_STOPPED_DDS
   "Stopped: Drive failure\n", //     STR_STOPPED_FAIL
   "Stopped: Error limit exceeded\n",   //     STR_STOPPED_LIMIT
   "Stopped: User intervention\n", //     STR_STOPPED_USER
   "Stopped: Test completed\n",    //     STR_STOPPED_COMPLETE
   "S.M.A.R.T. threshold reached\n",   //     STR_SMART_THRESHOLD
   "Internal Drive Prediction\n",  //     STR_DRIVE_PREDICTION
   "Spin up criterion met\n",  //     STR_SPINUP_CRITERION
   "Seek criterion met\n", //     STR_SEEK_CRITERION
   "Scheduled\n",  //     STR_SCHEDULED
   "Queried\n",    //     STR_QUERIED
   "Canceled\n",   //     STR_CANCELED
   "Exclusion period set\n",   //     STR_EX_PERIOD_SET
   "Debug Event\n",    //     STR_DEBUG_EVENT
   "%02d/%02d/%02d  %02d:%02d:%02d   ",    // STR_LOG_TIME
   "Level %d", // STR_LOG_LEVEL
//     STR_WRAP_OFF
//     STR_LOG_SEPARATOR
//     STR_SAVE_CHANGES
//     STR_MUST_REBOOT
//     STR_DEV_CONFIG
//     STR_READ_CACHING
//     STRT_DISCONNECT
//     STRT_FAIL
//     STR_START_DURING_EX
//     STR_STOP_BUILD_BAD
//     STR_STOP_GEN5_BUILD_BAD
//     STR_BOTH_FAILED
//     STRT_CONFIGURE
//     STR_SECTORS_COLON
//     STR_BYTES_SECTOR_LOGICAL
//     STR_BYTES_SECTOR
//     STR_SCSI_3                        = 977,
//     STR_SMART                         = 978,
//     STR_REBUILD                       = 979,
   "SAF-TE", // STR_SAFTE
   "SES", // STR_SES
//     STR_SAFTE_ENCL                    = 981,
//     STR_CE_ENOENT                     = 982,
//     STR_CE_EMFILE                     = 983,
//     STR_CE_EACCES                     = 984,
//     STR_CE_EINVACC                    = 985,
//     STR_CF_NOSPACE                    = 986,
//     STR_CF_OTHER                      = 987,
//     STR_CF_NOOPEN_SRC                 = 988,
//     STR_WIN_INSTALL                   = 989,
//     STR_UNCONF_EISA                   = 990,
//     STR_WIN32_NODASD                  = 991,
//     STR_FLASH_NEEDED                  = 992,
//     STR_VALID_FLASH_FILE              = 993,
//     STR_SUCCESSFUL_FLASH              = 994,
//     STR_FLASH_DL_ERROR                = 995,
//     STR_VERIFY_FLASH                  = 996,
//     STR_FLASH_FIRMWARE                = 997,
//     STR_FLASH_BIOS                    = 998,
//     STR_FLASH_SMOR                    = 999,
//     STR_BATTERY_MISSING               = 1000,
//     STR_BATTERY_PRESENT               = 1001,
   "Battery Status Change", // STR_BATT_STAT_CHANGE
//     STR_NORMAL                        = 1002,
//     STR_LOW                           = 1003,
//     STR_HIGH                          = 1004,
//     STR_VOLTAGE                       = 1005,
//     STR_SERIALNUM                     = 1006,
//     STR_STANDARD_CONNECTIONS          = 1007,
//     STR_CUSTOM_CONNECTIONS            = 1008,
//     STR_PROTOCOL_TCPIP                = 1009,
//     STR_PROTOCOL_SPX                  = 1010,
//     STR_PROTOCOL_SERIAL               = 1011,
//     STR_PROTOCOL_MODEM                = 1012,
//     STR_MODEM                         = 1013,
//     STR_CONNECTION_INFORMATION        = 1014,
//     STRT_ADDRESS                      = 1015,
//     STRT_USERNAME                     = 1016,
//     STRT_PASSWORD                     = 1017,
//     STR_PHONEBOOK                     = 1018,
//     STRT_PHONEBOOK                    = 1019,
//     STRT_DISPLAY_ICON                 = 1020,
//     STRT_PROTOCOL                     = 1021,
//     STRT_SERVERNAME                   = 1022,
//     STRT_ADD                          = 1023,
//     STRT_EDIT                         = 1024,
//     STRT_SAVE                         = 1025,
//     STRT_DELETE                       = 1026,
//     STRT_CONNECT                      = 1027,
//     STRT_CLOSE                        = 1028,
//     STR_CHOOSE_ICON                   = 1029,
//     STRT_CHOOSE_ICON                  = 1030,
//     STRT_ICONS                        = 1031,
//     STR_SX4000                        = 1032,
//     STR_TERMINATOR                    = 1033,
//     STR_RAID_CTLR                     = 1034,
//     STR_SES_ENCL                      = 1035,
//     STR_SAFTE_SES                     = 1036,
//     STR_XFER                          = 1037,
//     STR_FCAL                          = 1038,
//     STR_FIBRE_CHANNEL                 = 1039,
//     STR_132                           = 1040,
//     STR_264                           = 1041,
//     STR_64BIT_SLOT                    = 1042,
//     STR_32BIT_SLOT                    = 1043,
//     STR_EVENT_VIEWER                  = 1044,
//     STR_TAPI                          = 1045,
//     STR_LOG_DESKTOP                   = 1046,
//     STR_BROADCAST_TO_MACHINE          = 1047,
//     STR_LOG_COMPUTERS                 = 1048,
   "Fail Hot Spar", // STR_FAILED_HOTSPARE
//     STRT_EXPAND_RAID                  = 1050,
   "Ultra",       // STR_ULTRA
   "Ultra2",      // STR_ULTRA2
   "Ultra3",      // STR_ULTRA3
   "Ultra160",    // STR_ULTRA160
   "Ultra320",    // STR_ULTRA320
//     STR_NRAM_CACHE_NOTE               = 1052,
//     STR_CACHE_INHIBIT_COMMANDS        = 1053,
//     STR_CACHE_ADVISORY                = 1054,
//     STR_CACHE_MANDATORY               = 1055,
//     STR_CACHE_SYNC_COMMANDS           = 1056,
//     STR_CACHE_SIZE                    = 1057,
//     STR_CACHE_VIRTUAL                 = 1058,
//     STR_CACHE_PHYSICAL                = 1059,
//     STR_CACHE_CONTROL                 = 1060,
//     STR_CACHE_WRITE                   = 1061,
//     STR_CACHE_WRITE_BACK              = 1062,
//     STR_CACHE_WRITE_THROUGH           = 1063,
   "Cache disabled", // STR_CACHE_DISABLED
//     STR_TRACE_HOST_COMMANDS           = 1064,
//     STR_TRACE_SCSI_COMMANDS           = 1065,
   "No battery", // STR_NO_BATTERY
   "Bad Battery", // STR_BAD_BATTERY
//     STR_BBU_WND_TITLE                 = 1066,
//     STR_BBU_NOT_DETECTED              = 1067,
//     STR_BBU_CHECK_CONNECTIONS         = 1068,
//     STR_BBU_STATUS                    = 1069,
   "Starting up", // STR_BBU_STARTING
   "Operational", // STR_BBU_OPERATIONAL
//     STR_BBU_BACKUP                    = 1070,
//     STR_BBU_CAP_WARNINGS              = 1071,
//     STR_BBU_AUTO_WT                   = 1072,
   "Predictive Failure", // STR_BBU_PRED_FAILURE
//     STR_BBU_INIT_CAL_BUTTON           = 1074,
//     STR_BBU_MAINT_CAL_BUTTON          = 1075,
//     STR_BBU_DEFAULT_BUTTON            = 1076,
//     STR_BBU_INIT_DLG_TITLE            = 1077,
//     STR_BBU_INIT_DLG_MSG              = 1078,
//     STR_BBU_MAINT_DLG_TITLE           = 1079,
//     STR_BBU_MAINT_DLG_MSG             = 1080,
//     STR_BBU_GEN_ERROR_MSG             = 1081,
//     STR_BBU_GEN_ERROR_TITLE           = 1082,
   "Full", // STR_BBU_FULL
   "Charging", // STR_BBU_CHARGING
   "Discharging", // STR_BBU_DISCHARGING
   "Recharging", // STR_RECHARGING
   "Bad", // STR_BBU_BAD
   "Init chrg", // STR_BBU_INIT_CHARGE
   "Init dschrg", // STR_BBU_INIT_DISCHARGE
   "Init rechrg", // STR_BBU_INIT_RECHARGE
   "Maint dschg", // STR_BBU_MAINT_DISCHARGE
   "Maint chrg", // STR_BBU_MAINT_CHARGE
   "Trickle Charging", // STR_TRICKLE_CHRGING
   "Maintenance Calibration", // STR_MAINT_CALIB
   "Initial Calibration", // STR_INIT_CALIB
//     STR_BBU_AUTO_WT_MESSAGE           = 1092,
//     STR_MB_SEC_XFER                   = 1093,
//     STR_IN_MAINT_WT_MODE              = 1094,
//     STR_IN_FORCED_WT_MODE             = 1095,
//     STR_NO_MAINT_PERFORMED            = 1096,
//     STR_BBU_AUTO_PRED_MSG             = 1097,
//     STR_AUTO_WT_WARNING               = 1098,
//     STR_ADVANCED                      = 1099,
//     STR_DIRECTORY_FILE                = 1100,
//     STR_SUCCESSFUL_FLASH_REMOTE       = 1101,
//     NUM_STRINGS                       = 1102
   "Degraded",       // STR_DEGRADED      = 1103
   "Reconstruct",    // STR_RECONSTRUCT	= 1104
	"Expanding",      // STR_EXPAND        = 1105
	"Dead",           // STR_DEAD          = 1106
   "Array in Warn",  // STR_ARRAY_WARNING2= 1107
   "Verifying",      // STR_VERIFY        = 1108
   "Non-existent",   // STR_NON_EXIST     = 1109
   "Uninitialized",  // STR_UNINITIALIZED = 1110
   "Replaced Drive", // STR_REPLACED_DRIVE2=1111
   "Drive in Warn",  // STR_DRIVE_WARNING2 =1112
   "Parm Mismatch",  // STR_PARAM_MISMATCH2=1113
   "Format Unit",    // STR_FORMAT_UNIT2  = 1114
   "SES Enclosure",   // STR_SES_ENCL,
   "SAF-TE Enclosure",   // STR_SAFTE_ENCL,
   "DEC Enclosure",   // STR_DEC_ENCL,
   "Nile Enclosure",    // STR_NILE_ENCL
   "Communication Lost",  // STR_COMMUNICATION_LOST
   "Enclosure Found",  //   STR_SES_ENCL_FOUND,
   "Door Lock", // STR_DOOR_LOCK
   "Audible Alarm", // STR_AUD_ALARM
   "Selection Timeout",  //   STR_SES_ENCL_SELECTION_TIMEOUT,
   "Hardware Error",  //   STR_SES_ENCL_HARDWARE_ERROR,
   "Error Limit Exceeded",  //   STR_SES_ENCL_ERROR_LIMIT,
   "CPU Register Dump  : ",  //   STR_CPU_REGISTER_DUMP,
   "BlinkLED: Type = %.2x, Code = %.2x",  //   STR_BLINKLED_EVENT
   "Are you sure you want to Zap the drive(s) [yN]? ",  // STR_RU_SURE_TO_ZAP
   "Yy", // STR_CHAR_Y  for answering yes (Y or y)
   "This drive is busy. ", // STR_DRIVE_IS_BUSY
   "Operation and Resync successful, reboot unnecessary.\n", // STR_SUCCESS_RESYNC_SUCCESS
   "Operation successful, however Resync not supported on this OS, please reboot.\n", // STR_SUCCESS_RESYNC_NOT_SUPPORTED
   "Operation successful, however Resync failed, please reboot.\n", // STR_SUCCESS_RESYNC_FAILED
   "\nUart data for hba number %d:\n", //STR_UART_HBA_NUM
   "\nHba number %d not found\n", //STR_HBA_NUM_NOT_FOUND
   "Error occurred getting uart data from hba %d\n", //STR_ERR_UART_HBA_NUM
   "Uart data written to file for hba %d\n", //STR_UART_WRITTEN_TO_FILE
   "No uart data for hba %d\n", //STR_NO_UART_DATA
   "RAID 1 must be rebuilt\n", // STR_MUST_REBUILD_RAID1
   "%s/Pending", // STR_STR_OVER_PENDING
   "  Read Cache ", // STR_READ_CACHE
   "  Write Cache ", // STR_WRITE_CACHE
   " (Off due to maintenance operation)", // STR_OFF_DUE2_MAINT_OPER
   " (Off due to battery condition)", // STR_OFF_DUE2_BATTERY_COND
   "  Error occurred, no change made!\n", // STR_ERR_NO_CHANGE
   "Resync Successful, reboot unnecessary.\n", // STR_RESYNC_SUCCESS_NO_REBOOT
   "Resync not supported on this OS, please reboot.\n", // STR_RESYNC_NOT_SUPPORTED_REBOOT
   "Resync Failed, please reboot.\n", // STR_RESYNC_FAILED_REBOOT
   "You must be root to run this utility\n", // STR_MUST_BE_ROOT
   "Press ENTER to continue\n", // STR_PRESS_ENTER2_CONT
   "Integer not expected, use format d#b#t#d# or d#b#t##d#\n", // STR_INT_NOT_EXPECTD_USE
   "Logical View\n", // STR_LOGICAL_VIEW
   "Physical View\n", // STR_PHYSICAL_VIEW
   " Drive not Optimal, cannot change\n", // STR_DRIVE_NOT_OPT_CANT_CHG
   " unchanged - Optimal\n", // STR_UNCHANGED_OPTIMAL
   " Raid not Failed, cannot change\n", // STR_RAID_NOT_FAIL_CANT_CHG
   " unchanged - Failed\n", // STR_UNCHANGED_FAILED
   "NVRAM", // STR_NVRAM
   "%8s:%6ld Bytes\n", // STR_SCAN_TYPE_SIZE
   "%8s:%6ld Bytes %s %s\n", // STR_SCAN_TYPE_SIZE_DATE_VERSION
   "Controller not found.\n", // STR_CTLR_NOT_FOUND
   "Flashing complete.\n", // STR_FLASH_COMPLETE
   "An error occurred while flashing data to the controller.\n(at %lu bytes, Error=0x%X: %s)\n", // STR_FLASH_ERR_MSG
   "An error occurred while verifying the data that was flashed to the controller.\n(at %lu bytes, Error=0x%X: %s)\n", // STR_VERIFYING_ERR_MSG
   "Warning:  This feature is valid only for Windows NT and Windows 2000.\n", // STR_WARN_ONLY_NT_2000
   "The hba's event log has been cleared.\n", // STR_EVENT_LOG_CLEARED
   "for Channel", // STR_FOR_CHANNEL
   "Double Component Failed", // STR_DOUBLE_COMP_FAIL
   "The server ", // STR_THE_SERVER
   " could not ", // STR_COULD_NOT
   "a heartbeat", // STR_A_HEARTBEAT
   "/clear", // STR_SLASH_CLEAR
   " finished ", // STR_FINISHED
   "error", // STR_ERROR
   "Swap Signal Detected", // STR_SWAP_SIG_DETECTED
   "component malfunction", // STR_COMP_MALFUNC
   "component operational", // STR_COMP_OPERATIONAL
   "Dirty data", // STR_DIRTY_DATA
   "No Battery Detected or Battery CRC Failure", // STR_CRC_FAILURE
   "Local Parameter Failure", // STR_LOCAL_PARAM_FAILURE
   "Local Signature Failure", // STR_LOCAL_SIG_FAILURE
   "Cache Signature Failure", // STR_CACHE_SIG_FAILURE
   "LSU Device ID is Zero", // STR_LSU_DEV_ID_ZERO
   "No Cache", // STR_NO_CACHE
   "Invalid PCB", // STR_INVALID_PCB
   "PCB No LSU", // STR_PCB_NO_LSU
   "PCB Count", // STR_PCB_COUNT
   "Dirty Block Count", // STR_DIRTY_BLOCK_COUNT
   "LSU has Invalid Device ID", // STR_LSU_INVALID_DEV_ID
   "LSU has an Invalid Type", // STR_LSU_INVALID_TYPE
   "LSU Parent Device TID", // STR_LSU_PARENT_DEV_ID
   "LSU Capacity", // STR_LSU_CAPACITY
   "LSU Magic Number", // STR_LSU_MAGIC_NBR
   "Volts high", // STR_VOLTS_HIGH
   "voltage restored", // STR_VOLT_RESTORED
   "Volts low", // STR_VOLTS_LOW
   "Term Power ", // STR_TERM_POWER
   "Bad Data Block", // STR_BAD_DATA_BLOCK
   "Bad Parity Block", // STR_BAD_PARITY_BLOCK
   "Bad Data List Full, Block", // STR_BAD_DATA_LIST_FULL_BLOCK
   "Bad Parity List Full, Block", // STR_BAD_PARITY_LIST_FULL_BLOCK
   "Write Back Failure, Block", // STR_WRITE_BACK_FAIL_BLOCK
   "for", // STR_FOR
   "Global Cache State Change", // STR_GLOBAL_CACHE_STATE_CHG
   "External device or initator caused incoming bus reset", // STR_CAUSE_OF_BUS_RESET
   "Command from host requested that the bus to be reset", // STR_HOST_CMD_BUS_RESET
   "Command watchdog time-out caused the bus to be reset", // STR_WATCHDOG_BUS_RESET
   "Bus reset occurred on channel", // STR_BUS_RESET_ON_CHAN
   "The Fibre Loop on channel %d went from ", // STR_FIBRE_LOOP_CHAN_FROM
   "down to up\n", // STR_DOWN_TO_UP
   "up to down\n", // STR_UP_TO_DOWN
   "Access to Fibre device ID %d failed on Channel %d\n", // STR_ACCESS_FIBRE_FAILED_FILLIN
   "Unknown flags for Fibre Loop event\n", // STR_UNKNOWN_FLAGS_FIBRE_LOOP
   "Battery Calibration %s, Count = %d\n", // STR_BATTERY_CALIB_COUNT
   "Channel %d was ", // STR_CHAN_FILLIN_WAS
   "taken offline", // STR_TAKEN_OFFLINE
   "left offline", // STR_LEFT_OFFLINE
   "after ISP processor diagnostic check failed\n", // STR_ISP_PROC_DIAG_CHK_FAILED
   "after excessive reinitializes of the ISP sub-system\n", // STR_EXCESS_REINIT_ISP_SUB
   "after excessive inbound resets occurred\n", // STR_EXCESS_INBOUND_RESETS
   "because ISP was unable to reset the SCSI-FIBRE bus\n", // STR_ISP_UNABLE_RESET_SFBUS
   "because the ISP received an unrecoverable PCI bus fault\n", // STR_ISP_RECV_UNRECOV_PCIBUS_FAULT
   "because the ISP failed to initialize\n", // STR_ISP_FAILED_INIT
   "(unknown reason code)\n", // STR_UNKNOWN_REASON_CODE
   "down speed occurred on \nbus %d, id %d, lun %d\ndue to Domain Validation\n", // STR_DOWN_SPEED_DOMAIN_VALID
   "device located at bus %d, id %d, lun %d\nmay not be seen as valid SCSI device due to\nDomain Validation failure to complete\n", // STR_NOT_SCSI_DEV_DOMAIN_VALID
   "Error in log message\n", // STR_ERR_IN_LOG_MSG
   "Unknown Event", // STR_UNKNOWN_EVENT
   "Success", // STR_SUCCESS
   "Request Aborted", // STR_REQ_ABORTED
   "Unable To Abort", // STR_UNABLE_TO_ABORT
   "Complete With Error", // STR_COMPLETE_WITH_ERROR
   "Adapter Busy", // STR_ADAPTER_BUSY
   "Request Invalid", // STR_REQUEST_INVALID
   "Path Invalid", // STR_PATH_INVALID
   "Unable To Terminate", // STR_UNABLE_TO_TERMINATE
   "MR Message Received", // STR_MR_MSG_RECVD
   "Parity Error Failure", // STR_PARITY_ERR_FAILURE
   "Autosense Failed", // STR_AUTOSENSE_FAILED
   "No Adapter", // STR_NO_ADAPTER
   "Data Overrun", // STR_DATA_OVERRUN
   "Sequence Failure", // STR_SEQ_FAILURE
   "Request Length Error", // STR_REQ_LEN_ERROR
   "Provide Failure", // STR_PROVIDE_FAILURE
   "BDR Message Sent", // STR_BDR_MSG_SENT
   "Request Terminated", // STR_REQ_TERM
   "IDE Message Sent", // STR_IDE_MSG_SENT
   "Resource Unavailable", // STR_RESOURCE_UNAVAIL
   "Unacknowledged Event", // STR_UNACKED_EVENT
   "Message Received", // STR_MSG_RECVD
   "Invalid CDB", // STR_INVALID_CDB
   "LUN Invalid", // STR_LUN_INVALID
   "SCSI TID Invalid", // STR_SCSI_TID_INVALID
   "Function Unavailable", // STR_FUNC_UNAVAIL
   "No Nexus", // STR_NO_NEXUS
   "SCSI IID Invalid", // STR_SCSI_IID_INVALID
   "CDB Received", // STR_CDB_RECVD
   "LUN Already Enabled", // STR_LUN_ALREADY_AVAIL
   "Unknown Error Code", // STR_UNKNOWN_ERR_CODE
   "(Invalid HBA time)  ", // STR_INVALID_HBA_TIME
   "Unspecified", // STR_UNSPECIFIED
   "Enclosure Services Controller Electronics", // STR_ENCL_SERV_CTLR_ELEC
   "SCC Controller Electronics", // STR_SCC_CTLR_ELEC
   "Nonvolatile Cache", // STR_NONVOLATILE_CACHE
   "Uninterruptible", // STR_UNINTERRUPTABLE
   "Display", // STR_DISPLAY
   "Key Pad Entry Device", // STR_KEY_PAD_ENTRY_DEV
   "SCSI Port/Transceiver", // STR_SCSI_PORT_XCEIVER
   "Language", // STR_LANGUAGE
   "Communication Port", // STR_COMM_PORT
   "SCSI Target Port", // STR_SCSI_TGT_PORT
   "SCSI Initiator Port", // STR_SCSI_INIT_PORT
   "Simple Sub-enclosure", // STR_SIMPLE_SUBENCL
   "Critical", // STR_CRITICAL
   "Noncritical", // STR_NONCRITICAL
   "Unrecoverable", // STR_UNRECOVERABLE
   "Not Installed", // STR_NOT_INSTALLED
   "Unavailable", // STR_UNAVAILABLE
   "main", // STR_MAIN
   "sub", // STR_SUB
   "Parent Array Building ", // STR_PARENT_ARRAY_BUILDING
   "Parent Build", // STR_PARENT_BUILDING
	"Parent Expand", // STR_PARENT_EXPANDING
   "No Info Avail", // STR_NO_INFO_AVAIL
   "\n\nYou are attempting to mirror a larger drive onto a smaller drive - \n", // STR_MIRROR_MSG1
   "this will result in a loss of mirrored data.  If you want to do this rerun\n", // STR_MIRROR_MSG2
   "the command and precede it with the '-i' option.  Otherwise rerun the\n", // STR_MIRROR_MSG3
   "command, but reverse the order of the drives so that the smaller drive is\n", // STR_MIRROR_MSG4
   "copied to the larger drive.", // STR_MIRROR_MSG5
   "Configuration file loaded.\n", // STR_CFG_FILE_LOADED
   "Configuration file saved.\n", // STR_CFG_FILE_SAVED
   "Engine connect failed", // STR_ENG_CONN_FAILED
   ": Open\n", // STR_COLON_OPEN
   ": COMPATIBILITY number\n", // STR_COLON_COMPAT_NBR
   ": SCAN failed\n", // STR_COLON_SCAN_FAILED
   "HBA", // STR_HBA
   "Disk Drive (DASD)", // STR_SCSI_DASD
   "Tape Drive", // STR_SCSI_SASD
   "No items of that type found", // STR_NO_ITEMS_FOUND
   "No hotspare is available", // STR_NO_HOTSPARE_AVAIL
   "Rebuild drive too small", // STR_RBLD_DRV_TOO_SMALL
   "ATA33", // STR_ATA33
   "ATA45", // STR_ATA45
   "ATA66", // STR_ATA66
   "ATA100", // STR_ATA100
   "ATA133", // STR_ATA133
   "ATA", // STR_ATA
   "SATA1500", // STR_SATA1500
   "SATA3000", // STR_SATA3000
   "SATA6000", // STR_SATA6000
   "SATA", // STR_SATA
   "Remove Mode 0 jumper before reboot.\n", // STR_REMMODEZERO
   "Device reserve conflict detected. Command failed.\n", //STR_RESCONFLICT
   "Cluster bit is set, feature disabled.\n", // STR_CLSTR_FEAT_DISABLE
   "Total Raid size exceeds 2TB raid creation failed.", // STR_ERR_TWO_TB_RAID
   "Invalid segment offset.\n", //STR_ERR_INVALID_SEGMENT_OFFSET
   "Invalid segment size.\n", //STR_ERR_INVALID_SEGMENT_SIZE
   "JBOD", //STR_RAID_JBOD
   "No segments configured",//STR_NO_SEGMENT_CONFIG
   "~#  ~b0 ~b1 ~b2  ~Cont~roller     ~       ~FW    ~NVRAM     ~Serial     ~Status\n", // STR_LIST_DEV_CTLR_HEADER_NO_CACHE
   "~#  ~Cont~roller     ~       ~FW    ~NVRAM     ~BIOS   ~SMOR      ~Serial\n", // STR_LIST_DEV_VERSION_HEADER_NO_CACHE
   "Array name too long (must be 16 characters or less) ", // STR_ERR_NAME_TOO_LONG
   "No array name specified ", //STR_ERR_NO_ARRAY_NAME
   "Input name is already used by another device", //STR_ERR_NAME_ALREADY_USED
   "Array name successfully changed", //STR_ARRAY_NAME_CHANGED
   "~Address        ~Array Name        ~Manufactu~rer/Model      ~Capacity  ~Status\n", // STR_LIST_ARRAYNAME_HEADER
};

#endif	// __RSCSTRS_H

