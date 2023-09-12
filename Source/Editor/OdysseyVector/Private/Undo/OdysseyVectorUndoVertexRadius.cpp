#include "Undo/OdysseyVectorUndoVertexRadius.h"

FOdysseyVectorUndoVertexRadius::~FOdysseyVectorUndoVertexRadius()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }

    mVertexRadiusArray.clear();
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                                              , std::vector<FOdysseyVectorPoint*>& iPointArray
                                                              , bool iAlterAllAlong )
    : FOdysseyVectorUndo( iScene )
    , mAlterAllAlong( iAlterAllAlong )
{
    mVertexRadiusArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        if( iPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(iPointArray[i]);

            mVertexRadiusArray.push_back( FVertexRadius( vertex ) );
        }
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorVertex* iVertex
                                                              , bool iAlterAllAlong )
    : FOdysseyVectorUndo( iScene )
    , mAlterAllAlong( iAlterAllAlong )
{
    mVertexRadiusArray.push_back( FVertexRadius( iVertex ) );
}

static void
LoadArray( std::vector<FVertexRadius>& iVertexRadiusArray, bool iAlterAllAlong )
{
    for( int i = 0; i < iVertexRadiusArray.size(); i++ )
    {
        FVertexRadius formerRadius = FVertexRadius( iVertexRadiusArray[i].vertex );
        double deltaRadius = iVertexRadiusArray[i].radius - iVertexRadiusArray[i].vertex->GetRadius();

        // Note: virtual function Set() will invalidate segments in needed
        iVertexRadiusArray[i].vertex->AlterRadius( nullptr, deltaRadius, iAlterAllAlong );

        // replace with former value (prepare for the counterpart operation, either Apply or Revert)
        iVertexRadiusArray[i] = formerRadius;
    }
}

void
FOdysseyVectorUndoVertexRadius::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    LoadArray( mVertexRadiusArray, mAlterAllAlong );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoVertexRadius::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    LoadArray( mVertexRadiusArray, mAlterAllAlong );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexRadius::ToString() const
{
    return FString("FOdysseyVectorUndoVertexRadius");
}
