// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

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
                                                                               , mergedSegmentArray
                                                                               , notificationFlags );

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

    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , uint64& oSignalFlags )
{
    mPathStitchHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
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
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FName& iPropertyName )
{
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

void
UOdysseyPainterEditorVectorPathStitchTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathStitchTool, PickingRadius ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorPathStitchTool::GetTooltip() const
{
    return LOCTEXT("vector-path-stitch-tool.tooltip", "Path Stitch Tool");
}

#undef LOCTEXT_NAMESPACE
