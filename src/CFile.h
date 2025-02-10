// ===========================================================================
//	CFile.h				(c)1996-99 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	An object to handle file input and user choice of input file. This may
//	seem a bit over-object-oriented, but it allows me to substitute a different
//	source file on the Mac, which uses the standard Mac Open File dialog,
//	instead of typing in the file name.
// 

// 07jan99  removed member mMacFileRefNum, since it is now part of the
//          Mac-specific subclass CMacFile. Made all member functions
//          virtual for this reason.
//
//          removed 'short mMacFileRefNum' since it is part of CMacFile
//
// 04aug98	defined kMaxFileNameChars, rather than using a magic number.

#ifndef _H_CFile
#define _H_CFile

#define kCR			0x0D
#define kLF			0x0A
#define kTab		0x09
#define kNull		0x00

#include <fstream>
using namespace std;

const int kMaxFileNameChars = 64;

class CFile
{
	public:
		CFile();
		virtual ~CFile();
		
		virtual bool	ChooseFile( void );
		virtual bool	SetFile( const char *inFileName );
		virtual bool	OpenFile( void );
		virtual bool	CloseFile( void );
		virtual bool	GetLine( char * inLineBuffer, short inBuffSize );
		virtual bool	End( void );
		virtual bool	NextLineIsNewAnalysis( void );
		virtual bool	PositionToNextAnalysis( void );
		
		char mFileName[ kMaxFileNameChars + 1 ];
		ifstream * mFileStream;
};

#endif
