#include "polygon.h"
#include "transform.h"

#include <vector>

class Piece
{
	public:

		Piece();

		void getPolygonOnTarget( int iTarget, Polygon& output ) const;

		void subdivide( float minEdgeLength );
		void removeVerticesCloserThan( float minEdgeLength );
		void removeVerticesStraighterThan( float minAngle );
		void removeSelfIntersectingVertices();

	private:

        void recenter();
		void removeVertex( int iPt );

	private:

		Polygon atOrigin;
		std::vector< Transform > originToTarget;
};
