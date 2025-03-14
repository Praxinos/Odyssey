// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
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
    , PickingRadius( 75 )
    , MatchingInfluence( eMatchingInfluence::Radial )
    , Rigidity( 5 )
    , RigidifySelectionOnly( false )
    , ShowInbetweens ( false )
    , GridDisplayMode ( eMatchingGridDisplayMode::AsQuads )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching64");

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

uint64
UOdysseyPainterEditorVectorMatchingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

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
                                                              , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                                    , mMatchingHUD->GetSelectedBreakdownList()
                                                                                    , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

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

    oSignalFlags = notificationFlags;
    return true;
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , uint64& oSignalFlags )
{
    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;

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
            BLPoint localDiff = gridPoint->GetGrid()->GetBreakdown()->GetTargetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
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


    oSignalFlags = notificationFlags;
}

bool
UOdysseyPainterEditorVectorMatchingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    oSignalFlags =notificationFlags;
    return true;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    iScene->GetCell()->ResetHUD();

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

void
UOdysseyPainterEditorVectorMatchingTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, PickingRadius ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, Rigidity ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, RigidifySelectionOnly ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMatchingTool, ShowInbetweens ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorMatchingTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Matching Tool");
}

#undef LOCTEXT_NAMESPACE
