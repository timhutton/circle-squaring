// STL:
#include <string>

class Point2D
{
	public:

		Point2D( float x, float y );

		Point2D add( const Point2D& p );
		Point2D sub( const Point2D& p );
		float dist( const Point2D& p );
		Point2D mul( float f );
		Point2D div( float f );

		float len() const;
		Point2D normalize();

		std::string getAsJSONFormat() const;

		float x, y;
};

float crossProduct2D( const Point2D& v,const Point2D& w ); 
Point2D add( const Point2D& a, const Point2D& b );
Point2D sub( const Point2D& a, const Point2D& b );
float dist2( const Point2D& v, const Point2D& w );
float distToSegmentSquared( const Point2D& p, const Point2D& v, const Point2D& w );
float distToSegment( const Point2D& p, const Point2D& v, const Point2D& w );
bool lineSegmentsIntersect( const Point2D& p, const Point2D& p2, const Point2D& q, const Point2D& q2 );
float dot( const Point2D& a, const Point2D& b );
float angleBetweenTwoVectors( const Point2D& a, const Point2D& b );
