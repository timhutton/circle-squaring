// local:
#include "point2d.h"

// stdlib:
#include <math.h>

// STL:
#include <sstream>
using namespace std;

Point2D::Point2D( float x, float y ) 
	: x(x), y(y) 
{
}

Point2D Point2D::add( const Point2D& p ) 
{ 
	x+=p.x; 
	y+=p.y; 
	return *this; 
}

Point2D Point2D::sub( const Point2D& p ) 
{ 
	x-=p.x; 
	y-=p.y; 
	return *this; 
}

float Point2D::dist( const Point2D& p ) 
{ 
	return sqrt( pow(p.x-x,2.0f)+pow(p.y-y,2.0f) ); 
}

Point2D Point2D::mul( float f ) 
{ 
	x*=f; 
	y*=f; 
	return *this; 
}

Point2D Point2D::div( float f ) 
{ 
	x/=f; 
	y/=f; 
	return *this; 
}

float Point2D::len() const 
{ 
	return sqrt( x*x + y*y ); 
}

Point2D Point2D::normalize() 
{ 
	return div( len() ); 
}

string Point2D::getAsJSONFormat() const
{
	ostringstream oss;
	oss << "{ \"x\" : " << x << ", \"y\" : " << y << " }";
	return oss.str();
}

float crossProduct2D( const Point2D& v,const Point2D& w ) 
{ 
	return v.x*w.y - v.y*w.x; 
}

Point2D add( const Point2D& a, const Point2D& b ) 
{ 
	return Point2D( a.x + b.x, a.y + b.y ); 
}

Point2D sub( const Point2D& a, const Point2D& b ) 
{ 
	return Point2D( a.x - b.x, a.y - b.y ); 
}

float dist2( const Point2D& v, const Point2D& w ) 
{ 
	return pow( v.x - w.x, 2.0f ) + pow( v.y - w.y, 2.0f ); 
}

float distToSegmentSquared( const Point2D& p, const Point2D& v, const Point2D& w ) 
{
  float l2 = dist2(v, w);
  if (l2 == 0) return dist2(p, v);
  float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
  if (t < 0) return dist2(p, v);
  if (t > 1) return dist2(p, w);
  return dist2(p, Point2D( v.x + t * (w.x - v.x), v.y + t * (w.y - v.y) ) );
}

float distToSegment( const Point2D& p, const Point2D& v, const Point2D& w ) 
{ 
	return sqrt( distToSegmentSquared( p, v, w) ); 
}

bool lineSegmentsIntersect( const Point2D& p, const Point2D& p2, const Point2D& q, const Point2D& q2 ) 
{
    // two line segments: [p,p2] and [q,q2] - do they intersect?
    // http://stackoverflow.com/a/565282/126823
    Point2D r = sub(p2, p);
    Point2D s = sub(q2, q);

    float uNumerator = crossProduct2D(sub(q, p), r);
    float denominator = crossProduct2D(r, s);

    if( uNumerator == 0 && denominator == 0 ) {
        // colinear, so do they overlap?
        return ((q.x - p.x < 0) != (q.x - p2.x < 0) != (q2.x - p.x < 0) != (q2.x - p2.x < 0)) ||
                ((q.y - p.y < 0) != (q.y - p2.y < 0) != (q2.y - p.y < 0) != (q2.y - p2.y < 0));
    }

    if( denominator == 0 ) {
        // lines are parallel
        return false;
    }

    float u = uNumerator / denominator;
    float t = crossProduct2D(sub(q, p), s) / denominator;

    return (t >= 0) && (t <= 1) && (u >= 0) && (u <= 1);
}


float dot( const Point2D& a, const Point2D& b ) 
{
    return a.x * b.x + a.y * b.y;
}

float angleBetweenTwoVectors( const Point2D& a, const Point2D& b ) 
{
    return acos( dot( a, b ) / ( a.len() * b.len() ) );
}

