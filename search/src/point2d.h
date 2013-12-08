class Point2D
{
	public:

		Point2D( float x, float y );

		Point2D add( const Point2D& p );
		Point2D sub( const Point2D& p );
		Point2D dist( const Point2D& p );
		Point2D mul( float f );
		Point2D div( float f );

		float len() const;
		Point2D normalize();

	private:

		float x,y;
};
