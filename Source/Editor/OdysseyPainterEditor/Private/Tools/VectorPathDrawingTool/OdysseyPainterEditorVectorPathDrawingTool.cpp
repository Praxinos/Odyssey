// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Radius( 5.0f )
    , Stitch( true )
    , StitchingRadius( 10 )
    , Absolute( true )
    , mPreviousVertex ( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD.SetRadius( Radius );
    mPathDrawingHUD.SetStitchingRadius( StitchingRadius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
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

FOdysseyVectorVertexCubic*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorEngine* iVectorEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;
        FOdysseyVectorVertexCubic* stitchCubicVertex = nullptr;

        pickedPointArray.reserve( 50 );

        iVectorEngine->PickPoints( iScene
                                 , iWorldX
                                 , iWorldY
                                 , iPickingRadius
                                 , pickedPointArray
                                 , FOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertexCubic*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                return stitchCubicVertex;
            }
        }
    }

    return nullptr;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    FOdysseyVectorPathBuilder* pathBuilder = new FOdysseyVectorPathBuilder();
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorVertexCubic* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
    // take the upper value to prevent stroke with width 0.0
    float radius = iPointInTexture.pressure * Radius;
    FOdysseyVectorPathCubic* cubicPath = nullptr;
    BLPoint localCoords;

    if( cubicVertex )
    {
        cubicPath = static_cast<FOdysseyVectorPathCubic*>( cubicVertex->GetPath() );
    }
    else
    {
        cubicVertex = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 0.0f );
    }

    mPreviousVertex = cubicVertex;

    if ( cubicPath == nullptr )
    {
        cubicPath = new FOdysseyVectorPathCubic();

        iScene->AppendChild( cubicPath );

        cubicPath->AddVertex( cubicVertex );
        cubicPath->UpdateMatrix();
        cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    }

    localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

    cubicVertex->Set( localCoords.x, localCoords.y );
    cubicVertex->SetRadius( radius );

    iScene->AppendChild( pathBuilder );

    pathBuilder->Attach( cubicPath );
    pathBuilder->SetForegroundColor( cubicPath->GetForegroundColor() );
    pathBuilder->UpdateMatrix();

    pathBuilder->RecordStart( cubicVertex );

    iScene->ClearSelection();
    iScene->Select( pathBuilder );

    //mSelectionChanged.Broadcast(iScene);


    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
}

::ULIS::FRectI
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorPathBuilder* currentPathBuilder = static_cast<FOdysseyVectorPathBuilder*>( iScene->GetLastSelected() );
    FOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
    BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    float radius =  iPointInTexture.pressure * Radius;
    float roundedUpRadius = ceil (radius);
    FOdysseyVectorVertexCubic* nextVertex;
    ::ULIS::FRectI redrawRegion = GetInvalidationAreaFromPointer( iPointInTexture.x
                                                                , iPointInTexture.y
                                                                , Radius )
                                | RectangleDtoI( mPreviousVertex->GetBoundingBox( true ) );

    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    nextVertex = currentPathBuilder->RecordIntermediate( localCoords.x, localCoords.y, radius );

    iScene->Update( 0 );

    mPreviousVertex = nextVertex;

    return redrawRegion;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , FOdysseyVectorUndo** iUndo
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    FOdysseyVectorPathBuilder* currentPathBuilder = static_cast<FOdysseyVectorPathBuilder*>( iScene->GetLastSelected() );

    if( currentPathBuilder )
    {
        FOdysseyVectorVertexCubic* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
        FOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();

        // in some conditions (maximizing the window), you can get a Up event without a Down event. Check cubicPath exists.
        if( cubicPath )
        {
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            float radius =  iPointInTexture.pressure * Radius;
            float roundedUpRadius = /*ceil (radius)*/mPreviousVertex->GetRadius();

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
                cubicVertex = FOdysseyVectorVertexCubic::New( localCoords.x, localCoords.y, roundedUpRadius );

                cubicPath->AddVertex( cubicVertex );
            }

            currentPathBuilder->RecordEnd( cubicVertex );

            iScene->Select( cubicPath );
        }

        iScene->Unselect( currentPathBuilder );
        iScene->RemoveChild( currentPathBuilder );
        delete currentPathBuilder;

        // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
        if( iUndo && GUndo )
        {
            (*iUndo) = new FOdysseyVectorUndoObjectAdd( iScene, cubicPath );

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
