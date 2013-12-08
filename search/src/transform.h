#include "point2d.h"

class Transform
{
	public:
		
		Transform( float r, const Point2D& t );

		Point2D apply( const Point2D& p );
		Point2D applyInverse( const Point2D& p );
		
	private:

		float rotate;
		Point2D translate;
};
