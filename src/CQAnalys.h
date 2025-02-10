// ===========================================================================
//	CQAnalys.h				(c)1996-7 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	A class to manage a list of CPCRStat objects. Calculates Chi Squared Sum by
//	going through the list and asking each CPCRStat object its Chi Squared value.
//
//	Reads the data in when passed a pointer to a CFile object in ReadFile()
//
//	Displays the results to standard output via DisplayResults()
// 

// 17apr98  removed FillObjectList(), GetProbFalsePos(), and GetProbFalseNeg()
//

#ifndef _H_CQualityAnalysis
#define _H_CQualityAnalysis

const long kMaxNameLength = 50;

class CPCRStat;
class CFile;

class CQualityAnalysis
{
	public:
				CQualityAnalysis();
				CQualityAnalysis( CFile * inFile );
		virtual	~CQualityAnalysis();
		
		void	InitObject( void );
		void	SetDerivH( void );
		void	Run( void );
		
		double	GetInitialGuess(
					long inWhichItemToTweak,
					long inHowMuchToTweak,
					bool useFudgeFactor = false );
		void	DisplayResults( void );
		void	OutputChi2DataPoints( void );
		
		double	FindCopyNumToMinChiSquared(
					long	inWhichItemToTweak = -1,
					long	inHowMuchToTweak = 0,
					double	inAcceptableError = 0.0001,
					unsigned long	inMaxIterations = 5000 );
		
		double	ChiSquaredSum(
						double inCopyNum,
						long	inWhichItemToTweak = -1,
						long	inHowMuchToTweak = 0 );
		double	D_ChiSquaredSum(
						double inCopyNum,
						long	inWhichItemToTweak = -1,
						long	inHowMuchToTweak = 0 );
		double	D2_ChiSquaredSum(
						double inCopyNum,
						long	inWhichItemToTweak = -1,
						long	inHowMuchToTweak = 0 );
		
		void	ReadFile( CFile * inFile );
		void	ReadDilutions( const char *inBuffer );
		void	ReadNumPCRs( const char *inBuffer );
		void	ReadNumPosReactions( const char *inBuffer );
		void	ReadFalseProbs( const char *inBuffer );
		
	private:
		char mName[ kMaxNameLength ];
		CPCRStat ** mList;
		long	mNumItems;
		double	mExpLowLimit;
		double	mProbFalsePos;
		double	mProbFalseNeg;
		double	mSolvedNumCopies;
		double	mChi2atSolvedCopies;
		double	mDerivH;
		bool	mAnErrorOccurred;
		
		bool	DataIsValid( void );
		void	AnErrorOccurred( void );
		
		void	SortList( void );
		CPCRStat *	FindLastPositivePCR(
							long inWhichItemToTweak,
							long inHowMuchToTweak = 0 );
};
#endif