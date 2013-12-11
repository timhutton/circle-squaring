// local:
#include "transform.h"

// stdlib:
#include <math.h>

// STL:
#include <sstream>
using namespace std;

Transform::Transform( float r, const Point2D& p )
	: rotate(r), translate( p.x, p.y )
{
}

Point2D Transform::apply( const Point2D& p ) const
{
    return Point2D( p.x * cos( rotate ) - p.y * sin( rotate ) + translate.x, 
                    p.x * sin( rotate ) + p.y * cos( rotate ) + translate.y );
}

Point2D Transform::applyInverse( const Point2D& p ) const
{
    return Point2D( ( p.x - translate.x ) * cos( -rotate ) - ( p.y - translate.y ) * sin( -rotate ), 
                    ( p.x - translate.x ) * sin( -rotate ) + ( p.y - translate.y ) * cos( -rotate ) );
}

string Transform::getAsJSONFormat() const
{
	ostringstream oss;
	oss << "{ \"rotate\" : " << rotate << ", \"translate\" : " << translate.getAsJSONFormat() << " }";
	return oss.str();
}

