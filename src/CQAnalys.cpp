// ===========================================================================
//	CQualityAnalysis.cpp				(c)1996-7 Paul Goracke. All rights reserved.
// ===========================================================================
//
//	A class to manage a list of CPCRStat objects. Calculates Chi Squared Sum by
//	going through the list and asking each CPCRStat object its Chi Squared value.
//
//	Reads the data in when passed a pointer to a CFile object in ReadFile()
//
//	Displays the results to standard output via DisplayResults()
// 

// 10mar98  added mNumItems(0) member initialization. random init values
//          could (did) cause crashes in InitObject()
//
// 17apr98  removed FillObjectList(), GetProbFalsPos(), and GetProbFalseNeg()
//

#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>

#ifndef __OLD_STRING_STREAMS__
 #ifndef __dest_os // known OS
  #define __OLD_STRING_STREAMS__	0 // do not use old <strstream.h>
 #else
  #if ( ( __dest_os == __mac_os ) || ( __dest_os == __win32_os ) )
   #define __OLD_STRING_STREAMS__	0
  #else
   #define __OLD_STRING_STREAMS__	0
  #endif
 #endif
#endif

#if __OLD_STRING_STREAMS__
	#include <strstream>
#else
	#include <sstream>
#endif


const double kDerivH = 0.0001;

#ifndef NRUSED
	#define NRUSED 0
#endif

#if NRUSED
	#define NRANSI
	#include "NR.H"
#endif

#include "CQAnalys.h"
#include "CPCRStat.h"
#include "CFile.h"
#include <cstring>

const long kMaxNumDilutions = 50;

CQualityAnalysis::CQualityAnalysis( void )
	: mList( NULL ), mNumItems( 0 )
{
	InitObject();
}


CQualityAnalysis::CQualityAnalysis( CFile * inFile )
	: mList( NULL ), mNumItems( 0 )
{
	InitObject();
	ReadFile( inFile );
}

CQualityAnalysis::~CQualityAnalysis()
{
	for ( long i = 0; i < mNumItems; i++ )
		delete mList[ i ];
	delete [] mList;
}


void
CQualityAnalysis::InitObject( void )
{
	mAnErrorOccurred = false;
	
	mName[ 0 ] = '\0';
	
	for ( long i = 0; i < mNumItems; i++ )
		delete mList[ i ];
	delete [] mList;
	mList = new CPCRStat *[ kMaxNumDilutions ];
	
	mNumItems = 0;
	
	mExpLowLimit = 0.0;
	
	mProbFalsePos = 0.0;
	mProbFalseNeg = 0.0;
	
	mSolvedNumCopies = 0.0;
	mChi2atSolvedCopies = 0.0;
	
	SetDerivH();
}

void
CQualityAnalysis::SetDerivH( void )
{
	// this is done as a completely separate step to enable
	// setting this value via some other calculation, if so desired.
	
	mDerivH = kDerivH;
}


void
CQualityAnalysis::Run( void )
{
	if ( mName[ 0 ] != '\0' )
		// sample/analysis name
		cout << "Results for \"" << mName << "\"..." << endl;
	else
		// spacer if no name
		cout << endl << "-------------------" << endl;
	
	if ( DataIsValid() )
	{
		mSolvedNumCopies = FindCopyNumToMinChiSquared();
		if ( mSolvedNumCopies > 0.0 )
			mChi2atSolvedCopies = ChiSquaredSum( mSolvedNumCopies );
		
		DisplayResults();
	}
		
	cout << endl << endl;
}


bool
CQualityAnalysis::DataIsValid( void )
{
	if ( mProbFalsePos >= 1.0 )
	{
		AnErrorOccurred();
		cout
			<< "ERR: Prob of false positive [="
			<< mProbFalsePos
			<< "] >= 1"
			<< endl;
	}
	
	if ( mProbFalsePos < 0 )
	{
		AnErrorOccurred();
		cout
			<< "ERR: Prob of false positive [="
			<< mProbFalsePos
			<< "] < 0"
			<<
			endl;
	}
	
	if ( mProbFalseNeg >= 1.0 )
	{
		AnErrorOccurred();
		cout
			<< "ERR: Prob of false negative [="
			<< mProbFalseNeg
			<< "] >= 1"
			<< endl;
	}
	
	if ( mProbFalseNeg < 0 )
	{
		AnErrorOccurred();
		cout
			<< "ERR: Prob of false negative [="
			<< mProbFalseNeg
			<< "] < 0"
			<< endl;
	}
	
	for ( int i = 0; i < mNumItems; i++ )
	{
		if ( mList[ i ]->GetPositives() > mList[ i ]->GetPCRs() )
		{
			AnErrorOccurred();
			cout << "ERR: Dilution " << mList[ i ]->GetDilution();
			cout << " has #Pos [=" << mList[ i ]->GetPositives();
			cout << "] > #PCRs [=" << mList[ i ]->GetPCRs() << "]" << endl;
		}
	}
	
	return ( ! mAnErrorOccurred );
}


void
CQualityAnalysis::AnErrorOccurred( void )
{
	if ( mAnErrorOccurred == false )
	{
		mAnErrorOccurred = true;
		cout << endl;
	}
}


double
CQualityAnalysis::GetInitialGuess(
	long inWhichItemToTweak,
	long inHowMuchToTweak,
	bool useFudgeFactor )
{
	double numCopies = -1.0;
	
	SortList();
	CPCRStat * theItem =
		FindLastPositivePCR( inWhichItemToTweak, inHowMuchToTweak );
	
	if ( theItem == NULL )
		numCopies = -5.0;
	else
	{
		long tweakedPositives = theItem->GetPositives();
		
		if ( inWhichItemToTweak >= 0 )
			if ( theItem == mList[ inWhichItemToTweak ] )
				tweakedPositives += inHowMuchToTweak;
		
		numCopies = ( tweakedPositives / theItem->GetDilution() )
							/ theItem->GetPCRs();
		if ( useFudgeFactor )
		{
			double fudgeFactor = 1.0 / 10.0;
			numCopies *= fudgeFactor;
		}
	}
	
	return numCopies;
}


void
CQualityAnalysis::SortList( void )
{
	// In case someone inputs data that's not in decreasing dilution order
	// Otherwise, it will be difficult to figure the last positive PCR
	
	bool cleanRun = false;
	CPCRStat * tempItem;
	
	while ( ! cleanRun )
	{
		cleanRun = true;
		for ( long i = 1; i < mNumItems; i++ )
		{
			if ( mList[ i - 1 ]->GetDilution() < mList[ i ]->GetDilution() )
			{
				tempItem = mList[ i - 1 ];
				mList[ i - 1 ] = mList[ i ];
				mList[ i ] = tempItem;
				cleanRun = false;
			}
		}
	}
}


CPCRStat *
CQualityAnalysis::FindLastPositivePCR(
	long inWhichItemToTweak,
	long inHowMuchToTweak )
{
	long tweakedPositives;
	CPCRStat *theItem;
	short lowItemIndex = -1;
	double lowDilution = 0.0;
	
	for ( long i = mNumItems - 1; i >= 0; i-- )
	{
		theItem = mList[ i ];
		
		tweakedPositives = mList[ i ]->GetPositives();
		
		if ( i == inWhichItemToTweak )
			tweakedPositives += inHowMuchToTweak;
		
		if ( tweakedPositives > 0 )
			switch ( lowItemIndex )
			{
				case -1:
					lowItemIndex = i;
					lowDilution = theItem->GetDilution();
					break;
				
				default:
					if ( theItem->GetDilution() < lowDilution )
					{
						lowItemIndex = i;
						lowDilution = theItem->GetDilution();
					}
			}
	}
	
	if ( ( lowItemIndex >= 0 ) && ( lowItemIndex < mNumItems ) )
		return mList[ lowItemIndex ];
	
	return NULL;
}


double
CQualityAnalysis::FindCopyNumToMinChiSquared(
	long	inWhichItemToTweak,
	long	inHowMuchToTweak,
	double	inAcceptableError,
	unsigned long	inMaxIterations )
{
	// Okay--time to solve the equation
	
	double expLowLimit = 0.00001 + 0.0001;
	bool secondTryFlag = false;
	
	if ( inWhichItemToTweak >= 0 )
	{
		CPCRStat * theItem = mList[ inWhichItemToTweak ];
		long theNewCount = theItem->GetPositives() + inHowMuchToTweak;
		if ( ( theNewCount < 0 ) || ( theNewCount > theItem->GetPCRs() ) )
		{
			inWhichItemToTweak = -1;
			inHowMuchToTweak = 0;
		}
	}
	
	
	double currentValue = GetInitialGuess(
									inWhichItemToTweak,
									inHowMuchToTweak );
	
	if ( currentValue < expLowLimit ) // no last positive found--can't solve
		if ( secondTryFlag )
			return 0.0;	// strike two--we're outta here.
		else
			if ( ( currentValue = GetInitialGuess(
											inWhichItemToTweak,
											inHowMuchToTweak,
											true ) )
						< expLowLimit )
				return 0.0;
	
	double nextValue = 0.0;
	double slope;
	unsigned long iteration = 0;
	
	while ( iteration < inMaxIterations )
	{
		if ( ( slope = D2_ChiSquaredSum(
								currentValue,
								inWhichItemToTweak,
								inHowMuchToTweak ) )
					== 0.0 )
			if ( secondTryFlag )
				return 0.0;	// strike two--we're outta here.
			else
			{
				currentValue = GetInitialGuess(
										inWhichItemToTweak,
										inHowMuchToTweak,
										true );
				slope = D2_ChiSquaredSum(
								currentValue,
								inWhichItemToTweak,
								inHowMuchToTweak );
				if ( slope == 0.0 )
					return 0.0;

				iteration = 0;
				secondTryFlag = true;
			}
		
		double functionValue = D_ChiSquaredSum(
											currentValue,
											inWhichItemToTweak,
											inHowMuchToTweak );
		double countShift = functionValue / slope;
		nextValue = currentValue - countShift;
		if ( nextValue < expLowLimit )
			nextValue = expLowLimit;
		
		if ( ( fabs( countShift ) < inAcceptableError )
			&& ( fabs( functionValue ) < inAcceptableError ) )
		{
			if ( slope >= 0 )
				return nextValue;
			else
			{
				if ( secondTryFlag == false )
				{
					// found a maximum
					// -- redo using a fudge factor for starting guess
					nextValue = GetInitialGuess(
						inWhichItemToTweak,
						inHowMuchToTweak,true );
					iteration = 0;
					secondTryFlag = true;
				}
				else
					return 0.0;
			}
		}
		
		currentValue = nextValue;
		iteration++;
	}
	
	return 0.0;
}


void
CQualityAnalysis::DisplayResults()
{
	// buffer output to a string stream first, then output
	// to stdout
#if __OLD_STRING_STREAMS__
	ostrstream out;
#else
	ostringstream out;
#endif

	enum columns {
		kDilution,
		kNumPCRs,
		kNumPos,
		kPosExp,
		kPlusOne,
		kMinusOne };
	
	const long colWidths[ 6 ] = { 10, 8, 8, 10, 10, 10 };
	
	out.setf( ios::fixed, ios::floatfield );
	out.precision( 2 );
	
	if ( mSolvedNumCopies == 0.0 )
		out << "No solution found.\n\n";
	else
	{
		if ( mProbFalsePos != 0.0 )
		{
			// probability of false positive
			out.width( 25 );
			out.setf( ios::right, ios::adjustfield );
			out << "False positive prob:";
			out.width( 10 );
			out.setf( ios::right, ios::adjustfield );
			out << mProbFalsePos << endl;
		}
		
		if ( mProbFalseNeg != 0.0 )
		{
			// probability of false negative
			out.width( 25 );
			out.setf( ios::right, ios::adjustfield );
			out << "False negative prob:";
			out.width( 10 );
			out.setf( ios::right, ios::adjustfield );
			out << mProbFalseNeg << endl;
		}
		
		out << endl;
		
		// # of copies per unit
		out.width( 25 );
		out.setf( ios::right, ios::adjustfield );
		out << "# of copies per unit: ";
		out.width( 10 );
		out.setf( ios::right, ios::adjustfield );
		out.setf( ios::showpoint );
		out << mSolvedNumCopies << endl;
		
		// Standard Error
		out.width( 25 );
		out.setf( ios::right, ios::adjustfield );
		out << "Standard Error: ";
		out.width( 10 );
		out.setf( ios::right, ios::adjustfield );
		double stdErr = sqrt( 2.0 / D2_ChiSquaredSum( mSolvedNumCopies ) );
		out << stdErr << endl;
		
		out << endl;
		
		// Chi-squared goodness of fit
		out.width( 25 );
		out.setf( ios::right, ios::adjustfield );
		out << "Chi^2 goodness of fit: ";
		out.width( 10 );
		out.setf( ios::right, ios::adjustfield );
		out.precision( 4 );
		out << mChi2atSolvedCopies << endl;
		
		out.width( 25 );
		out.setf( ios::right, ios::adjustfield );
		out << "degrees of freedom: ";
		out.width( 10 );
		out.setf( ios::right, ios::adjustfield );
		long degFreedom = ( mNumItems - 1 );
		out << degFreedom << endl;
		
		out.precision( 2 );
		
#if NRUSED
		// p-value
		out.width( 25 );
		out.setf( ios::right, ios::adjustfield );
		out << "P-value: ";
		out.width( 10 );
		out.setf( ios::right, ios::adjustfield );
		out.precision( 4 );
		double gammResult = 1.1;
		gammResult =
			::gammq( (double)degFreedom / 2.0, mChi2atSolvedCopies / 2.0 );
		out << gammResult << endl;
		out.precision( 2 );
#endif

		out << endl;
		
		// Sensitivity analysis
		out << "Sensitivity analysis:" << endl << endl;
		
		// sens analysis headers
		out.width( colWidths[ kDilution ] );
		out << "Dilution";
		out.width( colWidths[ kNumPCRs ] );
		out << "# PCRs";
		out.width( colWidths[ kNumPos ] );
		out << "# pos";
		out.width( colWidths[ kPosExp ] );
		out << "Pos exp";
		out.width( colWidths[ kPlusOne ] );
		out << "+1 pos";
		out.width( colWidths[ kMinusOne ] );
		out << "-1 pos";
		out << endl << endl;
		
		// sens analysis rows
		for ( long i = 0; i < mNumItems; i++ )
		{
			out.width( colWidths[ kDilution ] );
			out.setf( ios::right, ios::adjustfield );
			out.precision( 4 );
			out << mList[ i ]->GetDilution();

			out.width( colWidths[ kNumPCRs ] );
			out.setf( ios::right, ios::adjustfield );
			out.precision( 2 );
			out << mList[ i ]->GetPCRs();

			out.width( colWidths[ kNumPos ] );
			out.setf( ios::right, ios::adjustfield );
			out << mList[ i ]->GetPositives();

			out.width( colWidths[ kPosExp ] );
			out.setf( ios::right, ios::adjustfield );
			out.precision( 1 );
			out << (double)mList[ i ]->GetPCRs() *
				( 1.0 - mList[ i ]->ProbOfNegative(
											mSolvedNumCopies,
											mProbFalsePos,
											mProbFalseNeg ) );
			out.precision( 2 );

			out.width( colWidths[ kPlusOne ] );
			out.setf( ios::right, ios::adjustfield );
			out << FindCopyNumToMinChiSquared( i, +1 );

			out.width( colWidths[ kMinusOne ] );
			out.setf( ios::right, ios::adjustfield );
			out << FindCopyNumToMinChiSquared( i, -1 );
			out << endl;
		}
	}
	
	cout << out.str();
}


void
CQualityAnalysis::OutputChi2DataPoints( void )
{
	// debugging function to output points bordering on the "solution" value
	// this was used to see whether we were solving at a maximum or minimum
	
	long numPoints = 50;
	
	double startPoint = 1.0;
	double endPoint = 60.0;
	
	if ( mSolvedNumCopies > 0 )
		endPoint = mSolvedNumCopies * 2.0;
	else
		endPoint = 200;
	
	double stepSize = ( endPoint - startPoint ) / numPoints;
	
	double thePoint = startPoint;
	
	cout.precision( 6 );
	for ( long i = 0; i < numPoints; i++ )
	{
		cout << thePoint << "\t" << ChiSquaredSum( thePoint );
		cout << "\t"
			<< D_ChiSquaredSum( thePoint )
			<< "\t" << D2_ChiSquaredSum( thePoint )
			<< endl;
		thePoint += stepSize;
	}
}


#pragma mark ==== Calculation of Sums ====

double
CQualityAnalysis::ChiSquaredSum(
	double inCopyNum,
	long	inWhichItemToTweak,
	long	inHowMuchToTweak )
{
// inWhichItemToTweak and inHowMuchToTweak are the keys to solving
// for +1 and -1 positive reations (the "Stability Analysis")

// inWhichItemToTweak corresponds to the Nth dilution in the list, and
// inHowMuchToTweak is +/-1 for one more or one less (respectively)
// pos reaction.
// So, to find out what the solution copy number would be if we had found
// one less positive reaction at the 3rd dilution, set inWhichItemToTweak=3
//  & inHowMuchToTweak=-1. It's that simple. 

// These values are also passed through the first and
// second derivative functions

	double result = 0.0;
	double tempResult = 0.0;
	
	for ( long itemNum = 0; itemNum < mNumItems; itemNum++ )
	{
		if ( itemNum == inWhichItemToTweak )
			tempResult = mList[ itemNum ]->ChiSquared(
										inCopyNum,
										mProbFalsePos,
										mProbFalseNeg,
										inHowMuchToTweak );
		else
			tempResult = mList[ itemNum ]->ChiSquared(
										inCopyNum,
										mProbFalsePos,
										mProbFalseNeg );
		result += tempResult;
	}
	
	return result;
}


double
CQualityAnalysis::D_ChiSquaredSum(
	double inCopyNum,
	long	inWhichItemToTweak,
	long	inHowMuchToTweak )
{
	// first derivative of ChiSquaredSum
	// a numerical approximation
	
	double result = 0.0;
	
	result += ChiSquaredSum(
					inCopyNum + mDerivH,
					inWhichItemToTweak,
					inHowMuchToTweak );
	
	result -= ChiSquaredSum(
					inCopyNum - mDerivH,
					inWhichItemToTweak,
					inHowMuchToTweak );
	
	result /= 2 * mDerivH;
	
	return result;
}


double
CQualityAnalysis::D2_ChiSquaredSum(
	double inCopyNum,
	long	inWhichItemToTweak,
	long	inHowMuchToTweak )
{
	// second derivative of ChiSquaredSum
	// - a numerical approximation
	
	double result = 0.0;
	
	result += ChiSquaredSum(
					inCopyNum + mDerivH,
					inWhichItemToTweak,
					inHowMuchToTweak );
					
	result -= 2 * ChiSquaredSum(
						inCopyNum,
						inWhichItemToTweak,
						inHowMuchToTweak );
						
	result += ChiSquaredSum(
					inCopyNum - mDerivH,
					inWhichItemToTweak,
					inHowMuchToTweak );
	
	result /= mDerivH * mDerivH;
	
	return result;
}


#pragma mark ==== File routines ====

void
CQualityAnalysis::ReadFile( CFile * inFile )
{
	const short buffSize = 100;
	char lineBuffer[ buffSize ];
	
	InitObject();
	
	if ( inFile->NextLineIsNewAnalysis() )
	{
		inFile->GetLine( lineBuffer, buffSize );
		
#if __OLD_STRING_STREAMS__
		istrstream lineStream( lineBuffer );
#else
		istringstream lineStream( lineBuffer );
#endif
		
		lineStream.ignore(1).getline( mName, kMaxNameLength );
	}
	
	// load next line -- dilutions
	inFile->GetLine( lineBuffer, buffSize );
	ReadDilutions( lineBuffer );
	
	// load next line -- numbers of PCRs
	inFile->GetLine( lineBuffer, buffSize );
	ReadNumPCRs( lineBuffer );
	
	// load next line -- numbers of Positive reactions
	inFile->GetLine( lineBuffer, buffSize );
	ReadNumPosReactions( lineBuffer );
	
	// look for false neg or pos settings
	
	while ( ! ( inFile->NextLineIsNewAnalysis() | inFile->End() ) )
	{
		inFile->GetLine( lineBuffer, buffSize );
		ReadFalseProbs( lineBuffer );
	}
}


void
CQualityAnalysis::ReadDilutions( const char *inBuffer )
{
// if you get errors here about "can't pass const to non-const", change
// "inBuffer" to "(char *)(void *)inBuffer". Or update your libraries to
// a newer, more standard version.
#if __OLD_STRING_STREAMS__
	istrstream lineStream( inBuffer );
#else
	istringstream lineStream( inBuffer );
#endif
	
	long numItems = 0;
	double dilution;
	while ( lineStream >> dilution )
	{
		mList[ numItems ] = new CPCRStat( dilution );
		numItems++;
		if ( numItems > kMaxNumDilutions )
			cout
				<< "More dilutions than I can handle ("
				<< kMaxNumDilutions
				<< " max)!"
				<< endl;
	}
	
	mNumItems = numItems;
}


void
CQualityAnalysis::ReadNumPCRs( const char *inBuffer )
{
// if you get errors here about "can't pass const to non-const", change
// "inBuffer" to "(char *)(void *)inBuffer". Or update your libraries to
// a newer, more standard version.
#if __OLD_STRING_STREAMS__
	istrstream lineStream( inBuffer );
#else
	istringstream lineStream( inBuffer );
#endif
	long i = 0;
	long numPCRs;
	
	while ( lineStream >> numPCRs )
	{
		mList[ i ]->SetPCRs( numPCRs );
		i++;
	}
	
	if ( i < mNumItems )
		cout << "## Error: fewer PCR values than dilutions!" << endl;
}


void
CQualityAnalysis::ReadNumPosReactions( const char *inBuffer )
{
// if you get errors here about "can't pass const to non-const", change
// "inBuffer" to "(char *)(void *)inBuffer". Or update your libraries to
// a newer, more standard version.
#if __OLD_STRING_STREAMS__
	istrstream lineStream( inBuffer );
#else
	istringstream lineStream( inBuffer );
#endif
	long i = 0;
	long numPosReactions;
	
	while ( lineStream >> numPosReactions )
	{
		mList[ i ]->SetPositives( numPosReactions );
		i++;
	}
	
	if ( i < mNumItems )
		cout << "## Error: fewer reaction results than dilutions!" << endl;
}


void
CQualityAnalysis::ReadFalseProbs( const char *inBuffer )
{
// if you get errors here about "can't pass const to non-const", change
// "inBuffer" to "(char *)(void *)inBuffer". Or update your libraries to
// a newer, more standard version.
#if __OLD_STRING_STREAMS__
	istrstream lineStream( inBuffer );
#else
	istringstream lineStream( inBuffer );
#endif
	char theString[ 50 ];
	theString[ 0 ] = '\0';
	
	double	theProb;
	
	lineStream >> theString >> theProb;
	
	if ( strcmp( theString, "probFalsePos" ) == 0 )
		mProbFalsePos = theProb;
	else
		if ( strcmp( theString, "probFalseNeg" ) == 0 )
			mProbFalseNeg = theProb;
}

