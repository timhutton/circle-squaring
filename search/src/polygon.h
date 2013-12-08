#include "point2d.h"
#include "transform.h"

#include <vector>

class Polygon
{
	public:

        enum PolygonRelation { disjoint, contained, containing, intersecting };

		Polygon();

		float				getArea() const;
		Point2D				getCentroid() const;
		bool				contains( const Point2D& p ) const;
		bool				doesLineSegmentIntersect( const Point2D& p, const Point2D& p2 ) const;
		PolygonRelation		getPolygonRelationWith( const Polygon& poly ) const;
		void				getTransformed( const Transform& t, Polygon& output ) const;
		bool				isPointNearEdge( const Point2D& p, float tol ) const;
		void				getNormal( int iPt, Point2D& output ) const;
		bool				isSelfIntersecting() const;

		static void			getCircle( const Point2D& center, float radius, int n );

	private:

		std::vector< Point2D > points;
};
