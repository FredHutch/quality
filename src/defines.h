// ===========================================================================
//	defines.h for QUALITY				(c)1996-99 Paul Goracke. All rights reserved.
// ===========================================================================
//

// 07jan99  definition of __dest_os moved here from main.cpp

#define kVersString	"1.1.4"
#define kVersMajor	0x01
#define kVersMinor	0x14

#define kVersDate		"07jan99"

#ifndef __undef_os
#define __undef_os		0
#define __mac_os		1
#define __win32_os		3
#endif

// define the character used as a command line switch/option flag
#ifdef __dest_os
 #if ( __dest_os != __mac_os )
  #if ( __dest_os == __win32_os )
   #define kCmdLineOptChar '/'
  #endif /* __dest_os == __win32_os */
 #else
 	// __dest_os == __mac_os
 	// need <unix.h> to set file type and creator
 	#include <unix.h>
 #endif /* __dest_os != __mac_os */
#else
 #ifdef COMPILE_DOS // defined as DOS in Makefile
  #define kCmdLineOptChar '/'
 #else
  // __dest_os undefined--assume it's unix
  #define __dest_os	__undef_os
  #define kCmdLineOptChar '-'
 #endif
#endif /* __dest_os undefined */
