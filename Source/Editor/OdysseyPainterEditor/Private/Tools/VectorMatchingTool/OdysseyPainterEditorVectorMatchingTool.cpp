// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorAnimationCell.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorMatchingTool::~UOdysseyPainterEditorVectorMatchingTool()
{
}

UOdysseyPainterEditorVectorMatchingTool::UOdysseyPainterEditorVectorMatchingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorMatchingToolHUD( this ), false )
    , PickingRadius( 75.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching64");

    mMatchingHUD = static_cast<FOdysseyPainterEditorVectorMatchingToolHUD*>( mBaseHUD );
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
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    mPickedPointArray.clear();
    mPickedGridArray.clear();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
/*
        GEditor->BeginTransaction(LOCTEXT("vector-matching-tool.transaction.match-grid","Vector Matching Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                                    , mPickedInbetweenerTag
                                                                                    , retFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
*/
        for( FInbetweenerBreakdown* breakdown : mMatchingHUD->GetSelectedBreakdownList() )
        {
            mMatchingHUD->PickTargetPoints( breakdown
                                          , iPointInTexture.x
                                          , iPointInTexture.y
                                          , PickingRadius
                                          , mPickedPointArray
                                          , mPickedGridArray );
        }
    }

    return retFlags | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // TODO: highlight grid handles ?

    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        for( FInbetweenerPoint* gridPoint : mPickedPointArray )
        {
            ::ULIS::FVec2D targetPosition = gridPoint->GetTargetPosition();
            BLPoint localDiff = gridPoint->GetGrid()->GetBreakdown()->GetTargetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                                                             , iPointInTexture.deltaPosition.Y );

            targetPosition.x += localDiff.x;
            targetPosition.y += localDiff.y;

            gridPoint->SetTargetPosition( targetPosition.x, targetPosition.y );
        }

        for( FInbetweenerGrid* grid : mPickedGridArray )
        {
            if( grid->GetBreakdown()->GetInbetweenerTag()->GetGridType() == eInbetweenerGridType::ARAP )
            {
                FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(grid);

                arapGrid->Regularize();
            }
        }

        // update
        iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    }

    return retFlags | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    return retFlags;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorMatchingTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> pickingRadiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "PickingRadius", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> pickingRadiusHandle = pickingRadiusPropertyView->GetPropertyHandle();

    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]

        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(pickingRadiusHandle, pickingRadiusPropertyView).ToSharedRef()
        ]
/*
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(YDivHandle, YDivPropertyView).ToSharedRef()
        ]
*/;
}

FText
UOdysseyPainterEditorVectorMatchingTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Matching Tool");
}

#undef LOCTEXT_NAMESPACE
