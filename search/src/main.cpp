// local:
#include "world.h"

// STL:
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// stdlib:
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

void GetInitialSolution( const int N, World& world );
void Optimize();
void SearchFromScratch();

float randomFloat( float low, float high )
{
	return low + ( (high-low)*rand() ) / RAND_MAX;
}

int randomInt( int low, int high )
{
	return low + rand()%(high-low+1);
}

int main()
{
	srand( static_cast< unsigned int >( time( nullptr ) ) );

	// choose your approach:
	Optimize();
	//SearchFromScratch()
}

void Optimize()
{
	// approach
	// 1. Find any solution for N
	// 2. Try a local small change, e.g. one of the transform values for one of the pieces
	// 3. If it gives an improvement then adopt it. 
	// 4. Repeat from 2 until stuck. Then report result and start from 1.

	World world,previousWorld;

	const int maxSteps = 1000;
	const int targetNumPieces = 2;
	const int numRefinements = 4;
	const float growStepTable[ numRefinements ] = { 3.0f, 1.0f, 0.4f, 0.1f };
	const float edgeLengthTable[ numRefinements ] = { 30.0f, 20.0f, 20.0f, 10.0f };
	const int numSearchesBeforeRestart = 200;
	const float rotJiggle = static_cast< float >( 1.0f * M_PI/180.0f ); // 1 degree
	const float moveJiggle = 1.0f;

	int remainingSearches;

	float bestEverCoverage = 0.0f;

	while( true )
	{
		float bestCoverage = 0.0f;
		remainingSearches = numSearchesBeforeRestart;

		GetInitialSolution( targetNumPieces, world );

		while( --remainingSearches > 0 )
		{
			cout << ".";

			// take a snapshot of the world before we tweak it
			previousWorld = world;

			// alter it by a small amount
			for( int iPiece = 0; iPiece < world.getNumberOfPieces(); ++iPiece )
			{
				for( int iTarget = 0; iTarget < 2; ++iTarget )
				{
					world.pieces[iPiece].originToTarget[iTarget].rotate += randomFloat(-1.0f * rotJiggle,rotJiggle);
					world.pieces[iPiece].originToTarget[iTarget].translate.x += randomFloat(-1.0f * moveJiggle,moveJiggle);
					world.pieces[iPiece].originToTarget[iTarget].translate.y += randomFloat(-1.0f * moveJiggle,moveJiggle);
				}
			}

			// allow the solution to fully grow
			int iRefinementStep = 0;
			int iStep = 0;
			bool moved;
			do
			{
				moved = world.adaptPieces( growStepTable[ iRefinementStep ], edgeLengthTable[ iRefinementStep ] );
				++iStep;
			} while( world.getNumberOfPieces() == targetNumPieces && (moved || ++iRefinementStep < numRefinements ) && iStep < maxSteps );

			if( world.getNumberOfPieces() == targetNumPieces && world.isValidSolution() && world.getPercentCovered() > bestCoverage )
			{
				// adopt the improvement
				bestCoverage = world.getPercentCovered();
				if( bestCoverage > bestEverCoverage )
				{
					cout << "\nFound improved result for N=" << world.getNumberOfPieces() << " : " << world.getPercentCovered() << "\n";
					bestEverCoverage = bestCoverage;
					ostringstream filename;
					filename << "output_N" << world.getNumberOfPieces() << ".obj";
					ofstream out( filename.str().c_str() );
					out << world.getAsOBJFileFormat();
					cout << "\nWrote " << filename.str() << "\n";
				}
				else
				{
					cout << " " << world.getPercentCovered() << " ";
				}
				// show where we've got to
				{
					ostringstream filename;
					filename << "progress_N" << world.getNumberOfPieces() << ".obj";
					ofstream out( filename.str().c_str() );
					out << world.getAsOBJFileFormat();
				}
				remainingSearches = numSearchesBeforeRestart;
			}
			else
			{
				// revert to the one that was better
				world = previousWorld;
			}
		}
		cout << "\nTried " << numSearchesBeforeRestart << " jiggles, restarting. Current winner for N=" << targetNumPieces << ": " << bestEverCoverage << "\n";
	}
}

void SearchFromScratch()
{
	// approach:
	// 1. Find any solution for N.
	// 2. Allow the pieces to grow.
	// 3. If best so far then output it.
	// 4. Repeat from 1.

	float bestCoverage = 0.0f;

	World world;

	const int maxSteps = 1000;
	const int targetNumPieces = 5;
	const int numRefinements = 4;
	const float growStepTable[ numRefinements ] = { 3.0f, 1.0f, 0.4f, 0.1f };
	const float edgeLengthTable[ numRefinements ] = { 30.0f, 20.0f, 20.0f, 10.0f };

	int numSearched = 0;

	while( true ) // just keep looking for better solutions
	{
		++numSearched;
		cout << ".";
		if( ( ++numSearched % 1000 ) == 0 )
			cout << numSearched;

		int iRefinementStep = 0;
		int iStep = 0;

		GetInitialSolution( targetNumPieces, world );

		bool moved;
		do
		{
			moved = world.adaptPieces( growStepTable[ iRefinementStep ], edgeLengthTable[ iRefinementStep ] );
			++iStep;
		} while( world.getNumberOfPieces() == targetNumPieces && (moved || ++iRefinementStep < numRefinements ) && iStep < maxSteps );

		if( world.getNumberOfPieces() == targetNumPieces && world.getPercentCovered() > bestCoverage && world.isValidSolution() )
		{
			cout << "\nFound new result for N=" << world.getNumberOfPieces() << " : " << world.getPercentCovered() << "\n";
			ostringstream filename;
			filename << "output_N" << world.getNumberOfPieces() << ".obj";
			ofstream out( filename.str().c_str() );
			out << world.getAsOBJFileFormat();
			cout << "\nWrote " << filename.str() << "\n";
			bestCoverage = world.getPercentCovered();
		}
	}
}

void GetInitialSolution( const int N, World& world )
{
	// find a valid solution for N
	do 
	{
		world.deletePieces();
		for( int i = 0; i < N; ++i )
			world.addRandomPiece();
		world.adaptPieces( 5.0f, 20.0f );
	}
	while( world.getNumberOfPieces() != N || !world.isValidSolution() );
}