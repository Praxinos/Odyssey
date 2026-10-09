// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"

#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/GenericPlatformTime.h"
#include "ISinglePropertyView.h"
#include "Toolkits/BaseToolkit.h"
#include "ToolMenu.h"
#include "Widgets/Layout/SWrapBox.h"

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorCommands.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SOdysseySinglePropertyView.h"

#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPathDrawingToolHUD>( this ), true, false )
    , Opacity( 1.0f )
    , Brush( nullptr )
    , TracingType( eTracingType::Organic )
    , TracingFidelity( eTracingFidelity::Highest )
    , Radius( 5.0f )
    , PressureSensitive( true )
    //, Absolute( true )
    , UpdatePaintGroups( true )
    , Stitch( false )
    , Snap( true )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , Debug( false )
    , mStitchedVertex( nullptr )
    , mPathNumber( 0 )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.PathDrawing64"));

    mPathTracer.SetTracingWidth( (double) TracingFidelity );

    mPathDrawingHUD = static_cast<FOdysseyPainterEditorVectorPathDrawingToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void UOdysseyPainterEditorVectorPathDrawingTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);

    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorPathDrawingTool::__VA_ARGS__ ) );
    #define MAP_ACTION_REPEAT(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorPathDrawingTool::__VA_ARGS__ ), EUIActionRepeatMode::RepeatEnabled );

    MAP_ACTION_REPEAT(painterEditorToolCommands.IncreaseBrushSize, AddSize, 1)
    MAP_ACTION_REPEAT(painterEditorToolCommands.DecreaseBrushSize, AddSize, -1)

    #undef MAP_ACTION
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::SetRadius(float iRadius)
{
    Radius = iRadius;
}

float
UOdysseyPainterEditorVectorPathDrawingTool::GetRadius() const
{
    return Radius;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::AddSize(int iAmount)
{
    double value = FMath::Max(Radius + iAmount, 0.f);
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathDrawingTool, Radius), value);
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // init pathTracer's raster image
    mPathTracer.Init( iScene );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}


FOdysseyVectorPathTracer&
UOdysseyPainterEditorVectorPathDrawingTool::GetPathTracer()
{
    return mPathTracer;
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorGroupPaint* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch || Snap || FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        FOdysseyVectorVertex* stitchCubicVertex = nullptr;
        // the HUD provides the hovered vertices
        std::vector<FOdysseyVectorPoint*>& pickedPointArray = mPathDrawingHUD->GetStitchedPointArray();

        pickedPointArray.reserve( 50 );

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

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                               , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                               , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    // stays empty
    std::vector<FOdysseyVectorPath*> addedObjectArray;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-drawing-tool.transaction.draw-path-and-stitch","Vector Path Drawing Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , addedObjectArray
                                                                  , iAddedVertexArray
                                                                  , iAddedSegmentArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathAdd( FOdysseyVectorGroupPaint* iScene
                                                             , FOdysseyVectorPath* iPath
                                                             , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                             , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-drawing-tool.transaction.draw-path","Vector Path Drawing Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene
                                                                                                    , iPath ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                 , const FKeyEvent& InKeyEvent )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        StitchAtKeyDown = Stitch;

        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            Stitch = !Stitch; // flip the value

            return true;
        }
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( iScene, InKeyEvent );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                               , const FKeyEvent& InKeyEvent )
{
    FKey key = InKeyEvent.GetKey();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   ) )
    {
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        Stitch = StitchAtKeyDown;

        return true;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( iScene, InKeyEvent );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    double pointRadius = PressureSensitive ? ( iPointInTexture.pressure * Radius ) : Radius;
    ::ULIS::FVec2D vertexWorldCoords = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    mPathTracer.Reset();
    mStitchedVertex = nullptr;
    mPathDrawingMode = ePathDrawingMode::Create;

    mTimeAtDown = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( iKey == EKeys::LeftMouseButton ) && GetWorkingGroup() && GetWorkingGroup()->IsVisible( true ) )
    {
        FOdysseyVectorPath* path = nullptr;

        mAddedVertexArray.clear();
        mAddedSegmentArray.clear();

        if( Stitch || Snap )
        {
            FOdysseyVectorVertex* pickedVertex = PickVertex( iScene
                                                           , iPointInTexture.x
                                                           , iPointInTexture.y
                                                           , StitchingRadius );

            if( pickedVertex )
            {
                if( Stitch )
                {
                    mStitchedVertex = pickedVertex;

                    path = mStitchedVertex->GetOwnerAsPath();

                    mPathDrawingMode = ePathDrawingMode::Alter;
                }

                if( Snap )
                {
                    ::ULIS::FVec2D& localCoords = pickedVertex->GetCoords();
                    double localRadius = pickedVertex->GetRadius();
                    BLPoint worldCoords = pickedVertex->GetOwner()->GetWorldMatrix().map_point( localCoords.x, localCoords.y );

                    vertexWorldCoords = ::ULIS::FVec2D( worldCoords.x
                                                      , worldCoords.y );
                }
            }
        }

        if( mPathDrawingMode == ePathDrawingMode::Create )
        {
            FOdysseyVectorGroup* workingGroup = GetWorkingGroup();

            path = new FOdysseyVectorPath( FString( "Path_" ) + FString::FromInt( mPathNumber++ ) );

            workingGroup->AppendChild( path );
            path->UpdateMatrix();

            SetPathColor( path );

            path->SetOpacity( Opacity );
        }

        if( mPathDrawingMode == ePathDrawingMode::Create )
        {
            BLMatrix2D& pathInverseWorldMatrix = path->GetInverseWorldMatrix();
            BLPoint localPoint = pathInverseWorldMatrix.map_point( vertexWorldCoords.x
                                                                , vertexWorldCoords.y );
            BLPoint localVector = pathInverseWorldMatrix.map_vector( pointRadius * 0.7071f
                                                                  , pointRadius * 0.7071f );
            double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();

            mStitchedVertex = new FOdysseyVectorVertex( localPoint.x
                                                      , localPoint.y
                                                      , localRadius );

            path->AddVertex( mStitchedVertex );
        }

        mPathTracer.AttachPath( path );

        mPathTracer.Trace( nullptr
                         , iPointInTexture.x
                         , iPointInTexture.y
                         , pointRadius );

        path->SetBrush( Brush );

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector);

    // Left mouse button clicked
    // For some reason, iPointInTexture.keysDown.Find does not find the left button click for the first few events
    // when using the stylus so we use mPathTracer.GetPath instead
    //if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    if( mPathTracer.GetPath() && GetWorkingGroup() && GetWorkingGroup()->IsVisible( true ) )
    {
        FOdysseyVectorPath* path = mPathTracer.GetPath();
        double pointRadius = PressureSensitive ? ( iPointInTexture.pressure * Radius ) : Radius;
        FOdysseyVectorSegment* newSegment;
        FOdysseyVectorSegment* prevSegment = nullptr;

        // mandatory for stitching vertices
        mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        if( mPathTracer.GetPointBuffer().size() == 1 )
        {
            BLMatrix2D& pathInverseWorldMatrix = path->GetInverseWorldMatrix();
            BLPoint localVector = pathInverseWorldMatrix.map_vector( pointRadius * 0.7071f
                                                                  , pointRadius * 0.7071f );
            double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();

            mPathTracer.GetPointBuffer()[0].radius = pointRadius;
            mStitchedVertex->SetRadius( localRadius );
        }

        newSegment = mPathTracer.Trace( mStitchedVertex
                                      , iPointInTexture.x
                                      , iPointInTexture.y
                                      , pointRadius );

        if( newSegment )
        {
            mAddedVertexArray.push_back( newSegment->GetVertex(1) );
            mAddedSegmentArray.push_back( newSegment );

            prevSegment = mStitchedVertex->GetOtherSegment( newSegment );

            // the newly created vertex is now the stitched vertex
            mStitchedVertex = mAddedVertexArray.back();


            //mUndoPathExtend->RecordSegment( newSegment, newSegment->GetVertex(1) );
        }

        // dont redraw everything if no new segment was created
        if( newSegment == nullptr )
        {
            static double oldMouseX;
            static double oldMouseY;
            double x1 = oldMouseX
                 , y1 = oldMouseY
                 , x2 = iPointInTexture.x
                 , y2 = iPointInTexture.y;
            double xmin = ::ULIS::FMath::Min( x1, x2 ) - pointRadius
                 , ymin = ::ULIS::FMath::Min( y1, y2 ) - pointRadius
                 , xmax = ::ULIS::FMath::Max( x1, x2 ) + pointRadius
                 , ymax = ::ULIS::FMath::Max( y1, y2 ) + pointRadius;

            //vectorEngine->SetInvalidatedRect( mPathTracer.GetRedrawRect() );

            iScene->GetCell()->InvalidateRect( ::ULIS::FRectD::FromMinMax( xmin
                                                                         , ymin
                                                                         , xmax
                                                                         , ymax ) );

            oldMouseX = iPointInTexture.x;
            oldMouseY = iPointInTexture.y;
        }

        // update invalidated objects. Updating via shared Env will invalidate the engine, thus redrawing the image
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING ); // update invalidated path after segment insertion
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    ::ULIS::FVec2D vertexWorldCoords = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    mTimeAtUp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( iKey == EKeys::LeftMouseButton ) && GetWorkingGroup() && GetWorkingGroup()->IsVisible( true ) )
    {
        // check path validity in case we get a UP without a DOWN first
        if( mPathTracer.GetPath() )
        {
            FOdysseyVectorPath* path = mPathTracer.GetPath();
            FOdysseyVectorSegment* newSegment;
            FOdysseyVectorVertex* endingVertex = PickVertex( iScene
                                                           , iPointInTexture.x
                                                           , iPointInTexture.y
                                                           , StitchingRadius );
            std::vector<FOdysseyVectorSegment*> pickedSegmentArray;

            // check for hovered segment before the new segment is added
            if( Snap )
            {
                if( endingVertex )
                {
                    vertexWorldCoords = endingVertex->GetWorldCoords();
                }
                else
                {
                    // don't snap when the user does small strokes.
                    if( ( mTimeAtUp - mTimeAtDown ) > 200 )
                    {
                        // check if we picked any segment
                        FOdysseyVectorSegment* closestSegment = PickSegments( iScene
                                                                            , iPointInTexture.x
                                                                            , iPointInTexture.y
                                                                            , StitchingRadius
                                                                            , false
                                                                            , true
                                                                            , pickedSegmentArray );

                        if( closestSegment )
                        {
                            FOdysseyVectorSegment* pickedSegment = pickedSegmentArray[0];
                            BLMatrix2D& inverseWorldMatrix = pickedSegment->GetOwner()->GetInverseWorldMatrix();
                            BLMatrix2D& worldMatrix = pickedSegment->GetOwner()->GetWorldMatrix();
                            BLPoint localPoint = inverseWorldMatrix.map_point( iPointInTexture.x
                                                                            , iPointInTexture.y );
                            ::ULIS::FVec2D vertexLocalCoords;
                            double projectedPointT = pickedSegment->ProjectConstrained( ::ULIS::FVec2D( localPoint.x
                                                                                                      , localPoint.y )
                                                                                        , vertexLocalCoords );
                            BLPoint worldPoint = worldMatrix.map_point( vertexLocalCoords.x
                                                                     , vertexLocalCoords.y );
                            vertexWorldCoords = ::ULIS::FVec2D( worldPoint.x, worldPoint.y );
                        }
                    }
                }
            }

            // stitching to another path at MouseUp is CURRENTLY not supported
            if( endingVertex && ( endingVertex->GetOwnerAsPath() != path ) )
            {
                endingVertex = nullptr;
            }

            newSegment = mPathTracer.Flush( mStitchedVertex, Stitch ? endingVertex : nullptr );

            if( newSegment )
            {
                BLPoint vertexLocalPoint = path->GetInverseWorldMatrix().map_point( vertexWorldCoords.x
                                                                                 , vertexWorldCoords.y );

                // this will snap to the correct location and do nothing in the worst case scenario.
                newSegment->GetVertex(1)->Set( vertexLocalPoint.x, vertexLocalPoint.y );

                if( endingVertex == nullptr )
                {
                    mAddedVertexArray.push_back( newSegment->GetVertex(1) );
                }

                mAddedSegmentArray.push_back( newSegment );
            }

            if( mPathDrawingMode == ePathDrawingMode::Alter )
            {
                if( mAddedSegmentArray.size() )
                {
                    RecordUndoPathAlter( iScene
                                        , mAddedVertexArray
                                        , mAddedSegmentArray );
                }
            }

            if( mPathDrawingMode == ePathDrawingMode::Create )
            {
                // TODO: this is only for NEW path
                if ( path->GetVertexList().size() <= 1 )
                {
                    // delete any new path with single vertex that was create at mouse down
                    path->GetParent()->RemoveChild( path );
                    iScene->GetCell()->UnselectObject( path );

                    delete path;
                }
                else
                {
                    // Note, newSegment can be NULL and we still have a valid path if the last vertex
                    // was create precisely at last drag before mouseup.
                    RecordUndoPathAdd( iScene
                                     , path
                                     , mAddedVertexArray
                                     , mAddedSegmentArray );
                }
            }
        }
    }

    // update invalidated objects. Updating via shared Env will invalidate the engine, thus redrawing the image
    iScene->GetLayer()->Update( UpdatePaintGroups ? FOdysseyVectorObject::UPDATE_PAINTGROUPS : 0 );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

/*
    // in OnMouseDragVector() we are not guaranteed to get a viewport redraw from what I understand.
    // this means the Invalidation Rectangle is not resetted, so we force it.
    vectorEngine->InvalidateRect( ::ULIS::FRectD( 0
                                                , 0
                                                , iScene->GetEngine()->GetLayer()->GetWidth()
                                                , iScene->GetEngine()->GetLayer()->GetHeight() ) );
*/

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                 , const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathDrawingTool, TracingType) )
    {
        switch( TracingType )
        {
            case eTracingType::Organic :
                mPathTracer.SetDotLimit( 0.0f ); // cos 90deg
            break;

            case eTracingType::Mechanic :
                mPathTracer.SetDotLimit( 0.9659f ); // cos 15deg
            break;

            default :
            break;
        }
    }

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathDrawingTool, TracingFidelity) )
    {
        mPathTracer.SetTracingWidth( (double) TracingFidelity );
    }

    //vectorEngine->ResetHUD(); // rebuilds quadtree if stitch mode changes

    // redraw
    //iScene->GetEngine()->Invalidate( 0 );

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathDrawingTool, Radius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathDrawingTool, TracingFidelity ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorPathDrawingTool::GetTooltip() const
{
    return LOCTEXT("vector-path-drawing-tool.tooltip", "Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
