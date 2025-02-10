// ===========================================================================
//	CFile.cpp				(c)1996-99 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	An object to handle file input and user choice of input file. This may
//	seem a bit over-object-oriented, but it allows me to substitute a different
//	source file on the Mac, which uses the standard Mac Open File dialog,
//	instead of typing in the file name.
// 

// 07jan99  removed mMacFileRefNum(0) from ctor member init list since it
//          is now part of the Mac-specific subclass CMacFile.
//
// 03aug98  added mMacFileRefNum(0) to ctor member init list.
//          added mFileName[0]='\0' to ctor.
//
// 04aug98	changed file name input in CFile::ChooseFile. It was using a
//          while((c=cin.get())!='\n') loop. Due to MW's PC implementation
//          the "press Enter to continue" wasn't flushing from the buffer,
//          therefore you'd only be able to run the program once in a row.
//          
//          fileName changed from char[] to std::string
//          
//          changed strcpy() in SetFile to strncpy() to prevent buffer overflow.
//
//          in OpenFile, changed check for invalid file from
//          if(mFileStream!= NULL) to if(mFileStream->is_open()) since the
//          first version is just plain wrong for iostreams--it will never be NULL.
//

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

#include "CFile.h"

#include "defines.h"
#include <cstring>

CFile::CFile()
	: mFileStream( NULL )
{
	mFileName[ 0 ] = '\0';
}

CFile::~CFile()
{
	delete mFileStream;
}


bool CFile::ChooseFile( void )
{
	bool success = false;
	string fileName;
	
	cout << "Enter file name: ";
	getline( cin, fileName );
	
	success = SetFile( fileName.c_str() );

	return success;
}


bool CFile::SetFile( const char *inFileName )
{
	bool success = false;
	
	strncpy( mFileName, inFileName, kMaxFileNameChars );
	
	// make sure the filename is null-terminated
	mFileName[ kMaxFileNameChars ] = '\0';
	
	success = OpenFile();
	
	return success;
}


bool CFile::OpenFile( void )
{
	bool success = false;
	
	if ( mFileName[ 0 ] != '\0' )
	{
		mFileStream = new ifstream( mFileName );
		if( mFileStream->is_open() )
			success = true;
	}
	
	return success;
}


bool CFile::CloseFile( void )
{
	delete mFileStream;
	mFileStream = NULL;
	
	return true;
}


bool CFile::GetLine( char * inLineBuffer, short inBuffSize )
{
	bool success = true;
	
	// getline automatically uses newline
	mFileStream->getline( inLineBuffer, inBuffSize );
	
	return success;
}

bool CFile::End( void )
{
	char c;
	
	c = mFileStream->get();
	bool endOfFile = ! mFileStream->good();
	mFileStream->putback( c );
	
	return endOfFile;
}


bool CFile::NextLineIsNewAnalysis( void )
{
	char c;
	
	c = mFileStream->get();
	mFileStream->putback( c );
	
	if ( c == '#' )
		return true;
	
	return false;
}

bool CFile::PositionToNextAnalysis( void )
{
	const short buffSize = 100;
	char buffer[ buffSize ];
	
	while ( ! End() )
	{
		if ( NextLineIsNewAnalysis() )
			return true;
		else
			GetLine( buffer, buffSize );
	}
	
	return false;
}
