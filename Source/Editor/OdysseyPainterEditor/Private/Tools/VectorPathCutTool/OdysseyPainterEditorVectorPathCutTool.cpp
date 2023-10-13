// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutToolHUD.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathCutTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
   delete mPathCutHUD;
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCutTool64");

    mPathCutHUD = new FOdysseyPainterEditorVectorPathCutToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathCutTool::Load()
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
UOdysseyPainterEditorVectorPathCutTool::Unload()
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
UOdysseyPainterEditorVectorPathCutTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathCutTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathCutHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathCutTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathCutHUD );

    iEngine->ResetHUD();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    mPathCutHUD->SetP0( iPointInTexture.x, iPointInTexture.y );
    mPathCutHUD->SetP1( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
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
UOdysseyPainterEditorVectorPathCutTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
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
    mPathCutHUD->SetP1( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
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

    if( FOdysseyVector::IntersectRegions<double>( rect, iPath->GetBBox(false), isxRect ) )
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
UOdysseyPainterEditorVectorPathCutTool::CutPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                                                     , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                                     , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                                     , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{

    BLMatrix2D& inverseWorldMatrix = iPaintGroup->GetInverseWorldMatrix();
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

    if( FOdysseyVector::IntersectRegions<double>( rect, iPaintGroup->GetBBox(false), isxRect ) )
    {
        std::list<FOdysseyVectorObject*>& childrenList = iPaintGroup->GetChildrenList();
        std::list<FOdysseyVectorObject*>::iterator it;

        for( it = childrenList.begin(); it != childrenList.end(); ++it )
        {
            FOdysseyVectorObject* child = *it;

            if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

                CutPath( path, oAddedVertexArray, oAddedSegmentArray, oRemovedSegmentArray );
            }
        }
    }
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;

    // crashes if I don't reserve. Why that ?
    removedSegmentArray.reserve(50);
    addedSegmentArray.reserve(50);
    addedVertexArray.reserve(50);

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath *path = static_cast<FOdysseyVectorPath*>(selectedObject);

            CutPath( path, addedVertexArray, addedSegmentArray, removedSegmentArray );
        }

        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint *paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            CutPaintGroup( paintGroup, addedVertexArray, addedSegmentArray, removedSegmentArray );
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathCutTool","Vector Path Cut Tool"));
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

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iEngine->ResetHUD();

    iScene->Update( 0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
