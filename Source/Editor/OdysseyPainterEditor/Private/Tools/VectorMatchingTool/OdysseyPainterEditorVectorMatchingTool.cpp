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
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include <chrono>
#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorMatchingTool::~UOdysseyPainterEditorVectorMatchingTool()
{
}

UOdysseyPainterEditorVectorMatchingTool::UOdysseyPainterEditorVectorMatchingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorMatchingToolHUD( this ), false )
    , PickingRadius( 75 )
    , Rigidity( 5 )
    , RigidifySelectionOnly( false )
    , ShowInbetweens ( false )
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
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

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

    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    mPickedPointArray.clear();
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
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    oSignalFlags = notificationFlags;
	return true;
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           	, const FOdysseyPoint& iPointInTexture
                                            				, uint64& oSignalFlags )
{
    // TODO: highlight grid handles ?

    mMatchingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // redraw
    iScene->GetEngine()->Invalidate( 0 );
}

void
UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          	, const FOdysseyPoint& iPointInTexture
                                            				, uint64& oSignalFlags )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 notificationFlags = 0;

//UE_LOG(LogTemp, Warning, TEXT("UOdysseyPainterEditorVectorMatchingTool::OnMouseDragVector %d") );

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( engine->GetInvalidationFlags()  )
        return;

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
        iScene->GetSharedEnv()->Update( /*FOdysseyVectorObject::UPDATE_INTERACTIVE*/0 );
    }

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

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

    FOdysseyVectorEngine* engine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    oSignalFlags =notificationFlags;
	return true;
}

uint64
UOdysseyPainterEditorVectorMatchingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorMatchingTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> pickingRadiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "PickingRadius", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> pickingRadiusHandle = pickingRadiusPropertyView->GetPropertyHandle();
    const TSharedPtr<ISinglePropertyView> rigidityPropertyView = propertyEditorModule.CreateSingleProperty(this, "Rigidity", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> rigidityHandle = rigidityPropertyView->GetPropertyHandle();
    const TSharedPtr<ISinglePropertyView> rigidifySelectionOnlyPropertyView = propertyEditorModule.CreateSingleProperty(this, "RigidifySelectionOnly", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> rigidifySelectionOnlyHandle = rigidifySelectionOnlyPropertyView->GetPropertyHandle();
    const TSharedPtr<ISinglePropertyView> showInbetweensPropertyView = propertyEditorModule.CreateSingleProperty(this, "ShowInbetweens", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> showInbetweensOnlyHandle = showInbetweensPropertyView->GetPropertyHandle();

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
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(rigidityHandle, rigidityPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(rigidifySelectionOnlyHandle, rigidifySelectionOnlyPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(showInbetweensOnlyHandle, showInbetweensPropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorMatchingTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Matching Tool");
}

#undef LOCTEXT_NAMESPACE
