/**
 * File types for SdFat, File, SdFile, SdBaseFile, fstream,
 * ifstream, and ofstream.
 *
 * Set SD_FILE_TYPE to:
 *
 * 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
 */

#include "SdFat.h"
#include "userconfig.h"

#ifndef SD_INCLUDE_EXFAT_SUPPORT
  #if defined(__AVR__) && FLASHEND < 0X8000
  // FAT16/FAT32 only, for low-memory boards
  #define SD_INCLUDE_EXFAT_SUPPORT 0
  #else  // defined(__AVR__) && FLASHEND < 0X8000
  #define SD_INCLUDE_EXFAT_SUPPORT 1
  #endif  // defined(__AVR__) && FLASHEND < 0X8000
#endif

#ifndef SD_FAT_TYPE
  #if SD_INCLUDE_EXFAT_SUPPORT == 0
  #define SD_FAT_TYPE 1
  #else
  #define SD_FAT_TYPE 3
  #endif
#endif

#if SD_FAT_TYPE == 0
typedef SdFat SDTYPE;
typedef File FILETYPE;
#elif SD_FAT_TYPE == 1
typedef SdFat32 SDTYPE;
typedef File32 FILETYPE;
#elif SD_FAT_TYPE == 2
typedef SdExFat SDTYPE;
typedef ExFile FILETYPE;
#elif SD_FAT_TYPE == 3
typedef SdFs SDTYPE;
typedef FsFile FILETYPE;
#else  // SD_FAT_TYPE
#error invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE
