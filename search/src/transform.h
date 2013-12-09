// local:
#include "point2d.h"

class Transform
{
	public:
		
		Transform( float r, const Point2D& t );

		Point2D apply( const Point2D& p ) const;
		Point2D applyInverse( const Point2D& p ) const;
		
		float rotate;
		Point2D translate;
};
