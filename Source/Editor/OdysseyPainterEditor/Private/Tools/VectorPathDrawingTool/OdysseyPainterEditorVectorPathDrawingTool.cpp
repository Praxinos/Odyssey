// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathExtend.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathDrawingToolHUD( this ) )
    , ColorSource( ePathDrawingToolColorSource::ColorWheel )
    , TracingType( eTracingType::Organic )
    , TracingFidelity( eTracingFidelity::Average )
    , Radius( 5.0f )
    , Opacity( 1.0f )
    , PressureSensitive( true )
    , Absolute( true )
    , UpdatePaintGroups( true )
    , Stitch( false )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , Debug( false )
    , mStitchedVertex( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD = static_cast<FOdysseyPainterEditorVectorPathDrawingToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( FOdysseyVectorScene* iScene )
{
    // init pathTracer's raster image
    mPathTracer.Init( iScene );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}


FOdysseyVectorPathTracer&
UOdysseyPainterEditorVectorPathDrawingTool::GetPathTracer()
{
    return mPathTracer;
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch || FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
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
UOdysseyPainterEditorVectorPathDrawingTool::SetPathColor( FOdysseyVectorPath* iPath )
{
    switch( ColorSource )
    {
        case ePathDrawingToolColorSource::ColorWheel:
        {
            ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
            ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
            FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

            iPath->GetForegroundBucket().SetSolidColor( ueColor );
        }
        break;

        case ePathDrawingToolColorSource::Palette:
        {
            TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
            UOdysseyPalette* palette = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette();

            if( palette )
            {
                UOdysseyPaletteEntry * paletteEntry = palette->CurrentEntry.Get();

                if( paletteEntry && paletteEntry->IsA( UOdysseyPaletteEntryColor::StaticClass() ) )
                {
                    iPath->GetForegroundBucket().SetPaletteEntry( paletteEntry );
                }
            }
        }
        break;

        default:
        break;
    }

    iPath->SetOpacity( Opacity );
    iPath->GetForegroundBucket().SetColorMode( (eBucketColorMode)ColorSource );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathExtend( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorPath* iPath  )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathDrawingTool","Vector Path Drawing Tool"));
    if( GUndo )
    {
        mUndoPathExtend = new FOdysseyVectorUndoPathExtend( iScene, iPath );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(mUndoPathExtend) );
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathAdd( FOdysseyVectorScene* iScene
                                                             , FOdysseyVectorPath* iPath  )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathDrawingTool","Vector Path Drawing Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, iPath ) );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                           , const FKey& iKey )
{
    StitchAtKeyDown = Stitch;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Stitch = !Stitch; // flip the value
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                         , const FKey& iKey )
{
    Stitch = StitchAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorPath* path = nullptr;

        mUndoPathExtend = nullptr;
        mStitchedVertex = nullptr;

        if( Stitch )
        {
            mStitchedVertex = PickVertex( iScene
                                        , iPointInTexture.x
                                        , iPointInTexture.y
                                        , StitchingRadius );
            if( mStitchedVertex )
            {
                path = mStitchedVertex->GetPath();
            }

            // This undo must be set before association with the new parent object
            RecordUndoPathExtend( iScene, path );
        }

        if( path == nullptr )
        {
            path = new FOdysseyVectorPath( "Path" );

            // This undo must be set before association with the new parent object
            RecordUndoPathAdd( iScene, path );

            iScene->AppendChild( path );
            path->UpdateMatrix();

            SetPathColor( path );
        }

        path->SetBrush( Brush );

        mPathTracer.AttachPath( path );

        //iScene->ClearSelection();
        //iScene->Select( path );

        // update invalidated objects
        iScene->Update( 0 );

        vectorEngine->ResetHUD(); // re-creates the quadtree;
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    imageRegion.x = 0;
    imageRegion.y = 0;
    imageRegion.w = width;
    imageRegion.h = height;

    if( mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) == true )
    {
        redrawRegion = imageRegion; // needs full redraw
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector);

    // Left mouse button clicked
    // For some reason, iPointInTexture.keysDown.Find does not find the left button click for the first few events
    // when using the stylus so we use mPathTracer.GetPath instead
    //if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    if( mPathTracer.GetPath() )
    {
        double pointRadius = PressureSensitive ? ( iPointInTexture.pressure * Radius ) : Radius;
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
        FOdysseyVectorSegment* newSegment;

        // mandatory for stitching vertices
        mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        newSegment = mPathTracer.Trace( mStitchedVertex, iPointInTexture.x, iPointInTexture.y, pointRadius );

        if( newSegment && mStitchedVertex )
        {
            mUndoPathExtend->RecordSegment( newSegment, newSegment->GetVertex(1) );
        }

        // dont redraw everything if no new segment was created
        if( newSegment == nullptr )
        {
            vectorEngine->SetInvalidatedRect( mPathTracer.GetRedrawRect() );
        }
        else
        {
            uint32 imgW = vectorEngine->GetPreferredWidth(),
                   imgH = vectorEngine->GetPreferredHeight();

            vectorEngine->SetInvalidatedRect( ::ULIS::FRectI( 0, 0, imgW, imgH ) );
        }

        iScene->Update( 0 ); // update invalidated path after segment insertion
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
        FOdysseyVectorSegment* newSegment;
        FOdysseyVectorVertex* endingVertex = PickVertex( iScene
                                                       , iPointInTexture.x
                                                       , iPointInTexture.y
                                                       , StitchingRadius );

        // stitching to another path at MouseUp is CURRENTLY not supported
        if( endingVertex && ( endingVertex->GetPath() != mPathTracer.GetPath() ) )
        {
            endingVertex = nullptr;
        }

        newSegment = mPathTracer.Flush( endingVertex );

        if( newSegment && mStitchedVertex )
        {
            mUndoPathExtend->RecordSegment( newSegment, endingVertex ? nullptr : newSegment->GetVertex(1) );
        }

        iScene->Update( UpdatePaintGroups ? FOdysseyVectorObject::UPDATEPAINTGROUPS : 0 ); // update invalidated objects

        vectorEngine->ResetHUD(); // re-creates the quadtree;
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY // important to remove the path builder from the hierarchy widget
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChangedVector( FOdysseyVectorScene* iScene
                                                                 , const FName& iPropertyName )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    if( iPropertyName == "TracingType" )
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

    if( iPropertyName == "TracingFidelity" )
    {
        mPathTracer.SetTracingWidth( (double) TracingFidelity );
    }

    vectorEngine->ResetHUD(); // rebuilds quadtree if stitch mode changes

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

#undef LOCTEXT_NAMESPACE
