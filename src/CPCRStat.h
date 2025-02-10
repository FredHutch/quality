// ===========================================================================
//	CPCRStat.h				(c)1996 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	Implements a class to maintain data for one set of PCRs: dilution,
//	number of PCRs at that dilution, and number of those PCRs that were
//	positive. Automatically calculates the number of negative PCRs that
//	implies.
//
//	Also calculates the Chi Squared value for this dilution. So, this is
//	where you should change the defining equation if so desired.
// 


#ifndef _H_CPCRStat
#define _H_CPCRStat

class CPCRStat
{
	public:
	
						CPCRStat();
							CPCRStat(
								double	inDilution,
								long	inNumPCRs = 0,
								long	inNumPositives = 0 );
		void		CalcNumNegatives( void );
		
		double	GetDilution( void );
		void		SetDilution( double inDilution );
		
		long		GetPCRs( void );
		void		SetPCRs( long inNumPCRs );
		
		long		GetPositives( void );
		void		SetPositives( long inNumPositives );
		
		long		GetNegatives( void );
		
		bool		IsValid( void );
		
		double	ChiSquared(
					double inNumCopies,
					double inProbFalsePos,
					double inProbFalseNeg,
					long inNumPositivesOffset = 0 );
		
		double	ProbOfNegative(
					double	inNumCopies,
					double	inProbFalsePos,
					double	inProbFalseNeg );
		
	protected:
		
		double	mDilution;
		long		mNumPCRs;
		long		mNumPositives;
		long		mNumNegatives;
		
		void	InitObject(
					double	inDilution,
					long		inNumPCRs,
					long		inNumPositives );
		
};

#endif