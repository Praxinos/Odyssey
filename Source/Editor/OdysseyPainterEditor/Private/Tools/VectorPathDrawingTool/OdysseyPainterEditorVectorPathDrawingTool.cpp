// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Radius( 5.0f )
    , Absolute( true )
    , Stitch( true )
    , StitchingRadius( 10 )
    , mPathBuilder( nullptr )
    , mPreviousVertex( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD.SetRadius( Radius );
    mPathDrawingHUD.SetStitchingRadius( StitchingRadius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::ActivateVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( &mPathDrawingHUD );
}

static ::ULIS::FRectI
GetInvalidationAreaFromPointer( int iX, int iY, int iRadius ) 
{
    int diameter = iRadius * 2;

    return ::ULIS::FRectI( iX - iRadius, iY - iRadius, diameter, diameter );
}

static ::ULIS::FRectI
RectangleDtoI( ::ULIS::FRectD iRect )
{
    return ::ULIS::FRectI( (int) iRect.x, (int) iRect.y, (int) iRect.w, (int) iRect.h );
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorEngine* iVectorEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;
        FOdysseyVectorVertex* stitchCubicVertex = nullptr;

        pickedPointArray.reserve( 50 );

        iVectorEngine->PickPoints( iScene
                                 , iWorldX
                                 , iWorldY
                                 , iPickingRadius
                                 , pickedPointArray
                                 , FOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                return stitchCubicVertex;
            }
        }
    }

    return nullptr;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorVertex* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
    // take the upper value to prevent stroke with width 0.0
    float radius = iPointInTexture.pressure * Radius;
    FOdysseyVectorPathCubic* cubicPath = nullptr;
    BLPoint localCoords;
    BLPoint localRadius;

 //UE_LOG(LogTemp, Warning, TEXT("radius:%f iPointInTexture.pressure:%f"), radius, iPointInTexture.pressure ); 

    // record for later undoing
    mVertexArray.clear();
    mSegmentArray.clear();

    // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
    mStitched = true;

    if( cubicVertex )
    {
        cubicPath = static_cast<FOdysseyVectorPathCubic*>( cubicVertex->GetPath() );
    }
    else
    {
        if ( cubicPath == nullptr )
        {
            cubicPath = new FOdysseyVectorPathCubic();

            iScene->AppendChild( cubicPath );

            // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
            mStitched = false;
        }

        cubicVertex = new FOdysseyVectorVertex( cubicPath, 0.0f, 0.0f, 0.0f );

        cubicPath->AddVertex( cubicVertex );
        // record for undos
        mVertexArray.push_back( cubicVertex );
    }

    mPreviousVertex = cubicVertex;

    cubicPath->UpdateMatrix();
    cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

    localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    localRadius = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * radius, 0.7071f * radius );

    cubicVertex->Set( localCoords.x, localCoords.y );
    cubicVertex->SetRadius( ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );

    mPathBuilder = new FOdysseyVectorPathBuilder();

    iScene->AppendChild( mPathBuilder );

    mPathBuilder->Attach( cubicPath );
    cubicPath->CopyTransformation( *cubicPath );
    mPathBuilder->UpdateMatrix();
    mPathBuilder->SetForegroundColor( cubicPath->GetForegroundColor() );
    mPathBuilder->RecordStart( cubicVertex );

    iScene->ClearSelection();


    //mSelectionChanged.Broadcast(iScene);


    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
}

::ULIS::FRectI
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    mPointRadius =  iPointInTexture.pressure * Radius;

//UE_LOG(LogTemp, Warning, TEXT("Some warning message %f %f"), iPointInTexture.acceleration.X, iPointInTexture.acceleration.Y );

    if( mPathBuilder )
    {
        FOdysseyVectorPathCubic* cubicPath = mPathBuilder->GetCubicPath();
        BLPoint localCoords;
        BLPoint localRadius;
        //float roundedUpRadius = ceil (radius);
        FOdysseyVectorVertex* nextVertex;
        ::ULIS::FRectI redrawRegion = GetInvalidationAreaFromPointer( iPointInTexture.x
                                                                    , iPointInTexture.y
                                                                    , Radius )
                                    | RectangleDtoI( mPreviousVertex->GetBoundingBox( true ) );

        mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

        localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        localRadius = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );

        nextVertex = mPathBuilder->RecordIntermediate( localCoords.x
                                                     , localCoords.y
                                                     , ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance()
                                                     , mVertexArray
                                                     , mSegmentArray );

        iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

        mPreviousVertex = nextVertex;

        return redrawRegion;
    }

    return ::ULIS::FRectI( 0, 0, 0, 0 );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , FOdysseyVectorUndo** iUndo
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if( mPathBuilder )
    {
        FOdysseyVectorVertex* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
        FOdysseyVectorPathCubic* cubicPath = mPathBuilder->GetCubicPath();
        FOdysseyVectorSegmentCubic* lastSegment = nullptr;

        // in some conditions (maximizing the window), you can get a Up event without a Down event. Check cubicPath exists.
        if( cubicPath )
        {
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            // in MouseUp events, pressure equals 0. We then use the last pressure value recorded in the Drag event.
            BLPoint localRadius = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );
            /*float roundedUpRadius = *//*ceil (radius)*//*mPreviousVertex->GetRadius();*/

            // the picked cubic vertex must belong to the path we are working with
            if( cubicVertex )
            {
                if ( cubicVertex->GetPath() != cubicPath )
                {
                    cubicVertex = nullptr;
                }
            }

            if( cubicVertex == nullptr )
            {
                cubicVertex = new FOdysseyVectorVertex( cubicPath
                                                      , localCoords.x
                                                      , localCoords.y
                                                      , ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );
                // record for undos
                mVertexArray.push_back( cubicVertex );

                cubicPath->AddVertex( cubicVertex );
            }

            lastSegment = mPathBuilder->RecordEnd( cubicVertex );

            if( lastSegment )
            {
                // record for undos
                mSegmentArray.push_back( lastSegment );
            }

            iScene->Select( cubicPath );
        }

        iScene->RemoveChild( mPathBuilder );
        delete mPathBuilder;
        mPathBuilder = nullptr;

        // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
        if( iUndo && GUndo )
        {
            (*iUndo) = ( mStitched == true ) ? static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoPathAlter( iScene
                                                                                                                , mVertexArray
                                                                                                                , mSegmentArray )) 
                                             : static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, cubicPath ));

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
        }

        // Update objects marked as invalidated
        iScene->Update(0);
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChanged( const FName& iPropertyName )
{
    if ( iPropertyName == "Radius" )
        mPathDrawingHUD.SetRadius( Radius );

    if ( ( iPropertyName == "StitchingRadius" ) || ( iPropertyName == "Stitch" ) )
    {
        if( Stitch == true )
            mPathDrawingHUD.SetStitchingRadius( StitchingRadius );
        else
            mPathDrawingHUD.SetStitchingRadius( 0.0f );
    }
}
