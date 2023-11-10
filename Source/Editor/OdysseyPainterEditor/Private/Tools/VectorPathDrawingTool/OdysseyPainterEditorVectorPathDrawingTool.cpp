// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Tools/VectorPathDrawingTool/SOdysseyPainterEditorVectorPathDrawingToolTopTab.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathExtend.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPalette.h"
#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
    delete mPathDrawingHUD;
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : TracingType( eTracingType::Organic )
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

    mPathDrawingHUD = new FOdysseyPainterEditorVectorPathDrawingToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->ClearHUD();
    vectorEngine->AddHUD( mPathDrawingHUD );

    vectorEngine->ResetHUD(); // creates the quadtree;

    // init pathTracer's raster image
    mPathTracer.Init( iScene );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Load()
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            LoadVector( vectorScene );
        }
    }
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorEngine* iVectorEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch || FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        FOdysseyVectorVertex* stitchCubicVertex = nullptr;
        std::vector<FOdysseyVectorPoint*>& pickedPointArray = mPathDrawingHUD->GetStitchedPointArray();

        pickedPointArray.reserve( 50 );
/*
        iVectorEngine->PickPoints( iScene
                                 , false
                                 , iWorldX
                                 , iWorldY
                                 , iPickingRadius
                                 , pickedPointArray
                                 , FOdysseyVectorPath::PICK_POINT );
*/
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

FOdysseyVectorPathTracer&
UOdysseyPainterEditorVectorPathDrawingTool::GetPathTracer()
{
    return mPathTracer;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->RemoveHUD( mPathDrawingHUD );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::HasMedia() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Unload()
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            UnloadVector( vectorScene );
        }
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    return HasMedia();
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathDrawingTool::CreateTopTabWidget()
{
    return SNew(SOdysseyPainterEditorVectorPathDrawingToolTopTab, this);
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnSizeChanged()
{

}

void
UOdysseyPainterEditorVectorPathDrawingTool::SetPathColor( FOdysseyVectorPath* iPath )
{
    ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), /*rgba8.A8()*/ Opacity * 255.0f );
    TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
    UOdysseyPalette* palette = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette();

    if( palette )
    {
        UOdysseyPaletteEntry* paletteEntry = palette->CurrentEntry.Get();

        if( paletteEntry && paletteEntry->IsA( UOdysseyPaletteEntryColor::StaticClass() ) )
        {
            FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(paletteEntry)->GetUsedColor();
            ueColor = colorEntry;

            iPath->GetForegroundBucket().SetPaletteEntry( paletteEntry );
        }
    }

    iPath->SetForegroundColor( ueColor );
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

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                           , const FKey& iKey )
{
    StitchAtKeyDown = Stitch;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Stitch = !Stitch; // flip the value
    }

    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyDown( const FKey& iKey )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            return OnKeyDownVector( vectorScene, iKey );
        }
    }

    return true;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                         , const FKey& iKey )
{
    Stitch = StitchAtKeyDown;

    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyUp( const FKey& iKey )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            return OnKeyUpVector( vectorScene, iKey );
        }
    }

    return true;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorPath* path = nullptr;

    mUndoPathExtend = nullptr;
    mStitchedVertex = nullptr;

    if( Stitch )
    {
        mStitchedVertex = PickVertex( vectorEngine
                                    , iScene
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

    mPathTracer.AttachPath( path );

    iScene->ClearSelection();
    iScene->Select( path );
    // update invalidated objects
    iScene->Update( 0 );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED
                        | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    vectorEngine->ResetHUD(); // re-creates the quadtree;

    return true;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            return OnMouseDownVector( vectorScene, iPointInTexture, iKey );
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetWidth();
    uint32 height = vectorEngine->GetHeight();
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

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            OnMouseHoverVector( vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector);
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

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            OnMouseDragVector( vectorScene, iPointInTexture );
        }
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorSegment* newSegment;
    FOdysseyVectorVertex* endingVertex = PickVertex( vectorEngine
                                                   , iScene
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

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY // important to remove the path builder from the hierarchy widget
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            return OnMouseUpVector( vectorScene, iPointInTexture, iKey );
        }
    }

    return true;
}


void
UOdysseyPainterEditorVectorPathDrawingTool::Commit()
{
}

void
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

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if( HasMedia() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
            return;

        if ( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            PropertyChangedVector( vectorScene, PropertyChangedEvent.GetPropertyName() );
        }
    }
}



#undef LOCTEXT_NAMESPACE
