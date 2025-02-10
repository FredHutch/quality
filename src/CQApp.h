// ===========================================================================
//	CQApp.h				(c)1996-7 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	A fairly simple Application object to create a new CQualityAnalysis
//	object, open an input file (prompting the user for one if necessary), then
//	Run() the Analysis. Loops through all analyses in the file, reusing the
//	same CQualityAnalysis object, so we can have unlimited analyses in a file.
// 


#ifndef _H_CQualityApp
#define _H_CQualityApp

class CQualityAnalysis;
class CFile;

class CQualityApp
{
	public:		
				CQualityApp();
		virtual ~CQualityApp();
		
		bool	Run( void );
		bool	Run( const char *inFileName );
		
		
		void	ReadFile( const char *inFileName );
		void	ReadFile( void );
		
	private:
		CFile * mFile;
		CQualityAnalysis * mAnalysis;
		long	mNumAnalyses;
		double	mExpLowLimit;
		bool	mTooManyAnalyses;
};

#endif