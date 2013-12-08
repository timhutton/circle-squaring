// utility functions
function crossProduct2D( v, w ) { return v.x*w.y - v.y*w.x; }
// --------------------------------------
function add( a, b ) { return new Point2D( a.x + b.x, a.y + b.y ); }
// --------------------------------------
function sub( a, b ) { return new Point2D( a.x - b.x, a.y - b.y ); }
// --------------------------------------
function sqr( x ) { return x * x }
// --------------------------------------
function dist2( v, w ) { return sqr(v.x - w.x) + sqr(v.y - w.y) }
// --------------------------------------
function distToSegmentSquared( p, v, w ) {
  var l2 = dist2(v, w);
  if (l2 == 0) return dist2(p, v);
  var t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
  if (t < 0) return dist2(p, v);
  if (t > 1) return dist2(p, w);
  return dist2(p, { x: v.x + t * (w.x - v.x),
                    y: v.y + t * (w.y - v.y) });
}
// --------------------------------------
function distToSegment( p, v, w ) { return Math.sqrt(distToSegmentSquared(p, v, w)); }
// --------------------------------------
function lineSegmentsIntersect( p, p2, q, q2 ) {
    // two line segments: [p,p2] and [q,q2] - do they intersect?
    // http://stackoverflow.com/a/565282/126823
    var r = sub(p2, p);
    var s = sub(q2, q);

    var uNumerator = crossProduct2D(sub(q, p), r);
    var denominator = crossProduct2D(r, s);

    if( uNumerator == 0 && denominator == 0 ) {
        // colinear, so do they overlap?
        return ((q.x - p.x < 0) != (q.x - p2.x < 0) != (q2.x - p.x < 0) != (q2.x - p2.x < 0)) ||
                ((q.y - p.y < 0) != (q.y - p2.y < 0) != (q2.y - p.y < 0) != (q2.y - p2.y < 0));
    }

    if( denominator == 0 ) {
        // lines are parallel
        return false;
    }

    var u = uNumerator / denominator;
    var t = crossProduct2D(sub(q, p), s) / denominator;

    return (t >= 0) && (t <= 1) && (u >= 0) && (u <= 1);
}
// --------------------------------------
function dot( a, b ) {
    return a.x * b.x + a.y * b.y;
}
// --------------------------------------
function angleBetweenTwoVectors( a, b ) {
    return Math.acos( dot( a, b ) / ( a.len() * b.len() ) );
}
