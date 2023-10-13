// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorEraserTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{

}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : Radius( 20.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");

    mEraserHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorEraserTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            LoadVector( vectorEngine, vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorEraserTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyPainterEditorVectorEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mEraserHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( &mEraserHUD );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    BLImage* currentImage = iEngine->GetBLImage();

    mEraserHUD.SetRadius( Radius );
    mEraserHUD.BlendMask( true );

    iEngine->UseMaskImage();
    iEngine->GetBLContext()->setFillAlpha( 0.0f );
    iEngine->GetBLContext()->clearAll();
    iEngine->GetBLContext()->setFillAlpha( 1.0f );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
    iEngine->UseImage( currentImage );

    iScene->Update( 0 );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/

    /*}*/
    // refresh vector scene and GUI widgets via delegates.
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    BLImage* currentImage = iEngine->GetBLImage();
    BLPoint pt = { 0, 0 };

    mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->UseMaskImage();

    iEngine->GetBLContext()->setStrokeWidth( Radius * 2 );
    iEngine->GetBLContext()->strokeLine( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                       , iPointInTexture.y - iPointInTexture.deltaPosition.Y
                                       , iPointInTexture.x 
                                       , iPointInTexture.y );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );

    iEngine->UseImage( currentImage );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> addedObjectArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorObject*> removedObjectArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    BLImage* currentImage = iEngine->GetBLImage();
    ::ULIS::FRectD roi;

    mEraserHUD.BlendMask( false );

    iEngine->UseMaskImage();

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        iEngine->EraseSections( iScene
                              , addedVertexArray
                              , addedSegmentArray
                              , removedObjectArray
                              , removedVertexArray
                              , removedSegmentArray
                              , false );
    }
    else
    {
        iEngine->Erase( iScene
                      , addedObjectArray
                      , addedVertexArray
                      , addedSegmentArray
                      , removedObjectArray
                      , removedVertexArray
                      , removedSegmentArray
                      , roi
                      , false );
    }

    iEngine->UseImage( currentImage );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("EraserTool","Erase"));
    if( GUndo )
    {
        FOdysseyVectorUndo *undo = new FOdysseyVectorUndoErase( iScene
                                                              , addedObjectArray
                                                              , addedVertexArray
                                                              , addedSegmentArray
                                                              , removedObjectArray
                                                              , removedVertexArray
                                                              , removedSegmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::Commit()
{

}

void
UOdysseyPainterEditorVectorEraserTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());

    // redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
        }
    }
}

void
UOdysseyPainterEditorVectorEraserTool::PropertyChanged( const FName& iPropertyName )
{
    mEraserHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE
