// ======================================
// Transform class
function Transform( r, p ) {
    this.rotate = r;                             // radians
    this.translate = new Point2D( p.x, p.y );    // Point2D
};
// --------------------------------------
Transform.prototype.apply = function( p ) {
    return new Point2D( p.x * Math.cos( this.rotate ) - p.y * Math.sin( this.rotate ) + this.translate.x, 
                        p.x * Math.sin( this.rotate ) + p.y * Math.cos( this.rotate ) + this.translate.y );
};
// --------------------------------------
Transform.prototype.applyInverse = function( p ) {
    return new Point2D( ( p.x - this.translate.x ) * Math.cos( -this.rotate ) - ( p.y - this.translate.y ) * Math.sin( -this.rotate ), 
                        ( p.x - this.translate.x ) * Math.sin( -this.rotate ) + ( p.y - this.translate.y ) * Math.cos( -this.rotate ) );
};
