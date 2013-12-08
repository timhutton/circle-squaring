// ======================================
// World class
function World( context ) {
    this.isAutoEnabled = true;

    this.circleCenter = new Point2D( 250, 350 );
    this.circleRadius = 200.0;
    
    this.squareCenter = new Point2D( 650, 350 );
    this.squareRadius = this.circleRadius * Math.sqrt( Math.PI ) / 2.0;

    this.targets = new Array();
    
    this.targets[0] = getCircle( this.circleCenter, this.circleRadius, 100 );
    this.targets[1] = new Polygon();
    this.targets[1].points[0] = new Point2D( this.squareCenter.x - this.squareRadius, this.squareCenter.y - this.squareRadius );
    this.targets[1].points[1] = new Point2D( this.squareCenter.x - this.squareRadius, this.squareCenter.y + this.squareRadius );
    this.targets[1].points[2] = new Point2D( this.squareCenter.x + this.squareRadius, this.squareCenter.y + this.squareRadius );
    this.targets[1].points[3] = new Point2D( this.squareCenter.x + this.squareRadius, this.squareCenter.y - this.squareRadius );
    
    this.leaderboard = new Array();

    this.username = "anon" + Math.floor(Math.random()*9000+1000);
    
    this.pieces = new Array();
   
    // make some pieces
    var nPieces = Math.floor( Math.random()*3 ) + 4 ;
    var scatterRadius = this.circleRadius * 0.7;
    for( var iPiece = 0; iPiece < nPieces; ++iPiece ) {
        var piece = this.pieces[ iPiece ] = new Piece();
        piece.atOrigin = getCircle( new Point2D( 0, 0 ), Math.random()*20+20, Math.floor(Math.random()*4)+3 );
        for( var iTarget = 0; iTarget < this.targets.length; ++iTarget )
            piece.originToTarget[ iTarget ] = new Transform( Math.random()*2.0*Math.PI, add( this.targets[ iTarget ].getCentroid(), 
                                                             new Point2D( Math.random()*scatterRadius*2-scatterRadius,
                                                                          Math.random()*scatterRadius*2-scatterRadius ) ) );
    }
    
    this.buttons = new Array();
    
    var buttonX = 15;
    var buttonSep = 10;
    this.deletePieceButton = this.buttons[0] = new Button( "Delete piece", "delete_piece", buttonX, 20, context );
    buttonX += this.buttons[0].getWidth() + buttonSep;
    this.newPieceButton = this.buttons[1] = new Button( "New piece", "new_piece", buttonX, 20, context );
    buttonX += this.buttons[1].getWidth() + buttonSep;
    this.automaticToggleButton = this.buttons[2] = new Button("Go manual", "toggle_auto", buttonX, 20, context );
    buttonX += this.buttons[2].getWidth() + buttonSep;
    this.automaticToggleButton.isEnabled = true;
    this.submitSolutionButton = this.buttons[3] = new Button("Submit solution", "submit_solution", buttonX, 20, context );
    buttonX = 15;
    this.clonePieceButton = this.buttons[4] = new Button("Clone piece", "clone_piece", buttonX, 60, context );
    buttonX += this.buttons[4].getWidth() + buttonSep;
    this.subdividePieceButton = this.buttons[5] = new Button("Subdivide piece", "subdivide_piece", buttonX, 60, context );
    buttonX += this.buttons[5].getWidth() + buttonSep;
    this.simplifyPieceButton = this.buttons[6] = new Button("Simplify piece", "simplify_piece", buttonX, 60, context );

    for( var N = 1; N <= 10; ++N ) {
        this.buttons.push( new Button( "N="+N+" : unknown", "leaderboard", 900, 70+N*40, context ) );
        this.buttons[ this.buttons.length-1 ].setWidth( 200 );
        if( N==1 )
            this.iFirstLeaderboardButton = this.buttons.length-1;
    }

    // we keep track of new results as they come in
    this.updates = new Array();
};
// --------------------------------------
World.prototype.draw = function( canvas ) {
    var context = canvas.getContext( "2d" );
    var isValidSolution = this.isValidSolution();
    
    // draw the background
    context.fillStyle = "rgb(200,200,200)";
    context.beginPath();
    context.fillRect( 0, 0, canvas.width, canvas.height );
    context.closePath();
    
    // update the buttons
    this.simplifyPieceButton.isEnabled = this.subdividePieceButton.isEnabled = this.clonePieceButton.isEnabled = this.deletePieceButton.isEnabled = this.isAnyPieceSelected();
    this.newPieceButton.isEnabled = !this.isAutoEnabled;
    var isCurrentBest = false;
    if( this.pieces.length <1 || this.pieces.length >10 )
        isCurrentBest = false;
    else if( isValidSolution && this.leaderboard[ this.pieces.length ] && this.getPercentCovered() > this.leaderboard[ this.pieces.length ].percent+0.01 )
        isCurrentBest = true;
    else if( isValidSolution && typeof this.leaderboard[ this.pieces.length ] == "undefined" )
        isCurrentBest = true;
    this.submitSolutionButton.isEnabled = (this.updates.length > 0) && isCurrentBest;
    
    // draw the buttons
    for( var iButton = 0; iButton < this.buttons.length; ++iButton )
        this.buttons[ iButton ].draw( context );
        
    // draw the targets
    context.fillStyle = "rgb(255,255,255)";
    for( var i = 0; i < this.targets.length; ++i ) {
        this.targets[i].drawPath( context );
        context.fill();
    }
        
    // draw the pieces (in reverse order so that the transparency appearance matches the selection behavior)
    for( var iPiece = this.pieces.length - 1; iPiece >= 0; --iPiece )
        this.pieces[ iPiece ].draw( context );
    
    // show some information about the current pieces
    context.fillStyle = "rgb(0,0,0)";
    context.font="12px Arial";
    var percentageCoverage = this.getPercentCovered();
    context.fillText( 'N = ' + this.pieces.length + " : " + percentageCoverage.toFixed(2) + "% coverage", 10, 100 );
    if( isValidSolution )
        context.fillText( "valid solution (no overlapping)", 10, 130 );
    else
        context.fillText( "overlapping detected!", 10, 130 );
        
    // show the last few updates that have been happening
    var numUpdatesToShow = Math.min( 5, this.updates.length );
    var timeNow = new Date();
    for( var i = 0; i < numUpdatesToShow; ++i ) {
        var update = this.updates[ i ]
        var msPerMinute = 1000*60;
        var minuteDifference = ( timeNow.getTime() - update.time.getTime() ) / msPerMinute
        var hourDifference = minuteDifference/60;
        var dayDifference = hourDifference/24;
        var yearDifference = dayDifference/365;
        var timeDiff = "";
        if( minuteDifference < 60 )
            timeDiff = minuteDifference.toFixed(0) + " minute(s) ago: ";
        else if( hourDifference < 24 )
            timeDiff = hourDifference.toFixed(0) + " hour(s) ago: ";
        else if( dayDifference < 365 )
            timeDiff = dayDifference.toFixed(0) + " day(s) ago: ";
        else
            timeDiff = yearDifference.toFixed(0) + " year(s) ago: ";
        context.fillText( timeDiff + update.text, 10, 700 - 20*i );
    }
    if( this.updates.length > 0 )
        context.fillText( "Most recent of " + this.updates.length + " events:", 10, 700 - 20*numUpdatesToShow );
    else
        context.fillText( "Loading leaderboard...", 10, 700 - 20*numUpdatesToShow );
};
// --------------------------------------
World.prototype.isValidSolution = function() {
    var polysOnTarget = new Array();
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece ) {
        // polygon must not be self-intersecting
        if( this.pieces[ iPiece ].atOrigin.isSelfIntersecting() )
            return false;
        // polygon must be clockwise (positive area)
        if( this.pieces[ iPiece ].atOrigin.getArea() <= 0 )
            return false;
    }
    for( var iTarget = 0; iTarget < this.targets.length; ++iTarget ) {
        for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece ) {
            // transform each piece onto this target
            polysOnTarget[ iPiece ] = this.pieces[ iPiece ].getPolygonOnTarget( iTarget );
            // invalid if any piece not fully contained in each target
            if( polysOnTarget[ iPiece ].getPolygonRelationWith( this.targets[ iTarget ] ) != PolygonRelationEnum.contained ) 
                return false;
            // invalid if any piece not fully outside any other
            for( var iPiece2 = 0; iPiece2 < iPiece; ++iPiece2 ) 
                if( polysOnTarget[ iPiece ].getPolygonRelationWith( polysOnTarget[ iPiece2 ] ) != PolygonRelationEnum.disjoint ) 
                    return false;
        }
    }
    return true;
};
// --------------------------------------
World.prototype.getPercentCovered = function() {
    // assumes no overlapping
    var area = 0.0;
    for( var i = 0; i < this.pieces.length; ++i )
        area += this.pieces[i].atOrigin.getArea();
    return 100.0 * area / ( Math.PI * this.circleRadius * this.circleRadius );
};
// --------------------------------------
World.prototype.addNewPiece = function() {
    var p = new Piece();
    p.atOrigin = getCircle( new Point2D( 0, 0 ), 30, 6 );
    for( var iTarget = 0; iTarget < this.targets.length; ++iTarget )
        p.originToTarget[ iTarget ] = new Transform( Math.random() * 2.0 * Math.PI, this.targets[ iTarget ].getCentroid() );
    this.pieces.unshift( p ); // add to start of list, so it appears on top
};
// --------------------------------------
World.prototype.deleteSelectedPiece = function() {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        if( this.pieces[ iPiece ].isSelected() )
            this.pieces.splice( iPiece, 1 );
};
// --------------------------------------
World.prototype.cloneSelectedPiece = function() {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        if( this.pieces[ iPiece ].isSelected() )
            var sourcePiece = this.pieces[ iPiece ];
    var p = new Piece();
    for( var iPt = 0; iPt < sourcePiece.atOrigin.points.length; ++iPt )
        p.atOrigin.points[ iPt ] = new Point2D( sourcePiece.atOrigin.points[ iPt ].x, sourcePiece.atOrigin.points[ iPt ].y );
    for( var iTarget = 0; iTarget < sourcePiece.originToTarget.length; ++iTarget )
        p.originToTarget[ iTarget ] = new Transform( sourcePiece.originToTarget[ iTarget ].rotate, sourcePiece.originToTarget[ iTarget ].translate );
    sourcePiece.state = PieceStateEnum.inactive;
    this.pieces.unshift( p ); // add to start of list, so it appears on top
};
// --------------------------------------
World.prototype.clipPiecesAgainstTargets = function() {
    for( var iPiece = this.pieces.length-1; iPiece >=0; --iPiece ) 
    {
        var piece = this.pieces[ iPiece ];
        var pieceDeleted = false;
        for( var iTarget = 0; iTarget < piece.originToTarget.length; ++iTarget ) 
        {
            var polygonOnTarget = piece.getPolygonOnTarget( iTarget );
            for( iPt = polygonOnTarget.points.length-1; iPt >= 0; --iPt ) 
            {
                if( !this.targets[ iTarget ].contains( polygonOnTarget.points[ iPt ] ) )
                {
                    piece.removeVertex( iPt );
                    if( piece.atOrigin.points.length < 3 )
                    {
                        this.pieces.splice( iPiece, 1 );
                        pieceDeleted = true;
                        break;
                    }
                }
            }
            if( pieceDeleted )
                break;
        }
    }
};
// --------------------------------------
World.prototype.clipPiecesAgainstOthers = function() {
    for( var iPiece = this.pieces.length-1; iPiece >=0; --iPiece ) 
    {
        var piece = this.pieces[ iPiece ];
        if( piece.isSelected() )
            continue; // don't clip the piece that is currently selected (feels more natural when changing it in auto mode)
        var pieceDeleted = false;
        for( var iTarget = 0; iTarget < this.targets.length; ++iTarget )
        {
            var polygonOnTarget = piece.getPolygonOnTarget( iTarget );
            for( var iOtherPiece = 0; iOtherPiece < this.pieces.length; ++iOtherPiece ) 
            {
                if( iOtherPiece == iPiece )
                    continue;
                var otherPolygonOnTarget = this.pieces[ iOtherPiece ].getPolygonOnTarget( iTarget );
                for( iPt = polygonOnTarget.points.length-1; iPt >= 0; --iPt ) 
                {
                    // check that the point isn't inside the other piece
                    // and that its edges don't intersect it
                    var pLeft = polygonOnTarget.points[ (iPt+polygonOnTarget.points.length-1)%polygonOnTarget.points.length ];
                    var pHere = polygonOnTarget.points[ iPt ];
                    var pRight = polygonOnTarget.points[ (iPt+1)%polygonOnTarget.points.length ];
                    if( otherPolygonOnTarget.contains( pHere ) ||
                        otherPolygonOnTarget.doesLineSegmentIntersect( pLeft, pHere ) || 
                        otherPolygonOnTarget.doesLineSegmentIntersect( pHere, pRight ) )
                    {
                        piece.removeVertex( iPt );
                        if( piece.atOrigin.points.length < 3 )
                        {
                            this.pieces.splice( iPiece, 1 );
                            pieceDeleted = true;
                            break;
                        }
                    }
                }
                if( pieceDeleted )
                    break;
            }
            if( pieceDeleted )
                break;
        }
    }
};
// --------------------------------------
World.prototype.adaptPieces = function( growStep ) {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        this.pieces[ iPiece ].removeSelfIntersectingVertices();
    this.clipPiecesAgainstTargets();
    this.clipPiecesAgainstOthers();
    var somethingChanged = false;
    // move vertices outwards a little on their surface normal for as long as solution remains valid
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece ) 
    {
        var piece = this.pieces[ iPiece ];
        piece.subdivide( 20 );
        piece.removeVerticesCloserThan( 3 );
        for( var iPt = 0; iPt < piece.atOrigin.points.length; ++iPt ) 
        {
            var oldPos = new Point2D( piece.atOrigin.points[ iPt ].x, piece.atOrigin.points[ iPt ].y );
            var move = piece.atOrigin.getNormal( iPt ).mul( growStep );
            piece.atOrigin.points[ iPt ].add( move );
            var newPosIsOK = true;
            if( newPosIsOK )
            {
                for( var iTarget = 0; iTarget < piece.originToTarget.length; ++iTarget ) {
                    var pOnTarget = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ iPt ] );
                    // check whether the point is still inside the target shape
                    if( !this.targets[ iTarget ].contains( pOnTarget ) ) {
                        newPosIsOK = false;
                        break;
                    }
                    // check whether the point doesn't intersect any other shape
                    for( var iOtherPiece = 0; iOtherPiece < this.pieces.length; ++iOtherPiece ) {
                        if( iOtherPiece == iPiece )
                            continue;
                        var otherPolygonOnTarget = this.pieces[ iOtherPiece ].getPolygonOnTarget( iTarget );
                        var left = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ (iPt+piece.atOrigin.points.length-1)%piece.atOrigin.points.length ] );
                        var right = piece.originToTarget[ iTarget ].apply( piece.atOrigin.points[ (iPt+1)%piece.atOrigin.points.length ] );
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
};
// --------------------------------------
World.prototype.subdivideSelectedPiece = function() {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        if( this.pieces[ iPiece ].isSelected() )
            this.pieces[ iPiece ].subdivide( 10 );
};
// --------------------------------------
World.prototype.simplifySelectedPiece = function() {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        if( this.pieces[ iPiece ].isSelected() )
            this.pieces[ iPiece ].removeVerticesStraighterThan( 0.1 );
};
// --------------------------------------
World.prototype.isAnyPieceSelected = function() {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        if( this.pieces[ iPiece ].isSelected() )
            return true;
    return false;
};
// --------------------------------------
World.prototype.onMouseMove = function( evt ) {
    // pass the message to the pieces
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        this.pieces[ iPiece ].onMouseMove( evt );
};
// --------------------------------------
World.prototype.onMouseLeftDown = function( evt ) {
    // was a button pressed?
    for( var iButton = 0; iButton < this.buttons.length; ++iButton ) {
        if( this.buttons[ iButton ].onMouseLeftDown( evt ) ) {
            this.doButtonAction( this.buttons[ iButton ].id, this.buttons[ iButton ].label );
            return;
        }
    }
    // send the message to the pieces
    var iPieceSelected = -1;
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece ) {
        if( this.pieces[ iPiece ].onMouseLeftDown( evt ) ) {
            iPieceSelected = iPiece;
            break; // we only allow one piece to accept the event
        }
    }
    // deselect everything other than the one that accepted the message
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece ) {
        if( iPiece != iPieceSelected )
            this.pieces[ iPiece ].state = PieceStateEnum.inactive;
    }
};
// --------------------------------------
World.prototype.onMouseLeftUp = function( evt ) {
    // tell the buttons
    for( var iButton = 0; iButton < this.buttons.length; ++iButton )
        this.buttons[ iButton ].onMouseLeftUp( evt );
};
// --------------------------------------
World.prototype.onMouseLeftDrag = function( evt ) {
    for( var iPiece = 0; iPiece < this.pieces.length; ++iPiece )
        this.pieces[ iPiece ].onMouseLeftDrag( evt );
};
// --------------------------------------
World.prototype.getSolutionData = function() {
    return {
                "N" : this.pieces.length,
                "percent" : this.getPercentCovered(),
                "circle" : { "center" : this.circleCenter, "radius" : this.circleRadius }, 
                "square" : { "center" : this.squareCenter, "radius" : this.squareRadius }, 
                "pieces" : this.pieces,
                "username" : this.username,
                "timestamp" : (new Date()).toJSON()
            };
};
// --------------------------------------
World.prototype.setSolutionData = function( solution ) {
    // adopt this solution for editing
    this.pieces = new Array();
    for( var iPiece = 0; iPiece < solution.pieces.length; ++iPiece ) {
        var piece = this.pieces[ iPiece ] = new Piece();
        for( var iPt = 0; iPt < solution.pieces[ iPiece].atOrigin.points.length; ++iPt )
            piece.atOrigin.points[ iPt ] = new Point2D( solution.pieces[ iPiece].atOrigin.points[ iPt ].x, solution.pieces[ iPiece].atOrigin.points[ iPt ].y );
        for( var iTarget = 0; iTarget < 2; ++iTarget )
            piece.originToTarget[ iTarget ] = new Transform( solution.pieces[ iPiece ].originToTarget[ iTarget ].rotate, solution.pieces[ iPiece ].originToTarget[ iTarget ].translate );
    }
    // (assuming for now that targets are the same)
};
// --------------------------------------
World.prototype.onRemoteSolutionAdded = function( solution ) {
    // reject entries we're not interested in
    if( !solution.N || solution.N < 1 || solution.N > 10 )
        return;
    // keep track of the winners on the leaderboard
    if( !this.leaderboard[ solution.N ] || solution.percent > this.leaderboard[ solution.N ].percent ) {
        this.leaderboard[ solution.N ] = solution;
        var iButton = this.iFirstLeaderboardButton + solution.N - 1;
        this.buttons[ iButton ].label = "N="+solution.N+" : "+this.leaderboard[ solution.N ].percent.toFixed(2)+"% ("+this.leaderboard[ solution.N ].username+")";
        this.buttons[ iButton ].isEnabled = true;
    }
    // add all the entries to the updates list
    this.updates.push( 
        { 
            time: new Date( solution.timestamp ), 
            text: "New record for N=" + solution.N + ": " + solution.percent.toFixed(2) + "% found by " + solution.username
        } );
    this.updates.sort( function( a, b ) { return b.time.getTime() - a.time.getTime(); } );
    // TODO: remove ones that weren't records (submitted before getting a server response about the leaderboard)
};
// --------------------------------------
World.prototype.onSubmitSolution = function() {
    this.username = prompt( "Enter a username:", this.username );
    if( this.username == null )
        return; // user cancelled when asked for username
    var dataRef = new Firebase("https://circle-squaring.firebaseio.com/solutions"); 
    dataRef.push( this.getSolutionData() ); 
};
// --------------------------------------
World.prototype.doButtonAction = function( id, label ) {
    switch( id ) {
        case "delete_piece":        this.deleteSelectedPiece(); break;
        case "clone_piece":         this.cloneSelectedPiece(); break;
        case "subdivide_piece":     this.subdivideSelectedPiece(); break;
        case "simplify_piece":      this.simplifySelectedPiece(); break;
        case "new_piece":           this.addNewPiece(); break;
        case "toggle_auto":         this.isAutoEnabled = !this.isAutoEnabled; 
                                    if( this.isAutoEnabled )
                                        this.automaticToggleButton.label = "Go manual"; 
                                    else
                                        this.automaticToggleButton.label = "Go auto"; 
                                    break;
        case "submit_solution":     this.onSubmitSolution(); break;
        case "leaderboard":         this.setSolutionData( this.leaderboard[ parseInt( label.substring(2) ) ] ); // leaderboard buttons are labelled e.g. "N=3 : ..."
                                    // TODO: should ask if OK to overwrite current work
                                    break;
        default: alert("Unknown button: "+label);
    }
};
