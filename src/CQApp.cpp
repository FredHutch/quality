// ===========================================================================
//	CQApp.cpp				(c)1996-99 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	A fairly simple Application object to create a new CQualityAnalysis
//	object, open an input file (prompting the user for one if necessary), then
//	Run() the Analysis. Loops through all analyses in the file, reusing the
//	same CQualityAnalysis object, so we can have unlimited analyses in a file.
// 

// 07jan99  added #include "defines.h" to get __dest_os definition
//
//          changed mFile ctor member initialization to use typedef
//          FileT. Combined with __dest_os, this allows easy creation
//          of CFile object appropriate to the platform. Polymorphism
//          takes care of the proper behavior.
// 
// 17apr98  removed unused var declaration 'i' from ReadFile()
//

#include <iostream>

#include "CQApp.h"
#include "CQAnalys.h"

#include "defines.h"

#if ( __dest_os == __mac_os )
	#include "CMacFile.h"
	typedef CMacFile	FileT;
#else
	#include "CFile.h"
	typedef CFile	FileT;
#endif

const long kNumAnalyses = 50;

CQualityApp::CQualityApp( void )
	: mFile( new FileT ),
		mAnalysis( new CQualityAnalysis ),
		mNumAnalyses( 0 ),
		mExpLowLimit( 0.0 ),
		mTooManyAnalyses( false )
{
}

CQualityApp::~CQualityApp()
{
	delete mFile;
	delete mAnalysis;
}

bool
CQualityApp::Run( void )
{
	// run the program, no input file specified yet.
	
	bool success = false;
	
	if ( mFile->ChooseFile() )
	{
		ReadFile();
		success = true;
	}
	else
	{
		cout << "Could not open file \"" << mFile->mFileName << "\"" << endl;
	}
	
	return success;
}


bool
CQualityApp::Run( const char *inFileName )
{
	// run the program, input file specified by name in inFileName
	
	bool success = false;
	
	if ( mFile->SetFile( inFileName ) )
	{
		ReadFile();
		success = true;
	}
	else
	{
		cout << "Could not open file \"" << inFileName << "\"" << endl;
	}
	
	return success;
}


#pragma mark ==== File routines ====

void
CQualityApp::ReadFile( const char * inFileName )
{
	if ( mFile->SetFile( inFileName ) )
		ReadFile();
}


void
CQualityApp::ReadFile( void )
{
	while ( ! mFile->End() )
	{
		mAnalysis->ReadFile( mFile );
		mFile->PositionToNextAnalysis();
		mAnalysis->Run();
	}
	
	mNumAnalyses = 1;
	
	mFile->CloseFile();	
}
