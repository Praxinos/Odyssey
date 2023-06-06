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
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mEraserHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->AddHUD( &mEraserHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    mEraserHUD.SetRadius( Radius );
    mEraserHUD.BlendMask( true );

    iEngine->UseMaskImage();
    iEngine->GetBLContext()->setFillAlpha( 0.0f );
    iEngine->GetBLContext()->clearAll();
    iEngine->GetBLContext()->setFillAlpha( 1.0f );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
    iEngine->UseColorImage();

    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    BLPoint pt = { 0, 0 };

    mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->UseMaskImage();
    iEngine->GetBLContext()->setStrokeWidth( Radius * 2 );
    iEngine->GetBLContext()->strokeLine( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                       , iPointInTexture.y - iPointInTexture.deltaPosition.Y
                                       , iPointInTexture.x 
                                       , iPointInTexture.y );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
    iEngine->UseColorImage();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
    ::ULIS::FRectD roi;

    mEraserHUD.BlendMask( false );

    iEngine->UseMaskImage();
    iEngine->Erase( iScene
                  , addedObjectArray
                  , addedVertexArray
                  , addedSegmentArray
                  , removedObjectArray
                  , removedVertexArray
                  , removedSegmentArray
                  , roi
                  , false );
    iEngine->UseColorImage();

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );

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
UOdysseyPainterEditorVectorEraserTool::PropertyChanged( const FName& iPropertyName )
{
    mEraserHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE
