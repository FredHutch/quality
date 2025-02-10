// ===========================================================================
//	main.cpp for QUALITY				(c)1996-99 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	
// 

// 07jan99  moved definition of __dest_os to defines.h to allow other project
//          files to use the value
//
// 17apr98  changed return type of main() from 'void' to 'int'
//
// 17apr98  moved version info #defines to "defines.h" which allows
//          auto-incorporation into Mac resources
//
// 17apr98  #if'd declaration of main so Mac version doesn't have
//          unused vars argc, argv
//
// 04aug98	changed variable 'theResult' to 'success' and now return
//          it's value (cast as 'int') at end of main.
//

#include <stdio.h>
#include <iostream>
using namespace std;
#include "defines.h"

#include "CQApp.h"

#if ( __dest_os != __mac_os )
int main( int argc, char *argv[] )
#else
int main( int, char )
#endif
{
	char *	cmdLineFileName = NULL;
	bool	success = false;
	bool	fileNameGiven = false;
	bool	runProgram = true;
	
#if ( __dest_os != __mac_os )
	for ( int i = 1; i < argc; i++ )
	{
		if ( argv[ i ][ 0 ] == kCmdLineOptChar )
		{
			switch ( (char)(argv[ i ][ 1 ]) )
			{
				case 'v':
					cout << "QUALITY, version " << kVersString;
					cout << ", " << kVersDate << endl;
					cout << "written by Paul Goracke" << endl;
					runProgram = false;
					break;
				
				default:
					cout << "Unrecognized option: ";
					cout << argv[ i ][ 1 ] << endl;
					runProgram = false;
					break;
			}
		}
		else
		{
			if ( ! fileNameGiven ) // only take the first filename passed
			{
				cmdLineFileName = argv[ i ];
				fileNameGiven = true;
			}
		}
	}
#else
	/* Initialize all the needed Mac managers. */
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
	
	_ftype = 'TEXT';
	_fcreator = 'R*ch';
	
#endif /* __dest_os != __mac_os */
	
	if ( runProgram )
	{
		CQualityApp * theApp;
		theApp = new CQualityApp;
		
		if ( theApp != NULL )
			if ( fileNameGiven )
				success = theApp->Run( cmdLineFileName );
			else
				success = theApp->Run();
		
		delete theApp;
	}

#if ( __dest_os == __mac_os )
	// so we can tell when the Mac version is finished, since a
	// command line prompt will not appear, like in DOS or Unix
	cout << endl << "Program done." << endl;
	
#endif /* __dest_os == __mac_os */
	
	return (int)success;
}
