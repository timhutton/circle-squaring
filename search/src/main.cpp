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

int main()
{
	srand( static_cast< unsigned int >( time( nullptr ) ) );

	float bestCoverage = 0.0f;

	World world;

	const int maxSteps = 1000;
	const int targetNumPieces = 4;
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

		world.deletePieces();
		for( int i = 0; i < targetNumPieces; ++i )
			world.addRandomPiece();

		int iRefinementStep = 0;
		int iStep = 0;

		// workaround for issue where nested polygons don't correct themselves
		world.adaptPieces( growStepTable[ iRefinementStep ], edgeLengthTable[ iRefinementStep ] );
		if( !world.isValidSolution() )
			continue;

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

	return EXIT_SUCCESS;
}