// local:
#include "polygon.h"
#include "transform.h"

// stdlib:
#define _USE_MATH_DEFINES
#include <math.h>

float Polygon::getArea() const
{
    float area=0.0f;
    int i, j = static_cast< int >( points.size() ) - 1;
    for( i = 0; i < static_cast< int >( points.size() ); ++i )
    {
        area += ( points[j].x + points[i].x ) * ( points[j].y - points[i].y ); 
        j = i;
    }
    return area * 0.5f; 
}

Point2D Polygon::getCentroid() const
{
    Point2D centroid( 0.0f, 0.0f );
    for( int i = 0; i < static_cast< int >( points.size() ); ++i )
        centroid.add( points[ i ] );
    return centroid.div( static_cast< float >( points.size() ) ); 
}

Polygon Polygon::getCircle( const Point2D& center, float radius, int n )
{
    Polygon poly;
	poly.points.reserve( n );
    for( int i = 0; i < n; ++i )
    {
        poly.points.push_back( Point2D( center.x + radius * static_cast< float >( cos( i * 2.0 * M_PI / n ) ), 
                                        center.y - radius * static_cast< float >( sin( i * 2.0 * M_PI / n ) ) ) );
    }
    return poly;
}

bool Polygon::contains( const Point2D& p ) const
{
    bool c = false;
    int i, j;
    float vertxi,vertyi,vertxj,vertyj;
    for( i = 0, j = static_cast< int >( points.size() ) - 1; i < static_cast< int >( points.size() ); j = i++ ) 
    {
        vertxi = points[i].x;
        vertyi = points[i].y;
        vertxj = points[j].x;
        vertyj = points[j].y;
        if ( ((vertyi>p.y) != (vertyj>p.y)) && (p.x < (vertxj-vertxi) * (p.y-vertyi) / (vertyj-vertyi) + vertxi) )
        {
            c = !c;
        }
    }
    return c;
}

bool Polygon::doesLineSegmentIntersect( const Point2D& p, const Point2D& p2 ) const
{
    for( int i = 0; i < static_cast< int >( points.size() ); ++i )
    {
        Point2D q = points[ i ];
        Point2D q2 = points[ (i+1)%static_cast< int >( points.size() ) ];
        if( lineSegmentsIntersect( p, p2, q, q2 ) )
            return true;
    }
    return false;
}

Polygon::PolygonRelation Polygon::getPolygonRelationWith( const Polygon& poly ) const
{
    for( int i = 0; i < points.size(); ++i )
    {
        Point2D p = points[ i ];
        Point2D p2 = points[ (i+1)%static_cast< int >( points.size() ) ];
        for( int j = 0; j < static_cast< int >( poly.points.size() ); ++j )
        {
            Point2D q = poly.points[ j ];
            Point2D q2 = poly.points[ (j+1)%static_cast< int >( poly.points.size() ) ];
            if( lineSegmentsIntersect( p, p2, q, q2 ) )
                return PolygonRelation::intersecting;
        }
    }
    if( poly.contains( points[0] ) )
        return PolygonRelation::contained;
    if( contains( poly.points[0] ) )
        return PolygonRelation::containing;
    return PolygonRelation::disjoint;
}

void Polygon::getTransformed( const Transform& t, Polygon& output ) const
{
	output.points.clear();
	output.points.reserve( points.size() );
    for( int iPt = 0; iPt < static_cast< int >( points.size() ); ++iPt )
        output.points.push_back( t.apply( points[ iPt ] ) );
}

bool Polygon::isPointNearEdge( const Point2D& p, float tol ) const
{
    for( int iPt = 0; iPt < static_cast< int >( points.size() ); ++iPt )
        if( distToSegment( p, points[iPt], points[ (iPt+1)%static_cast< int >( points.size() ) ] ) < tol )
            return true;
    return false;
}

Point2D Polygon::getNormal( int iPt ) const
{
    // return average of two neighboring edge normals
    Point2D a = points[ (iPt+points.size()-1)%static_cast< int >( points.size() ) ];
    Point2D b = points[ iPt ];
    Point2D c = points[ (iPt+1)%static_cast< int >( points.size() ) ];
    Point2D e1 = sub( b, a );
    Point2D en1 = Point2D( -e1.y, e1.x ).normalize();
    Point2D e2 = sub( c, b );
    Point2D en2 = Point2D( -e2.y, e2.x ).normalize();
    Point2D n = add( en1, en2 ).normalize();
    return n;
}

bool Polygon::isSelfIntersecting() const
{
    // does any line segment cross any other?
    for( int iPt = 0; iPt < static_cast< int >( points.size() ); ++iPt )
    {
        int iP1 = iPt;
        int iP2 = (iPt+1)%static_cast< int >( points.size() );
        Point2D p1 = points[ iP1 ];
        Point2D p2 = points[ iP2 ];
        for( int iPt2 = 0; iPt2 < iPt-1; ++iPt2 )
        {
            int iQ1 = iPt2;
            int iQ2 = (iPt2+1)%static_cast< int >( points.size() );
            if( iQ1 == iP2 || iQ2 == iP1 )
                continue; // neighboring edges, ignore
            Point2D q1 = points[ iQ1 ];
            Point2D q2 = points[ iQ2 ];
            if( lineSegmentsIntersect( p1, p2, q1, q2 ) )
                return true;
        }
    }
    return false;
}
