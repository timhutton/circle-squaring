// local:
class Point2D;
class Transform;

// STL:
#include <vector>
#include <string>

class Polygon
{
	public:

        enum PolygonRelation { disjoint, contained, containing, intersecting };

		float				getArea() const;
		Point2D				getCentroid() const;
		bool				contains( const Point2D& p ) const;
		bool				doesLineSegmentIntersect( const Point2D& p, const Point2D& p2 ) const;
		PolygonRelation		getPolygonRelationWith( const Polygon& poly ) const;
		void				getTransformed( const Transform& t, Polygon& output ) const;
		bool				isPointNearEdge( const Point2D& p, float tol ) const;
		Point2D				getNormal( int iPt ) const;
		bool				isSelfIntersecting() const;

		std::string			getAsJSONFormat() const;

		static Polygon		getCircle( const Point2D& center, float radius, int n );

		std::vector< Point2D > points;
};
