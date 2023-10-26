// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorEraserTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{
    delete mEraserHUD;
}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : Radius( 20.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");

    mEraserHUD = new FOdysseyPainterEditorVectorEraserToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->RemoveHUD( mEraserHUD );

    mEraserHUD->Unload( iScene );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mEraserHUD->Load( iScene );

    iEngine->ClearHUD();
    iEngine->AddHUD( mEraserHUD );

    iEngine->ResetHUD();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mEraserHUD->BlendMask( true );
        mEraserHUD->ClearMask();
        mEraserHUD->FillCircle( iPointInTexture.x, iPointInTexture.y );

        iScene->Update( 0 );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mEraserHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/

    /*}*/
    // refresh vector scene and GUI widgets via delegates.
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mEraserHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
        mEraserHUD->StrokeLine( ::ULIS::FVec2D( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                              , iPointInTexture.y - iPointInTexture.deltaPosition.Y )
                              , ::ULIS::FVec2D( iPointInTexture.x 
                                              , iPointInTexture.y ) );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        std::vector<FOdysseyVectorObject*> addedObjectArray;
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorObject*> removedObjectArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        ::ULIS::FRectD roi;

        mEraserHUD->BlendMask( false );
        // TODO: pass the mask image as arg to Pick function
        iEngine->SetBLMask( mEraserHUD->GetMask() );

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

        iEngine->SetBLMask( nullptr );

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
   }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED;
}

#undef LOCTEXT_NAMESPACE
