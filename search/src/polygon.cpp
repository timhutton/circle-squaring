// ======================================
// Polygon class
function Polygon() {
    this.points = new Array(); // a list of Point2D's
};
// --------------------------------------
Polygon.prototype.drawPath = function( ctx ) {
    if( this.points.length == 0 ) 
        return;
    ctx.beginPath();
    ctx.moveTo( this.points[0].x, this.points[0].y );
    for( var i = 1; i < this.points.length; ++i )
        ctx.lineTo( this.points[i].x, this.points[i].y );
    ctx.closePath();
};
// --------------------------------------
Polygon.prototype.getArea = function() {
    var area=0.0;
    var i, j = this.points.length-1;
    for( i = 0; i < this.points.length; ++i )
    {
        area += ( this.points[j].x + this.points[i].x ) * ( this.points[j].y - this.points[i].y ); 
        j = i;
    }
    return area * 0.5; 
};
// --------------------------------------
Polygon.prototype.getCentroid = function() {
    var centroid = new Point2D( 0, 0 );
    for( var i = 0; i < this.points.length; ++i )
        centroid.add( this.points[ i ] );
    return centroid.div( this.points.length ); 
};
// --------------------------------------
function getCircle( center, radius, n )
{
    var poly = new Polygon();
    for( var i = 0; i < n; ++i )
    {
        poly.points[i] = new Point2D( center.x + radius * Math.cos( i * 2.0 * Math.PI / n ), 
                                      center.y - radius * Math.sin( i * 2.0 * Math.PI / n ) );
    }
    return poly;
};
// --------------------------------------
Polygon.prototype.contains = function( p ) {
    var c = false;
    var i, j;
    var vertxi,vertyi,vertxj,vertyj;
    for( i = 0, j = this.points.length-1; i < this.points.length; j = i++ ) 
    {
        vertxi = this.points[i].x;
        vertyi = this.points[i].y;
        vertxj = this.points[j].x;
        vertyj = this.points[j].y;
        if ( ((vertyi>p.y) != (vertyj>p.y)) && (p.x < (vertxj-vertxi) * (p.y-vertyi) / (vertyj-vertyi) + vertxi) )
        {
            c = !c;
        }
    }
    return c;
};
// --------------------------------------
Polygon.prototype.doesLineSegmentIntersect = function( p, p2 ) {
    for( var i = 0; i < this.points.length; ++i )
    {
        var q = this.points[ i ];
        var q2 = this.points[ (i+1)%this.points.length ];
        if( lineSegmentsIntersect( p, p2, q, q2 ) )
            return true;
    }
    return false;
};
// --------------------------------------
var PolygonRelationEnum = Object.freeze ({ disjoint: {}, contained: {}, containing: {}, intersecting: {} });
// --------------------------------------
Polygon.prototype.getPolygonRelationWith = function( poly ) {
    for( var i = 0; i < this.points.length; ++i )
    {
        var p = this.points[ i ];
        var p2 = this.points[ (i+1)%this.points.length ];
        for( var j = 0; j < poly.points.length; ++j )
        {
            var q = poly.points[ j ];
            var q2 = poly.points[ (j+1)%poly.points.length ];
            if( lineSegmentsIntersect( p, p2, q, q2 ) )
                return PolygonRelationEnum.intersecting;
        }
    }
    if( poly.contains( this.points[0] ) )
        return PolygonRelationEnum.contained;
    if( this.contains( poly.points[0] ) )
        return PolygonRelationEnum.containing;
    return PolygonRelationEnum.disjoint;
};
// --------------------------------------
Polygon.prototype.getTransformed = function( t ) {
    var poly = new Polygon();
    for( var iPt = 0; iPt < this.points.length; ++iPt )
        poly.points[ iPt ] = t.apply( this.points[ iPt ] );
    return poly;
};
// --------------------------------------
Polygon.prototype.isPointNearEdge = function( p, tol ) {
    for( var iPt = 0; iPt < this.points.length; ++iPt )
        if( distToSegment( p, this.points[iPt], this.points[ (iPt+1)%this.points.length ] ) < tol )
            return true;
    return false;
};
// --------------------------------------
Polygon.prototype.getNormal = function( iPt ) {
    // return average of two neighboring edge normals
    var a = this.points[ (iPt+this.points.length-1)%this.points.length ];
    var b = this.points[ iPt ];
    var c = this.points[ (iPt+1)%this.points.length ];
    var e1 = sub( b, a );
    var en1 = new Point2D( -e1.y, e1.x ).normalize();
    var e2 = sub( c, b );
    var en2 = new Point2D( -e2.y, e2.x ).normalize();
    var n = add( en1, en2 ).normalize();
    return n;
};
// --------------------------------------
Polygon.prototype.isSelfIntersecting = function() {
    // does any line segment cross any other?
    for( var iPt = 0; iPt < this.points.length; ++iPt )
    {
        var iP1 = iPt;
        var iP2 = (iPt+1)%this.points.length;
        var p1 = this.points[ iP1 ];
        var p2 = this.points[ iP2 ];
        for( var iPt2 = 0; iPt2 < iPt-1; ++iPt2 )
        {
            var iQ1 = iPt2;
            var iQ2 = (iPt2+1)%this.points.length;
            if( iQ1 == iP2 || iQ2 == iP1 )
                continue; // neighboring edges, ignore
            var q1 = this.points[ iQ1 ];
            var q2 = this.points[ iQ2 ];
            if( lineSegmentsIntersect( p1, p2, q1, q2 ) )
                return true;
        }
    }
    return false;
};
