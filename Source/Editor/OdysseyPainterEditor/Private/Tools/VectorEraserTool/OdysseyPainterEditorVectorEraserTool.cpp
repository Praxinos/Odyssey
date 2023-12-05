// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorEraserTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{
}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorEraserToolHUD( this ), false )
    , Radius( 20.0f )
    , Split( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");

    mEraserHUD = static_cast<FOdysseyPainterEditorVectorEraserToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
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
UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
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
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
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

void
UOdysseyPainterEditorVectorEraserTool::EraseSections( FOdysseyVectorGroupPaint* iScene
                                                    , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                                                    , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                                    , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                                    , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray
                                                    , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                                    , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &oAddedObjectArray
      , &oAddedVertexArray
      , &oAddedSegmentArray
      , &oRemovedObjectArray
      , &oRemovedVertexArray
      , &oRemovedSegmentArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) ) 
              {
                  FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);
                  ::ULIS::FRectD unusedRect;

                  paintGroup->EraseSections( unusedRect
                                           , oAddedObjectArray
                                           , oAddedVertexArray
                                           , oAddedSegmentArray
                                           , oRemovedObjectArray
                                           , oRemovedVertexArray
                                           , oRemovedSegmentArray
                                           , Split );
              }

              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) ) 
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  FOdysseyVectorObject* parent = path->GetParent();

                  if( parent->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(parent);
                      ::ULIS::FRectD unusedRect;

                      paintGroup->EraseSections( unusedRect
                                               , oAddedObjectArray
                                               , oAddedVertexArray
                                               , oAddedSegmentArray
                                               , oRemovedObjectArray
                                               , oRemovedVertexArray
                                               , oRemovedSegmentArray
                                               , Split );
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    for( int i = 0; i < oAddedObjectArray.size(); i++ )
    {
        // parent is stored in mParent variable byt the Erase function even though it does not
        // belong to the parent. It kinda sucks but it is easier that way, otherwise Traverse()
        // will have some problems because we change the children list, and Traverse is recursive.
        oAddedObjectArray[i]->GetParent()->AppendChild( oAddedObjectArray[i] );

        oAddedObjectArray[i]->Invalidate();
    }

    // Also here, we remove AFTER the Traverse() has been executed, because traverse is recursive
    // so we can't alter the hierarchy, unles traverse works on copies of the children list but that 
    // would be very inefficient.
    for( int i = 0; i < oRemovedObjectArray.size(); i++ )
    {
        if( oRemovedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            oRemovedObjectArray[i]->GetParent()->RemoveChild( oRemovedObjectArray[i] );

            if( oRemovedObjectArray[i]->IsSelected() )
            {
                vectorEngine->UnselectObject( oRemovedObjectArray[i] );
            }
        }
    }

    iScene->UpdateMatrix();

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}

void
UOdysseyPainterEditorVectorEraserTool::ErasePaths( FOdysseyVectorGroupPaint* iScene
                                                 , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                                                 , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                                 , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                                 , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                                 , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                                 , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , GetEditor()->GetVectorHUDFlags()
    , [ this
      , iScene
      , vectorEngine
      , &oAddedObjectArray
      , &oAddedVertexArray
      , &oAddedSegmentArray
      , &oRemovedVertexArray
      , &oRemovedSegmentArray
      , &oRemovedObjectArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) ) 
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  ::ULIS::FRectD unusedRect;

                  if( path->Erase( unusedRect
                                 , oAddedObjectArray
                                 , oAddedVertexArray
                                 , oAddedSegmentArray
                                 , oRemovedVertexArray
                                 , oRemovedSegmentArray
                                 , false
                                 , Split ) )
                  {
                      oRemovedObjectArray.push_back( path );
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    for( int i = 0; i < oAddedObjectArray.size(); i++ )
    {
        // parent is stored in mParent variable byt the Erase function even though it does not
        // belong to the parent. It kinda sucks but it is easier that way, otherwise Traverse()
        // will have some problems because we change the children list, and Traverse is recursive.
        oAddedObjectArray[i]->GetParent()->AppendChild( oAddedObjectArray[i] );

        oAddedObjectArray[i]->Invalidate();
    }

    // Also here, we remove AFTER the Traverse() has been executed, because traverse is recursive
    // so we can't alter the hierarchy, unles traverse works on copies of the children list but that 
    // would be very inefficient.
    for( int i = 0; i < oRemovedObjectArray.size(); i++ )
    {
        if( oRemovedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            oRemovedObjectArray[i]->GetParent()->RemoveChild( oRemovedObjectArray[i] );

            if( oRemovedObjectArray[i]->IsSelected() )
            {
                vectorEngine->UnselectObject( oRemovedObjectArray[i] );
            }
        }
    }

    iScene->UpdateMatrix();

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}

uint64
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

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
        vectorEngine->SetBLMask( mEraserHUD->GetMask() );

        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            EraseSections( iScene
                         , addedObjectArray
                         , addedVertexArray
                         , addedSegmentArray
                         , removedObjectArray
                         , removedVertexArray
                         , removedSegmentArray );
        }
        else
        {
            ErasePaths( iScene
                      , addedObjectArray
                      , addedVertexArray
                      , addedSegmentArray
                      , removedVertexArray
                      , removedSegmentArray
                      , removedObjectArray );
        }

        vectorEngine->SetBLMask( nullptr );

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

    // this will resize the selection box, knowing that some paths may have been removed after erasal.
    vectorEngine->ResetHUD();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED;
}

#undef LOCTEXT_NAMESPACE
