// ===========================================================================
//	CPCRStat.cpp				(c)1996-98 Paul Goracke. All rights reserved.
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

// 17apr98  change kTinyNumber to be FLT_EPSILON from <cfloat> instead of
//          some arbitrary number.
//

#include <math.h>
#include <cfloat>

#include "CPCRStat.h"

const double kTinyNumber = FLT_EPSILON;

CPCRStat::CPCRStat( void )
	: mDilution( 0.0 ),
		mNumPCRs( 0 ),
		mNumPositives( 0 ),
		mNumNegatives( 0 )
{
	InitObject(
		0.0,
		0,
		0 );
}


CPCRStat::CPCRStat(
	double	inDilution,
	long	inNumPCRs,
	long	inNumPositives )
	: mDilution( 0.0 ),
		mNumPCRs( 0 ),
		mNumPositives( 0 ),
		mNumNegatives( 0 )
{
	InitObject(
		inDilution,
		inNumPCRs,
		inNumPositives );
}


void
CPCRStat::InitObject(
	double	inDilution,
	long	inNumPCRs,
	long	inNumPositives )
{
	SetDilution( inDilution );
	SetPCRs( inNumPCRs );
	SetPositives( inNumPositives );
}


void
CPCRStat::CalcNumNegatives( void )
{
	mNumNegatives = mNumPCRs - mNumPositives;
}


double
CPCRStat::GetDilution( void )
{
	return mDilution;
}

void
CPCRStat::SetDilution( double inDilution )
{
	mDilution = inDilution;
}


long
CPCRStat::GetPCRs( void )
{
	return mNumPCRs;
}

void
CPCRStat::SetPCRs( long inNumPCRs )
{
	mNumPCRs = inNumPCRs;
}


long
CPCRStat::GetPositives( void )
{
	return mNumPositives;
}

void
CPCRStat::SetPositives( long inNumPositives )
{
	mNumPositives = inNumPositives;
	CalcNumNegatives();
}


long
CPCRStat::GetNegatives( void )
{
	return mNumNegatives;
}


bool
CPCRStat::IsValid( void )
{
	if ( mDilution != 0.0 )
		if ( mNumPCRs > 0 )
			return true;
	
	return false;
}


#pragma mark ===== Chi Squared calculations =====

double
CPCRStat::ChiSquared(
	double	inNumCopies,
	double	inProbFalsePos,
	double	inProbFalseNeg,
	long	inNumPositivesOffset )
{
//
//         [   ( ( #pcrs * probOfNeg ) - #negs ) ^ 2   ]
//  f(c) = [ ----------------------------------------- ]
//         [   #pcrs * probOfNeg * ( 1 - probOfNeg )   ]
//

	double probNeg = 0.0;
	probNeg = ProbOfNegative( inNumCopies, inProbFalsePos, inProbFalseNeg );
	
	double result = 0.0;
	// ( ( #pcrs * probOfNeg ) - #negs )
	result = mNumPCRs * probNeg - ( mNumNegatives - inNumPositivesOffset );
	
	// ( #pcrs * probOfNeg - #negs ) ^ 2
	result *= result;
	
	// [   ( #pcrs * probOfNeg - #negs ) ^ 2   ]
	// [ ------------------------------------- ]
	// [            ( 1 - probOfNeg )          ]
	result /= 1 - probNeg;
	
	// [   ( #pcrs * probOfNeg - #negs ) ^ 2   ]
	// [ ------------------------------------- ]
	// [ #pcrs * probOfNeg * ( 1 - probOfNeg ) ]
	result /= mNumPCRs * probNeg;
	
	return result;
}


#pragma mark ===== Probability of Negative =====

double
CPCRStat::ProbOfNegative(
	double	inNumCopies,
	double	inProbFalsePos,
	double	inProbFalseNeg )
{
	double prob = 0.0;
//
// probOfNeg = (exp(-dilution*numCopies))*(1-probFalsePos)
//							+ (1-exp(-dilution*numCopies))*probFalseNeg
//
//    reduces to
//
// probOfNeg = (exp(-dilution*numCopies))*(1-probFalsePos-probFalseNeg)
//						+ probFalseNeg
//

	// exp(-dilution*numCopies)
	prob = exp( -mDilution * inNumCopies );
	
	// exp(-dilution*numCopies)) * (1-probFalsePos-probFalseNeg
	prob *= ( 1 - ( inProbFalsePos + inProbFalseNeg ) );
	
	// (exp(-dilution*numCopies))*(1-probFalsePos-probFalseNeg) + probFalseNeg
	prob += inProbFalseNeg;
	
	// if prob = 0 or 1 exactly, the ChiSquared equation will blow up.
	// so we move it in a tiny bit.
	if ( prob <= 0.0 )
		prob = kTinyNumber;
	
	if ( prob >= 1.0 )
		prob = 1.0 - kTinyNumber;
	
	return prob;
}
