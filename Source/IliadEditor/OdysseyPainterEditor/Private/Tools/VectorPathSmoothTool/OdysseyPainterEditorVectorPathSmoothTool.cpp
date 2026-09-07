// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"

#include "Editor.h"
#include "ToolMenu.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SBox.h"

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPathSmoothToolHUD>( this ), false, true )
    , mUndoSegmentReshape( nullptr )
    , SmoothingMode( ePathSmoothingMode::Round )
    , PickingRadius( 20.0f )
    , PreserveHandleLength( false )
    , RestrictToSelectedObjects( false )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.PathSmooth64"));

    mPathSmoothHUD = static_cast<FOdysseyPainterEditorVectorPathSmoothToolHUD*>( mBaseHUD.Get() );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::SetRadius(float iRadius)
{
    PickingRadius = iRadius;
}

float
UOdysseyPainterEditorVectorPathSmoothTool::GetRadius() const
{
    return PickingRadius;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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

uint64
UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKey& iKey )
{
    // backup SmoothingMode value
    SmoothingModeAtKeyDown = SmoothingMode;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        // flip the value
        SmoothingMode = ( SmoothingMode == ePathSmoothingMode::Round ) ? ePathSmoothingMode::Sharp
                                                                       : ePathSmoothingMode::Round;
        return true;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKey& iKey )
{
    // restore SmoothingMode value
    SmoothingMode = SmoothingModeAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
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
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();

    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-smooth-tool.transaction.smooth-path","Vector Path Smooth Tool"));
        if( GUndo )
        {
            mUndoSegmentReshape = new FOdysseyVectorUndoSegmentReshape( iScene );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>( mUndoSegmentReshape ) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    // Calling Update via Root will request a redraw even if root is not invalidated
    //iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;

        mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        pickedPointArray = mPathSmoothHUD->GetPickedPointArray();

        for( int i = 0; i < pickedPointArray.size(); i++ )
        {
            if( pickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(pickedPointArray[i]);

                if( ( RestrictToSelectedObjects == false ) || ( vertex->GetOwnerAsPath()->IsSelected() ) )
                {
                    // record vertex and connected segments segment for undos first
                    if( mUndoSegmentReshape->HasVertex( vertex ) == false )
                    {
                        mUndoSegmentReshape->RecordVertex( vertex );
                    }

                    // then sharp or smooth
                    if( SmoothingMode == ePathSmoothingMode::Sharp )
                    {
                        FOdysseyVectorPath::SharpSegments( vertex, PreserveHandleLength );

                        vertex->SetHandleAligned( false );
                    }

                    if( SmoothingMode == ePathSmoothingMode::Round )
                    {
                        FOdysseyVectorPath::SmoothSegments( vertex, PreserveHandleLength );

                        vertex->SetHandleAligned( true );
                    }
                }
            }
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        std::vector<FOdysseyVectorVertex*> vertexArray;
        std::vector<FOdysseyVectorSegment*> segmentArray;
        std::vector<FOdysseyVectorPoint*> pickedPointArray = mPathSmoothHUD->GetPickedPointArray();

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // update invalidated objects

        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathSmoothTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorPathSmoothTool::GetTooltip() const
{
    return LOCTEXT("vector-path-smooth-tool.tooltip", "Path Smooth Tool");
}

#undef LOCTEXT_NAMESPACE
