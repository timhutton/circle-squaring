// ======================================
// Piece class
// --------------------------------------
var PieceStateEnum = Object.freeze ({ inactive: {}, highlighted: {}, selected: {}, translating: {}, vertexDragging: {}, rotating: {} });
// --------------------------------------
function Piece() {
    this.atOrigin = new Polygon();              // canonical polygon with centroid at the origin
    this.originToTarget = new Array();          // a Transform for each target
    this.state = PieceStateEnum.inactive;
    this.iActiveVertex = -1;
    this.whichTargetSelected = -1;
    this.draggingPoint = null;
    this.hue = Math.random() * 360.0;
    this.vertexRadius = 5;
};
// --------------------------------------
Piece.prototype.isSelected = function() {
    return this.state != PieceStateEnum.inactive && this.state != PieceStateEnum.highlighted;
}
// --------------------------------------
Piece.prototype.draw = function( ctx ) {
    // draw the pieces on each target
    ctx.strokeStyle = "rgb(0,0,0)";
    ctx.lineWidth = 1;
    for(var iTarget = 0; iTarget < this.originToTarget.length; ++iTarget ) 
    {
        ctx.save();
        if( this.state != PieceStateEnum.inactive )
            ctx.fillStyle = "hsla("+this.hue+",100%,50%,0.8)";
        else
            ctx.fillStyle = "hsla("+this.hue+",80%,50%,0.3)";
        ctx.translate( this.originToTarget[ iTarget ].translate.x, this.originToTarget[ iTarget ].translate.y );
        ctx.rotate( this.originToTarget[ iTarget ].rotate );
        this.atOrigin.drawPath( ctx );
        ctx.fill();
        if( this.isSelected() ) 
        {
            ctx.stroke();
            // draw vertex handles
            ctx.fillStyle = "rgba(0,0,0,0.4)";
            for( var iVert = 0; iVert < this.atOrigin.points.length; ++iVert ) {
                ctx.beginPath();
                ctx.arc( this.atOrigin.points[ iVert ].x, this.atOrigin.points[ iVert ].y, this.vertexRadius, 0, 2*Math.PI );
                ctx.fill();
                if( this.state == PieceStateEnum.vertexDragging && this.iActiveVertex == iVert )
                    ctx.stroke();
                ctx.closePath();
            }
        }
        ctx.restore();
    }
    // draw the cursor
    if( this.state == PieceStateEnum.rotating ) {
        // draw a broken circle to show that piece can be rotated like this
        ctx.strokeStyle = "rgba(0,0,0,0.2)";
        ctx.lineWidth = this.vertexRadius;
        var arcRadius = this.vertexRadius*2;
        ctx.beginPath();
        var pOnTarget = this.originToTarget[ this.whichTargetSelected ].apply( this.draggingPoint );
        ctx.arc( pOnTarget.x, pOnTarget.y, arcRadius, 0, 1.8 * Math.PI );
        ctx.stroke();
        ctx.closePath();
        // also draw the center of rotation
        var center = this.originToTarget[ this.whichTargetSelected ].apply( new Point2D( 0, 0 ) );
        ctx.strokeStyle = "rgba(0,0,0,1)";
        ctx.lineWidth = 1;
        var crossWidth = this.vertexRadius/2;
        ctx.beginPath();
        ctx.moveTo( center.x - crossWidth, center.y )
        ctx.lineTo( center.x + crossWidth, center.y )
        ctx.moveTo( center.x, center.y - crossWidth )
        ctx.lineTo( center.x, center.y + crossWidth )
        ctx.stroke();
        ctx.closePath();
    }
    else if( this.state == PieceStateEnum.translating || this.state == PieceStateEnum.highlighted ) {
        // draw a cross to show that piece can be moved like this
        ctx.strokeStyle = "rgba(0,0,0,0.2)";
        ctx.lineWidth = this.vertexRadius;
        var crossWidth = this.vertexRadius*2;
        ctx.beginPath();
        ctx.moveTo( this.draggingPoint.x - crossWidth, this.draggingPoint.y )
        ctx.lineTo( this.draggingPoint.x + crossWidth, this.draggingPoint.y )
        ctx.moveTo( this.draggingPoint.x, this.draggingPoint.y - crossWidth )
        ctx.lineTo( this.draggingPoint.x, this.draggingPoint.y + crossWidth )
        ctx.stroke();
        ctx.closePath();
    }
};
// --------------------------------------
Piece.prototype.getPolygonOnTarget = function( iTarget ) {
    return this.atOrigin.getTransformed( this.originToTarget[ iTarget ] ); // TODO: cache this, only update when needed
};
// --------------------------------------
Piece.prototype.recenter = function() {
    var center = this.atOrigin.getCentroid();
    for( var iPt = 0; iPt < this.atOrigin.points.length; ++iPt )
        this.atOrigin.points[ iPt ].sub( center );
    for( var iTarget = 0; iTarget < this.originToTarget.length; ++iTarget )
        this.originToTarget[ iTarget ].translate = this.originToTarget[ iTarget ].apply( center );
};
// --------------------------------------
Piece.prototype.onMouseMove = function( evt ) {
    var mousePos = new Point2D( evt.clientX, evt.clientY );
    // selected -> selected, translating, rotating or vertexDragging
    if( this.isSelected() )
    {
        for( var iTarget = 0; iTarget < this.originToTarget.length; ++iTarget ) 
        {
            var pAtOrigin = this.originToTarget[ iTarget ].applyInverse( mousePos );
        
            // in position for vertex dragging?
            for( var iVert = 0; iVert < this.atOrigin.points.length; ++iVert ) {
                if( pAtOrigin.dist( this.atOrigin.points[ iVert ] ) < this.vertexRadius ) {
                    this.state = PieceStateEnum.vertexDragging;
                    this.iActiveVertex = iVert;
                    this.draggingPoint = pAtOrigin;
                    this.whichTargetSelected = iTarget;
                    return;
                }
            }
            // in position for rotating the piece?
            if( this.atOrigin.isPointNearEdge( pAtOrigin, this.vertexRadius*2 ) ) {
                this.state = PieceStateEnum.rotating;
                this.draggingPoint = pAtOrigin;
                this.whichTargetSelected = iTarget;
                return;
            }
            // in position for translating the piece?
            if( this.atOrigin.contains( pAtOrigin ) ) {
                this.state = PieceStateEnum.translating;
                this.draggingPoint = mousePos;
                this.whichTargetSelected = iTarget;
                return;
            }
        }
        this.state = PieceStateEnum.selected;
        this.whichTargetSelected = -1;
        return;
    }
    // inactive or highlighted
    for( var iTarget = 0; iTarget < this.originToTarget.length; ++iTarget )
    {
        var pAtOrigin = this.originToTarget[ iTarget ].applyInverse( mousePos );
        if( this.atOrigin.contains( pAtOrigin ) )
        {
            this.state = PieceStateEnum.highlighted;
            this.draggingPoint = mousePos;
            this.whichTargetSelected = iTarget;
            return;
        }
    }
    this.state = PieceStateEnum.inactive;
    this.whichTargetSelected = -1;
};
// --------------------------------------
Piece.prototype.onMouseLeftDown = function( evt ) {
    switch( this.state )
    {
        case PieceStateEnum.selected:
            this.state = PieceStateEnum.inactive;
            return false;
            
        case PieceStateEnum.highlighted:
            this.state = PieceStateEnum.translating;
            return true; // no other piece should get this mouse click
            
        case PieceStateEnum.inactive:
            return false;
            
        default:
            return true; // translating/rotating/vertexDraggging is happening, no other piece should get this mouse click
    }
};
// --------------------------------------
Piece.prototype.onMouseLeftDrag = function( evt ) {
    var mousePos = new Point2D( evt.clientX, evt.clientY );
    switch( this.state ) 
    {
        case PieceStateEnum.vertexDragging:
            var pAtOrigin = this.originToTarget[ this.whichTargetSelected ].applyInverse( mousePos );
            this.atOrigin.points[ this.iActiveVertex ].add( sub( pAtOrigin, this.draggingPoint ) );
            this.recenter();
            this.draggingPoint = this.originToTarget[ this.whichTargetSelected ].applyInverse( mousePos ); // need to recompute because transform has changed
            break;
            
        case PieceStateEnum.translating:
            this.originToTarget[ this.whichTargetSelected ].translate.add( sub( mousePos, this.draggingPoint ) );
            this.draggingPoint = mousePos;
            break;
            
        case PieceStateEnum.rotating:
            var pAtOrigin = this.originToTarget[ this.whichTargetSelected].applyInverse( mousePos );
            this.originToTarget[ this.whichTargetSelected ].rotate += Math.atan2( pAtOrigin.y, pAtOrigin.x ) - Math.atan2( this.draggingPoint.y, this.draggingPoint.x );
            this.draggingPoint = this.originToTarget[ this.whichTargetSelected].applyInverse( mousePos ); // need to recompute because transform has changed
            break;
    }
};
// --------------------------------------
Piece.prototype.removeVertex = function( iPt ) {
    this.atOrigin.points.splice( iPt, 1 );
    if( this.state == PieceStateEnum.vertexDragging ) {
        if( this.iActiveVertex == iPt ) {
            this.state = PieceStateEnum.selected;
            this.iActiveVertex = -1;
        }
        else if( this.iActiveVertex > iPt ) {
            this.iActiveVertex--;
        }
    }
};
// --------------------------------------
Piece.prototype.subdivide = function( minEdgeLength ) {
    var newPoints = new Array();
    var newActiveVertex = -1;
    for( var iOldPt = 0; iOldPt < this.atOrigin.points.length; ++iOldPt )
    {
        newPoints.push( this.atOrigin.points[ iOldPt ] );
        if( this.state == PieceStateEnum.vertexDragging && this.iActiveVertex == iOldPt )
            newActiveVertex = newPoints.length-1;
        if( this.atOrigin.points[ iOldPt ].dist( this.atOrigin.points[ (iOldPt+1)%this.atOrigin.points.length ] ) > minEdgeLength )
            newPoints.push( add( this.atOrigin.points[ iOldPt ], this.atOrigin.points[ (iOldPt+1)%this.atOrigin.points.length ] ).div( 2.0 ) );
    }
    if( this.state == PieceStateEnum.vertexDragging )
        this.iActiveVertex = newActiveVertex;
    this.atOrigin.points = newPoints;
};
// --------------------------------------
Piece.prototype.removeVerticesCloserThan = function( minEdgeLength ) {
    do {
        var removedOne = false;
        for( var iPt = 0; iPt < this.atOrigin.points.length; ++iPt )
        {
            if( this.atOrigin.points[ iPt ].dist( this.atOrigin.points[ (iPt+1)%this.atOrigin.points.length ] ) < minEdgeLength ) {
                this.removeVertex( iPt );
                removedOne = true;
                break;
            }
        }
    } while( removedOne );
};
// --------------------------------------
Piece.prototype.removeVerticesStraighterThan = function( minAngle ) {
    do {
        var removedOne = false;
        for( var iPt = 0; iPt < this.atOrigin.points.length; ++iPt )
        {
            var edge1 = sub( this.atOrigin.points[ iPt ], this.atOrigin.points[ (iPt+1)%this.atOrigin.points.length ] );
            var edge2 = sub( this.atOrigin.points[ (iPt+this.atOrigin.points.length-1)%this.atOrigin.points.length ], this.atOrigin.points[ iPt ]  );
            if( angleBetweenTwoVectors( edge1, edge2 ) < minAngle ) {
                this.removeVertex( iPt );
                removedOne = true;
                break;
            }
        }
    } while( removedOne );
};
// --------------------------------------
Piece.prototype.removeSelfIntersectingVertices = function() {
    // does any line segment cross any other?
    for( var iPt = this.atOrigin.points.length-1; iPt >= 0; --iPt )
    {
        var iP1 = iPt;
        var iP2 = (iPt+1)%this.atOrigin.points.length;
        var p1 = this.atOrigin.points[ iP1 ];
        var p2 = this.atOrigin.points[ iP2 ];
        for( var iPt2 = 0; iPt2 < iPt-1; ++iPt2 )
        {
            var iQ1 = iPt2;
            var iQ2 = (iPt2+1)%this.atOrigin.points.length;
            if( iQ1 == iP2 || iQ2 == iP1 )
                continue; // neighboring edges, ignore
            var q1 = this.atOrigin.points[ iQ1 ];
            var q2 = this.atOrigin.points[ iQ2 ];
            if( lineSegmentsIntersect( p1, p2, q1, q2 ) ) {
                this.removeVertex( iPt );
                break;
            }
        }
    }
};
