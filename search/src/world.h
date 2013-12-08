#include "piece.h"
#include "point2d.h"
#include "polygon.h"

class World
{
	public:

		World();

		bool isValidSolution() const; 
		float getPercentCovered() const;

		void adaptPieces();

	private:

		void clipPiecesAgainstTargets();
		void clipPiecesAgainstOthers();

	private:

		Point2D circleCenter;
		float circleRadius;
    
		Point2D squareCenter;
		float squareRadius;

	    std::vector< Piece > pieces;
};
