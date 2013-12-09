// local:
#include "piece.h"

// STL:
using namespace std;

void Piece::getPolygonOnTarget( int iTarget, Polygon& output ) const
{
    atOrigin.getTransformed( originToTarget[ iTarget ], output ); // TODO: cache this, only update when needed
}

void Piece::recenter() 
{
    Point2D center = atOrigin.getCentroid();
    for( int iPt = 0; iPt < static_cast< int >( atOrigin.points.size() ); ++iPt )
        atOrigin.points[ iPt ].sub( center );
    for( int iTarget = 0; iTarget < static_cast< int >( originToTarget.size() ); ++iTarget )
        originToTarget[ iTarget ].translate = originToTarget[ iTarget ].apply( center );
}

void Piece::removeVertex( int iPt ) 
{
    atOrigin.points.erase( atOrigin.points.begin()+iPt );
}

void Piece::subdivide( float minEdgeLength ) 
{
    vector< Point2D > newPoints;
    for( int iOldPt = 0; iOldPt < static_cast< int >( atOrigin.points.size() ); ++iOldPt )
    {
        newPoints.push_back( atOrigin.points[ iOldPt ] );
        if( atOrigin.points[ iOldPt ].dist( atOrigin.points[ (iOldPt+1)%static_cast< int >( atOrigin.points.size() ) ] ) > minEdgeLength )
            newPoints.push_back( add( atOrigin.points[ iOldPt ], atOrigin.points[ (iOldPt+1)%static_cast< int >( atOrigin.points.size() ) ] ).div( 2.0f ) );
    }
    atOrigin.points = newPoints;
}

void Piece::removeVerticesCloserThan( float minEdgeLength ) 
{
	bool removedOne;
    do {
        removedOne = false;
        for( int iPt = 0; iPt < static_cast< int >( atOrigin.points.size() ); ++iPt )
        {
            if( atOrigin.points[ iPt ].dist( atOrigin.points[ (iPt+1)%static_cast< int >( atOrigin.points.size() ) ] ) < minEdgeLength ) {
                removeVertex( iPt );
                removedOne = true;
                break;
            }
        }
    } while( removedOne );
}

void Piece::removeVerticesStraighterThan( float minAngle ) 
{
	bool removedOne;
    do {
        removedOne = false;
        for( int iPt = 0; iPt < static_cast< int >( atOrigin.points.size() ); ++iPt )
        {
            Point2D edge1 = sub( atOrigin.points[ iPt ], atOrigin.points[ (iPt+1)%static_cast< int >( atOrigin.points.size() ) ] );
            Point2D edge2 = sub( atOrigin.points[ (iPt+static_cast< int >( atOrigin.points.size() )-1)%static_cast< int >( atOrigin.points.size() ) ], atOrigin.points[ iPt ]  );
            if( angleBetweenTwoVectors( edge1, edge2 ) < minAngle ) {
                removeVertex( iPt );
                removedOne = true;
                break;
            }
        }
    } while( removedOne );
}

void Piece::removeSelfIntersectingVertices() 
{
    // does any line segment cross any other?
    for( int iPt = static_cast< int >( atOrigin.points.size() )-1; iPt >= 0; --iPt )
    {
        int iP1 = iPt;
        int iP2 = (iPt+1)%static_cast< int >( atOrigin.points.size() );
        Point2D p1 = atOrigin.points[ iP1 ];
        Point2D p2 = atOrigin.points[ iP2 ];
        for( int iPt2 = 0; iPt2 < iPt-1; ++iPt2 )
        {
            int iQ1 = iPt2;
            int iQ2 = (iPt2+1)%static_cast< int >( atOrigin.points.size() );
            if( iQ1 == iP2 || iQ2 == iP1 )
                continue; // neighboring edges, ignore
            Point2D q1 = atOrigin.points[ iQ1 ];
            Point2D q2 = atOrigin.points[ iQ2 ];
            if( lineSegmentsIntersect( p1, p2, q1, q2 ) ) {
                removeVertex( iPt );
                break;
            }
        }
    }
}
