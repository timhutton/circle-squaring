// local:
#include "world.h"

// stdlib:
#define _USE_MATH_DEFINES
#include <math.h>

// STL:
#include <sstream>
using namespace std;

World::World() :
	  numTargets( 2 )
    , circleCenter( 250.0f, 350.0f )
    , circleRadius( 200.0f )
    , squareCenter( 650.0f, 350.0f )
    , squareRadius( static_cast< float >( circleRadius * sqrt( M_PI ) / 2.0f ) )
{
}

bool World::isValidSolution() const
{
    for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece ) {
        // polygon must not be self-intersecting
        if( pieces[ iPiece ].atOrigin.isSelfIntersecting() )
            return false;
        // polygon must be clockwise (positive area)
        if( pieces[ iPiece ].atOrigin.getArea() <= 0 )
            return false;
    }
    vector< Polygon > polysOnTarget( static_cast< int >( pieces.size() ) );
    for( int iTarget = 0; iTarget < numTargets; ++iTarget ) {
        for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece ) {
            // transform each piece onto this target
            pieces[ iPiece ].getPolygonOnTarget( iTarget, polysOnTarget[ iPiece ] );
            // invalid if any piece not fully contained in each target
            if( !isInsideTarget( iTarget, polysOnTarget[ iPiece ] ) ) // TODO: update js code with this (and all other targets[].contains)
                return false;
            // invalid if any piece not fully outside any other
            for( int iPiece2 = 0; iPiece2 < iPiece; ++iPiece2 ) 
                if( polysOnTarget[ iPiece ].getPolygonRelationWith( polysOnTarget[ iPiece2 ] ) != Polygon::PolygonRelation::disjoint ) 
                    return false;
        }
    }
    return true;
}

float World::getPercentCovered() const
{
    // assumes no overlapping
    float area = 0.0f;
    for( int i = 0; i < static_cast< int >( pieces.size() ); ++i )
        area += pieces[ i ].atOrigin.getArea();
    return static_cast< float >( 100.0f * area / ( M_PI * circleRadius * circleRadius ) );
}

void World::addRandomPiece()
{
	Piece piece;
	piece.atOrigin = Polygon::getCircle( Point2D( 0.0f, 0.0f ), static_cast< float >( rand()*circleRadius/RAND_MAX ), 12 );
	piece.originToTarget.push_back( Transform( static_cast< float >( rand()*2.0*M_PI/RAND_MAX ), 
			add( circleCenter, Point2D( static_cast< float >( rand()*circleRadius*2.0/RAND_MAX - circleRadius ), 
										static_cast< float >( rand()*circleRadius*2.0/RAND_MAX - circleRadius ) ) ) ) );
	piece.originToTarget.push_back( Transform( static_cast< float >( rand()*2.0*M_PI/RAND_MAX ), 
			add( squareCenter, Point2D( static_cast< float >( rand()*squareRadius*2.0/RAND_MAX - squareRadius ), 
										static_cast< float >( rand()*squareRadius*2.0/RAND_MAX - squareRadius ) ) ) ) );
	pieces.push_back( piece );
}

void World::clipPiecesAgainstTargets() 
{
	Polygon polygonOnTarget;
    for( int iPiece = static_cast< int >( pieces.size() )-1; iPiece >=0; --iPiece ) 
    {
        Piece& piece = pieces[ iPiece ];
        bool pieceDeleted = false;
        for( int iTarget = 0; iTarget < static_cast< int >( piece.originToTarget.size() ); ++iTarget ) 
        {
            piece.getPolygonOnTarget( iTarget, polygonOnTarget );
            for( int iPt = static_cast< int >( polygonOnTarget.points.size() )-1; iPt >= 0; --iPt ) 
            {
                if( !isInsideTarget( iTarget, polygonOnTarget.points[ iPt ] ) )
                {
                    piece.removeVertex( iPt );
                    if( piece.atOrigin.points.size() < 3 )
                    {
                        pieces.erase( pieces.begin() + iPiece );
                        pieceDeleted = true;
                        break;
                    }
                }
            }
            if( pieceDeleted )
                break;
        }
    }
}

void World::clipPiecesAgainstOthers() 
{
	Polygon polygonOnTarget;
	Polygon otherPolygonOnTarget;
    for( int iPiece = static_cast< int >( pieces.size() )-1; iPiece >=0; --iPiece ) 
    {
        Piece& piece = pieces[ iPiece ];
        bool pieceDeleted = false;
        for( int iTarget = 0; iTarget < numTargets; ++iTarget )
        {
            piece.getPolygonOnTarget( iTarget, polygonOnTarget );
            for( int iOtherPiece = 0; iOtherPiece < static_cast< int >( pieces.size() ); ++iOtherPiece ) 
            {
                if( iOtherPiece == iPiece )
                    continue;
                pieces[ iOtherPiece ].getPolygonOnTarget( iTarget, otherPolygonOnTarget );
                for( int iPt = static_cast< int >( polygonOnTarget.points.size() )-1; iPt >= 0; --iPt ) 
                {
                    // check that the point isn't inside the other piece
                    // and that its edges don't intersect it
                    Point2D pLeft = polygonOnTarget.points[ (iPt+static_cast< int >( polygonOnTarget.points.size() )-1)%static_cast< int >( polygonOnTarget.points.size() ) ];
                    Point2D pHere = polygonOnTarget.points[ iPt ];
                    Point2D pRight = polygonOnTarget.points[ (iPt+1)%static_cast< int >( polygonOnTarget.points.size() ) ];
                    if( otherPolygonOnTarget.contains( pHere ) ||
                        otherPolygonOnTarget.doesLineSegmentIntersect( pLeft, pHere ) || 
                        otherPolygonOnTarget.doesLineSegmentIntersect( pHere, pRight ) )
                    {
                        piece.removeVertex( iPt );
                        if( piece.atOrigin.points.size() < 3 )
                        {
                            pieces.erase( pieces.begin() + iPiece );
                            pieceDeleted = true;
                            break;
                        }
						// need to refresh the polygon
			            piece.getPolygonOnTarget( iTarget, polygonOnTarget ); // TODO: add this missing line in js code
                    }
                }
                if( pieceDeleted )
                    break;
            }
            if( pieceDeleted )
                break;
        }
    }
}

bool World::adaptPieces( float growStep, float minEdgeLength ) 
{
	bool pieceDeleted;
	do {
		pieceDeleted = false;
		for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece )
		{
			pieces[ iPiece ].removeSelfIntersectingVertices();
			if( pieces[ iPiece ].atOrigin.points.size() < 3 ) // TODO: add this missing check in js code
			{
				pieces.erase( pieces.begin() + iPiece );
				pieceDeleted = true;
				break;
			}
		}
	} while( pieceDeleted );

    clipPiecesAgainstTargets();
    clipPiecesAgainstOthers();
    bool somethingChanged = false;
	Polygon otherPolygonOnTarget;
    // move vertices outwards a little on their surface normal for as long as solution remains valid
    for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece ) 
    {
        Piece& piece = pieces[ iPiece ];
        piece.subdivide( minEdgeLength );
        piece.removeVerticesCloserThan( 3 );
        for( int iPt = 0; iPt < static_cast< int >( piece.atOrigin.points.size() ); ++iPt ) 
        {
            Point2D oldPos = piece.atOrigin.points[ iPt ];
            Point2D move = piece.atOrigin.getNormal( iPt ).mul( growStep );
            piece.atOrigin.points[ iPt ].add( move );
            bool newPosIsOK = true;
            if( newPosIsOK )
            {
                for( int iTarget = 0; iTarget < static_cast< int >( piece.originToTarget.size() ); ++iTarget ) {
                    Point2D pOnTarget = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ iPt ] );
                    // check whether the point is still inside the target shape
                    if( !isInsideTarget( iTarget, pOnTarget ) )
					{
                        newPosIsOK = false;
                        break;
                    }
                    // check whether the point doesn't intersect any other shape
                    for( int iOtherPiece = 0; iOtherPiece < static_cast< int >( pieces.size() ); ++iOtherPiece ) {
                        if( iOtherPiece == iPiece )
                            continue;
                        pieces[ iOtherPiece ].getPolygonOnTarget( iTarget, otherPolygonOnTarget );
                        Point2D left = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ (iPt+static_cast< int >( piece.atOrigin.points.size() )-1)%static_cast< int >( piece.atOrigin.points.size() ) ] );
                        Point2D right = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ (iPt+1)%static_cast< int >( piece.atOrigin.points.size() ) ] );
                        if( otherPolygonOnTarget.doesLineSegmentIntersect( left, pOnTarget ) || otherPolygonOnTarget.doesLineSegmentIntersect( pOnTarget, right ) ) {
                            newPosIsOK = false;
                            break;
                        }
                    }
                    if( !newPosIsOK )
                        break;
                }            
            }
            if( !newPosIsOK )
                piece.atOrigin.points[ iPt ] = oldPos;
            else
                somethingChanged = true;
        }
        piece.recenter();
    }
    return somethingChanged;
}

bool World::isInsideTarget( int iTarget, const Point2D& p ) const
{
	switch( iTarget )
	{
		case 0: return dist2( p, circleCenter ) < pow( circleRadius, 2.0f );
		default:
		case 1: return fabs( p.x - squareCenter.x ) < squareRadius && fabs( p.y - squareCenter.y ) < squareRadius;
	}
}

bool World::isInsideTarget( int iTarget, const Polygon& poly ) const
{
	for( int iPt = 0; iPt < static_cast< int >( poly.points.size() ); ++iPt )
		if( !isInsideTarget( iTarget, poly.points[ iPt ] ) )
			return false;
	return true;
}

string World::getAsOBJFileFormat() const // TODO: add to js code
{
	ostringstream oss;
	oss << "# Circle target has center at (" << circleCenter.x << "," << circleCenter.y << ") and radius " << circleRadius << "\n";
	oss << "# Square target has center at (" << squareCenter.x << "," << squareCenter.y << ") and square radius " << squareRadius << "\n";
	oss << "# N=" << pieces.size() << " pieces, giving " << getPercentCovered() << "% coverage\n";
	// print transforms
	for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece )
	{
		oss << "# Transforms for piece " << iPiece+1 << ":";
		for( int iTarget = 0; iTarget < numTargets; ++iTarget )
		{
			oss << " " << pieces[ iPiece ].originToTarget[ iTarget ].rotate << " rad";
			oss << " (" << pieces[ iPiece ].originToTarget[ iTarget ].translate.x << "," << pieces[ iPiece ].originToTarget[ iTarget ].translate.y << "),";
		}
		oss << "\n";
	}
	// print vertices
	int iVertOffset = 1; // OBJ format has 1-based indices
	Polygon polygonOnTarget;
	for( int iTarget = 0; iTarget < numTargets; ++iTarget )
	{
		oss << "\n# On target " << iTarget+1 << ":\n";
		for( int iPiece = 0; iPiece < static_cast< int >( pieces.size() ); ++iPiece )
		{
			pieces[ iPiece ].getPolygonOnTarget( iTarget, polygonOnTarget );
			oss << "\n# Piece " << iPiece+1 << ":\n";
			for( int iPt = 0; iPt < static_cast< int >( polygonOnTarget.points.size() ); ++iPt )
				oss << "v " << polygonOnTarget.points[ iPt ].x << " " << polygonOnTarget.points[ iPt ].y << " 0\n";
			oss << "f";
			for( int iPt = 0; iPt < static_cast< int >( polygonOnTarget.points.size() ); ++iPt )
				oss << " " << iVertOffset + iPt;
			oss << "\n";
			iVertOffset += static_cast< int >( polygonOnTarget.points.size() );
		}
	}
	return oss.str();
}

string World::getAsJSONFormat() const // TODO: add to js code
{
	ostringstream oss;
	oss << "{\n\"N\":" << pieces.size() << ",\n";
	oss << "\"percent\" : " << getPercentCovered() << ",\n";
	oss << "\"circle\" : { \"center\" : { \"x\" : " << circleCenter.x << ", \"y\" : " << circleCenter.y << " }, \"radius\" : " << circleRadius << " },\n";
	oss << "\"square\" : { \"center\" : { \"x\" : " << squareCenter.x << ", \"y\" : " << squareCenter.y << " }, \"radius\" : " << squareRadius << " },\n";
	oss << "\"pieces\" : [\n";
	for( size_t iPiece = 0; iPiece < pieces.size()-1; ++iPiece )
		oss << pieces[iPiece].getAsJSONFormat() << ",\n";
	oss << pieces.back().getAsJSONFormat() << "\n";
	oss << "],\n";
	oss << "\"username\" : \"search\",\n";
	oss << "\"timestamp\" : \"2013.12.11\"\n}"; // TODO: insert timestamp!
	return oss.str();
}
