// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Tools/VectorPathDrawingTool/SOdysseyPainterEditorVectorPathDrawingToolTopTab.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
    delete mPathDrawingHUD;
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Radius( 5.0f )
    , Opacity( 1.0f )
    , PressureSensitive( true )
    , Absolute( true )
    , UpdatePaintGroups( true )
    , Stitch( false )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , Debug( false )
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
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        LoadVector( vectorScene );
    }
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

void
UOdysseyPainterEditorVectorPathDrawingTool::Unload()
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        UnloadVector( vectorScene );
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
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

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorPath* path = new FOdysseyVectorPath( "Path" );

    iScene->AppendChild( path );

    path->UpdateMatrix();

    mPathTracer.AttachPath( path );

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
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        return OnMouseDownVector( vectorScene, iPointInTexture, iKey );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        OnMouseHoverVector( vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    double pointRadius = PressureSensitive ? ( iPointInTexture.pressure * Radius ) : Radius;
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mPathTracer.Trace( iPointInTexture.x, iPointInTexture.y, pointRadius );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        OnMouseDragVector( vectorScene, iPointInTexture );
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mPathTracer.Flush();

    iScene->Update( UpdatePaintGroups ? FOdysseyVectorObject::UPDATEPAINTGROUPS : 0 ); // update invalidated objects

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY // important to remove the path builder from the hierarchy widget
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    if ( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

        return OnMouseUpVector( vectorScene, iPointInTexture, iKey );
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

    vectorEngine->ResetHUD(); // rebuilds quadtree if stitch mode changes

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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



#undef LOCTEXT_NAMESPACE
