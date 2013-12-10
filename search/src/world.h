// local:
#include "piece.h"

// STL:
#include <string>

class World
{
	public:

		World();

		bool isValidSolution() const; 
		float getPercentCovered() const;

		void addRandomPiece();
		int getNumberOfPieces() const { return static_cast< int >( pieces.size() ); }
		void deletePieces() { pieces.clear(); }

		bool adaptPieces( float growStep, float minEdgeLength ); // returns whether something changed

		std::string getAsOBJFileFormat() const;

	    std::vector< Piece > pieces;

	private:

		void clipPiecesAgainstTargets();
		void clipPiecesAgainstOthers();

		bool isInsideTarget( int iTarget, const Point2D& p ) const;
		bool isInsideTarget( int iTarget, const Polygon& poly ) const;

	private:

		Point2D circleCenter;
		float circleRadius;
    
		Point2D squareCenter;
		float squareRadius;

		int numTargets;

};
