// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutToolHUD.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathCutToolHUD( this ), false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCut64");

    mPathCutHUD = static_cast<FOdysseyPainterEditorVectorPathCutToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathCutTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathCutTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathCutTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathCutTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPathCutHUD->SetP0( iPointInTexture.x, iPointInTexture.y );
        mPathCutHUD->SetP1( iPointInTexture.x, iPointInTexture.y );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathCutTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture )
{
/*
    ::ULIS::FVec2D& p0 = mLineHUD.GetP0();
    ::ULIS::FVec2D& p1 = mLineHUD.GetP1();
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(p0.x, p1.x)
                                                    , ::ULIS::FMath::Min(p0.y, p1.y)
                                                    , ::ULIS::FMath::Max(p0.x, p1.x)
                                                    , ::ULIS::FMath::Max(p0.y, p1.y) );
*/
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathCutHUD->SetP1( iPointInTexture.x, iPointInTexture.y );
    } 

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

void
UOdysseyPainterEditorVectorPathCutTool::CutPath( FOdysseyVectorPath* iPath
                                               , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                               , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                               , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    BLMatrix2D& inverseWorldMatrix = iPath->GetInverseWorldMatrix();
    ::ULIS::FVec2D& p0 = mPathCutHUD->GetP0();
    ::ULIS::FVec2D& p1 = mPathCutHUD->GetP1();
    BLPoint localP0 = inverseWorldMatrix.mapPoint( p0.x, p0.y );
    BLPoint localP1 = inverseWorldMatrix.mapPoint( p1.x, p1.y );
    double xmin = ::ULIS::FMath::Min( localP0.x, localP1.x );
    double ymin = ::ULIS::FMath::Min( localP0.y, localP1.y );
    double xmax = ::ULIS::FMath::Max( localP0.x, localP1.x );
    double ymax = ::ULIS::FMath::Max( localP0.y, localP1.y );
    ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    ::ULIS::FRectD isxRect;

    if( FOdysseyVector::IntersectRegions<double>( rect, iPath->GetBBox(false), &isxRect ) )
    {
        iPath->Cut( ::ULIS::FVec2D( localP0.x, localP0.y )
                  , ::ULIS::FVec2D( localP1.x, localP1.y )
                  , oAddedVertexArray
                  , oAddedSegmentArray
                  , oRemovedSegmentArray );
    }

    //path->Invalidate();
}

void
UOdysseyPainterEditorVectorPathCutTool::CutObjectRecursive( FOdysseyVectorObject* iObject
                                                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        CutPath( path, oAddedVertexArray, oAddedSegmentArray, oRemovedSegmentArray );
    }

    for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
    {
        CutObjectRecursive( childObject
                          , oAddedVertexArray
                          , oAddedSegmentArray
                          , oRemovedSegmentArray );
    }
}

uint64
UOdysseyPainterEditorVectorPathCutTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        FOdysseyVectorEngine* iEngine = iScene->GetEngine();

        // crashes if I don't reserve. Why that ?
        removedSegmentArray.reserve(50);
        addedSegmentArray.reserve(50);
        addedVertexArray.reserve(50);

        // traverse recursively on objects determined by the HUD
        vectorEngine->Traverse
        ( iScene
        , 0
        , [ this
          , iScene
          , vectorEngine
          , &addedVertexArray
          , &addedSegmentArray
          , &removedSegmentArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
          {
              if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags )  )
              {
                  if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                  {
                      FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                      CutPath( path
                              , addedVertexArray
                              , addedSegmentArray
                              , removedSegmentArray );
                  }

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-cut-tool.transaction.cut-path","Vector Path Cut Tool"));
        if( GUndo )
        {
            std::vector<FOdysseyVectorPath*> removedPathArray; // empty on purpose.
            std::vector<FOdysseyVectorVertex*> removedVertexArray; // empty on purpose.
            std::vector<FOdysseyVectorPath*> addedPathArray; // empty on purpose.

            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                      , removedPathArray
                                                                      , removedVertexArray
                                                                      , removedSegmentArray
                                                                      , addedPathArray
                                                                      , addedVertexArray
                                                                      , addedSegmentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iEngine->ResetHUD();

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

#undef LOCTEXT_NAMESPACE
