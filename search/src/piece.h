// local:
#include "polygon.h"
#include "transform.h"

// STL:
#include <vector>

class Piece
{
	public:

		void getPolygonOnTarget( int iTarget, Polygon& output ) const;

		void subdivide( float minEdgeLength );
		void removeVerticesCloserThan( float minEdgeLength );
		void removeVerticesStraighterThan( float minAngle );
		void removeSelfIntersectingVertices();

        void recenter();
		void removeVertex( int iPt );

		std::string getAsJSONFormat() const;

		Polygon atOrigin;
		std::vector< Transform > originToTarget;
};
