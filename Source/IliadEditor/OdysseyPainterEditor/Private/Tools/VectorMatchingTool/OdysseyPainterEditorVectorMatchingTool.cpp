// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"

#include "Editor.h"
#include "ISinglePropertyView.h"
#include "PropertyEditorModule.h"
#include "ToolMenu.h"
#include "Widgets/Layout/SBox.h"

#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include "SOdysseySinglePropertyView.h"

#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorMatchingTool::~UOdysseyPainterEditorVectorMatchingTool()
{
}

UOdysseyPainterEditorVectorMatchingTool::UOdysseyPainterEditorVectorMatchingTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorMatchingToolHUD>( this ), false, true )
    , mUndo ( nullptr )
    , ShowInbetweens ( eShowInbetweens::None )
    , PickingRadius( 75 )
    , MatchingInfluence( eMatchingInfluence::Radial )
    , Rigidity( 5 )
    , RigidifySelectionOnly( false )
    , GridDisplayMode ( eMatchingGridDisplayMode::AsQuads )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Matching64"));

    mMatchingHUD = static_cast<FOdysseyPainterEditorVectorMatchingToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorMatchingTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN );
}

bool
UOdysseyPainterEditorVectorMatchingTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorMatchingTool::SetRadius(float iRadius)
{
    PickingRadius = iRadius;
}

float
UOdysseyPainterEditorVectorMatchingTool::GetRadius() const
{
    return PickingRadius;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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
UOdysseyPainterEditorVectorMatchingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorMatchingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    mPickedPointArray.clear();
    mWorldDistanceArray.clear();
    mPickedGridArray.clear();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer

        GEditor->BeginTransaction(LOCTEXT("vector-matching-tool.transaction.match-grid","Vector Matching Tool"));
        if( GUndo )
        {
            mUndo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                , mMatchingHUD->GetSelectedBreakdownList() );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        mUndo->Begin(); // snapshot before changes

        for( FInbetweenerBreakdown* breakdown : mMatchingHUD->GetSelectedBreakdownList() )
        {
            mMatchingHUD->PickTargetPoints( breakdown
                                          , iPointInTexture.x
                                          , iPointInTexture.y
                                          , PickingRadius
                                          , mPickedPointArray
                                          , mWorldDistanceArray
                                          , mPickedGridArray );
        }

        for( FInbetweenerGrid* grid : mPickedGridArray )
        {
            for( FInbetweenerPoint& point : grid->GetPointBuffer() )
            {
                point.SetDeformable( RigidifySelectionOnly ? false : true );
            }
        }
    }

    // redraw
    //iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
//UE_LOG(LogTemp, Warning, TEXT("UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector %d") );

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( iScene->GetCell()->PendingRedraw() )
        return;

    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        for( uint32 i = 0; i < mPickedPointArray.size(); i++ )
        {
            FInbetweenerPoint* gridPoint = mPickedPointArray[i];
            float ratio = ( PickingRadius && MatchingInfluence == eMatchingInfluence::Radial ) ? 1.0f - ( float ) ( mWorldDistanceArray[i] / PickingRadius ) : 1.0f;
            ::ULIS::FVec2D targetPosition = gridPoint->GetTargetPosition();
            BLPoint localDiff = gridPoint->GetGrid()->GetBreakdown()->GetTargetInverseWorldMatrix().map_vector( iPointInTexture.deltaPosition.X
                                                                                                             , iPointInTexture.deltaPosition.Y );

            targetPosition.x += ( localDiff.x * ratio );
            targetPosition.y += ( localDiff.y * ratio );

            gridPoint->SetTargetPosition( targetPosition.x, targetPosition.y, true );

            if( RigidifySelectionOnly )
            {
                gridPoint->SetDeformable( true );
            }
        }

        for( FInbetweenerGrid* grid : mPickedGridArray )
        {
            if( grid->GetBreakdown()->GetInbetweenerTag()->GetGridType() == eInbetweenerGridType::ARAP )
            {
                FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(grid);

                arapGrid->Regularize( Rigidity );
            }
        }

        // update
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }
}

bool
UOdysseyPainterEditorVectorMatchingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        if( mUndo )
        {
            mUndo->End();

            mUndo = nullptr;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorMatchingTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                            , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_BREAKDOWN_LIST] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_BREAKDOWN_LIST] ) )
    {
        if( mBaseHUD )
        {
            mBaseHUD->Reset();
        }
    }

    // will react to OBJECT_SELECTION and CHILD_OBJECT_SELECTION.
    // Will also Reset the HUD. the HUD in that case might be reset twice
    UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( iInvalidationFlags, iUpdateFlags );
}

void
UOdysseyPainterEditorVectorMatchingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    iScene->GetLayer()->ResetHUD( iScene );

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

void
UOdysseyPainterEditorVectorMatchingTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, ShowInbetweens ), FSinglePropertyParams())
                .InnerPadding(10.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, Rigidity ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, RigidifySelectionOnly ), FSinglePropertyParams())
                .InnerPadding(10.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorMatchingTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Matching Tool");
}

#undef LOCTEXT_NAMESPACE
