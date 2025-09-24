// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathStitchTool::~UOdysseyPainterEditorVectorPathStitchTool()
{
}

UOdysseyPainterEditorVectorPathStitchTool::UOdysseyPainterEditorVectorPathStitchTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPathStitchToolHUD>( this ), false, true )
    , PickingRadius(20.0f)
//    , RestrictToSelection( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathStitch64");

    mPathStitchHUD = static_cast<FOdysseyPainterEditorVectorPathStitchToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathStitchTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                              , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        // we need to reset the QuadTree that we use for faster picking
        if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TOPOLOGY] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TOPOLOGY] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE] ) )
        {
            if( mBaseHUD )
            {
                mBaseHUD->Reset();
            }
        }
    }

    // will react to OBJECT_SELECTION and CHILD_OBJECT_SELECTION.
    // Will also Reset the HUD. the HUD in that case might be reset twice
    UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( iInvalidationFlags, iUpdateFlags );
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorVertex* knotVertex;
        // for undos
        std::vector<FOdysseyVectorPath*> addedPathArray; // stays empty
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorPath*> removedPathArray; // receives the merged path if any
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
        std::vector<FOdysseyVectorVertex*> mergedVertexArray;
        FOdysseyVectorVertex** stitchableVertex = mPathStitchHUD->GetStitchableVertices();

        //pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.

    /*
        iEngine->PickPoints( iScene
                           , false
                           , iPointInTexture.x
                           , iPointInTexture.y
                           , PickingRadius
                           , pickedPointArray
                           , FOdysseyVectorPath::PICK_POINT );
    */
        if( stitchableVertex[0] && stitchableVertex[1] )
        {
            FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( stitchableVertex[0] );
            FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( stitchableVertex[1] );
            FOdysseyVectorPath* mergedPath = nullptr;

            // TODO: remove vertexB->GetPath() from selected objects.
            if( vertexA->GetOwnerAsPath() != vertexB->GetOwnerAsPath() )
            {
                mergedPath = vertexB->GetOwnerAsPath();

                vertexB->GetOwnerAsPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetOwnerAsPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );


                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = mergedVertexArray[vertexB->GetID()];

                if( mergedPath->IsSelected() )
                {
                    iScene->GetCell()->UnselectObject( mergedPath );

                    iScene->GetCell()->SelectObject( vertexA->GetOwnerAsPath() );
                }

                removedPathArray.push_back( mergedPath );
            }

            knotVertex = FOdysseyVectorCell::Stitch( vertexA, vertexB, addedSegmentArray, removedSegmentArray, true );

            if( knotVertex )
            {
                addedVertexArray.push_back( knotVertex );
                removedVertexArray.push_back( vertexA );
                removedVertexArray.push_back( vertexB );

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-path-stitch-tool.transaction.stitch-path","Vector Path Stitch Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPathStitch( iScene
                                                                               , removedPathArray
                                                                               , removedVertexArray
                                                                               , removedSegmentArray
                                                                               , addedPathArray
                                                                               , addedVertexArray
                                                                               , addedSegmentArray
                                                                               , mergedVertexArray
                                                                               , mergedSegmentArray );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS
                                  | FOdysseyVectorObject::UPDATE_INTERACTIVE );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    mPathStitchHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathStitchHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    mPathStitchHUD->Reset();

    return true;
}

void
UOdysseyPainterEditorVectorPathStitchTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FName& iPropertyName )
{
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

void
UOdysseyPainterEditorVectorPathStitchTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathStitchTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorPathStitchTool::GetTooltip() const
{
    return LOCTEXT("vector-path-stitch-tool.tooltip", "Path Stitch Tool");
}

bool UOdysseyPainterEditorVectorPathStitchTool::IsSameAs(const UOdysseyPainterEditorTool* Other) const
{
    // Same class verification
    if (!UOdysseyPainterEditorTool::IsSameAs(Other))
        return false;

    const UOdysseyPainterEditorVectorPathStitchTool* otherTool = Cast< UOdysseyPainterEditorVectorPathStitchTool >(Other);

    return  PickingRadius == otherTool->PickingRadius;
}

#undef LOCTEXT_NAMESPACE
