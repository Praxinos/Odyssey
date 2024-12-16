// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTagInbetweener.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorChartTool::~UOdysseyPainterEditorVectorChartTool()
{
}

UOdysseyPainterEditorVectorChartTool::UOdysseyPainterEditorVectorChartTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorChartToolHUD( this ), false )
    , PickingRadius( 10.0f )
    , mPickingMode( eChartPickingMode::Default )
    , ShiftingOp ( eChartShiftingOp::Relative )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Chart64");

    mChartHUD = static_cast<FOdysseyPainterEditorVectorChartToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorChartTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN );
}

uint64
UOdysseyPainterEditorVectorChartTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorChartTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorChartTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand ) )
    {
        mPickingMode  = eChartPickingMode::Control;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mPickingMode  = eChartPickingMode::Shift;
    }

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return false;
}

bool
UOdysseyPainterEditorVectorChartTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand )
      || ( iKey == EKeys::LeftShift   ) || ( iKey == EKeys::RightShift   )
      || ( iKey == EKeys::LeftAlt     ) || ( iKey == EKeys::RightAlt     ) )
    {
        // redraw
        iScene->GetEngine()->Invalidate( 0 );
    }

    // first reset display mode
    mPickingMode = eChartPickingMode::Default;

    return false;
}

bool
UOdysseyPainterEditorVectorChartTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey
                                                       , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    mPickedInbetween = nullptr;
    mPickedBezierPoint = nullptr;
    mEasing = 0.0f;

    mMouseAtDown.x = iPointInTexture.x;
    mMouseAtDown.y = iPointInTexture.y;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mChartHUD->GetBreakdown() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = mChartHUD->GetBreakdown()->GetInbetweenerTag();

            if( ( mPickingMode == eChartPickingMode::Default )
              ||( mPickingMode == eChartPickingMode::Shift ) )
            {
                mPickedInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                           , iPointInTexture.y );

                if( mPickedInbetween )
                {
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-chart-tool.transaction.edit-chart","Vector Chart Tool"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                                                 , inbetweenerTag
                                                                                                 , notificationFlags );

                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();
                }
            }

            if( mPickingMode == eChartPickingMode::Control )
            {
                mPickedBezierPoint = mChartHUD->PickBezierPoint( iPointInTexture.x
                                                               , iPointInTexture.y
                                                               , 10 );

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-chart-tool.transaction.edit-chart","Vector Chart Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                                             , inbetweenerTag
                                                                                             , notificationFlags );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
        }
    }
    // request redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    oSignalFlags = notificationFlags;

    return true;
}

FInbetweenerChart::Inbetween*
UOdysseyPainterEditorVectorChartTool::GetHoveredInbetween()
{
    return mHoveredInbetween;
}

void
UOdysseyPainterEditorVectorChartTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                        , uint64& oSignalFlags )
{
    mHoveredInbetween = nullptr;

    if( mPickingMode == eChartPickingMode::Default )
    {
        // TODO: highlight grid handles ?
        mHoveredInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                    , iPointInTexture.y );

        // redraw
        iScene->GetEngine()->Invalidate( 0 );
    }
}

void
UOdysseyPainterEditorVectorChartTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                        , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );
    FOdysseyPoint pointInTexture = iPointInTexture;
    uint64 notificationFlags = 0;

    // because we ignore some events, we need to accumulate the delta
    deltaPositionCumul += iPointInTexture.deltaPosition;

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( engine->GetInvalidationFlags()  )
        return;

    pointInTexture.deltaPosition = deltaPositionCumul;

    if( pointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mChartHUD->GetBreakdown() )
        {
            FInbetweenerBreakdown* currentBreakdown = mChartHUD->GetBreakdown();
            FInbetweenerChart::HUDBezier* HUDBezier = currentBreakdown->GetChart()->GetHUDBezier();

            if( mPickingMode == eChartPickingMode::Default )
            {
                if( mPickedInbetween )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();
                    double newT = currentBreakdown->GetChart()->GetHUDBezier()->HitTest( ::ULIS::FVec2D( pointInTexture.x
                                                                                                       , pointInTexture.y )
                                                                                       , INT_MAX );

                    inbetweenerTag->MoveInbetween( mPickedInbetween
                                                 , newT
                                                 , false );
                }
            }

            if( mPickingMode == eChartPickingMode::Shift )
            {
                if( ShiftingOp == eChartShiftingOp::Relative )
                {
                    double newT = currentBreakdown->GetChart()->GetHUDBezier()->HitTest( ::ULIS::FVec2D( pointInTexture.x
                                                                                                       , pointInTexture.y )
                                                                                       , INT_MAX );

                    if ( mPickedInbetween )
                    {
                        FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();

                        inbetweenerTag->MoveInbetween( mPickedInbetween
                                                     , newT
                                                     , true );
                    }
                }

                if( ShiftingOp == eChartShiftingOp::EaseInOrOut )
                {
                    if( pointInTexture.x < mMouseAtDown.x )
                    {
                        currentBreakdown->EaseIn( mEasing );

                        mEasing = std::clamp( ( pointInTexture.deltaPosition.X < 0.0f ) ? mEasing + 0.2f
                                                                                         : mEasing - 0.2f, 0.0f, 1.0f );
                    }

                    if( pointInTexture.x > mMouseAtDown.x )
                    {
                        currentBreakdown->EaseOut( mEasing );

                        mEasing = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mEasing + 0.2f
                                                                                         : mEasing - 0.2f, 0.0f, 1.0f );
                    }
                }

                if( ShiftingOp == eChartShiftingOp::EaseInAndOut )
                {
                    if( mPickedInbetween )
                    {
                        currentBreakdown->EaseInAndOut( mEasing, mPickedInbetween );

                        mEasing = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mEasing + 0.2f
                                                                                        : mEasing - 0.2f, -1.0f, 1.0f );
                    }
                }
            }

            if( mPickingMode == eChartPickingMode::Control )
            {
                ::ULIS::FVec2D deltaPosition = ::ULIS::FVec2D( pointInTexture.deltaPosition.X
                                                             , pointInTexture.deltaPosition.Y );

                if( mPickedBezierPoint )
                {
                    ::ULIS::FVec2D newPos = mPickedBezierPoint->GetPosition() + deltaPosition;

                    mPickedBezierPoint->SetPosition( newPos.x, newPos.y );
                }
                else
                {
                    ::ULIS::FVec2D pointPosition[3] = { HUDBezier->GetPoints()[0].GetPosition() + deltaPosition
                                                      , HUDBezier->GetPoints()[1].GetPosition() + deltaPosition
                                                      , HUDBezier->GetPoints()[2].GetPosition() + deltaPosition };

                    HUDBezier->GetPoints()[0].SetPosition( pointPosition[0].x, pointPosition[0].y );
                    HUDBezier->GetPoints()[1].SetPosition( pointPosition[1].x, pointPosition[1].y );
                    HUDBezier->GetPoints()[2].SetPosition( pointPosition[2].x, pointPosition[2].y );
                }

                //currentBreakdown->GetChart()->GetHUDBezier()->Update();
            }
        }
    }

    // update ALL impacted scenes.
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    // redraw the current scene as it might not be modified
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    deltaPositionCumul = FVector2D( 0.0f, 0.0f );

    oSignalFlags = notificationFlags;
}

bool
UOdysseyPainterEditorVectorChartTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 retFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedInbetween )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();

            // we need to manually redraw because no object is modified
            inbetweenerTag->RedrawCells();

             // update ALL impacted scenes
            iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        }
    }

    oSignalFlags = retFlags;
    return true;
}

eChartPickingMode
UOdysseyPainterEditorVectorChartTool::GetPickingMode()
{
    return mPickingMode;
}

uint64
UOdysseyPainterEditorVectorChartTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    return 0;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorChartTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;

    const TSharedPtr<ISinglePropertyView> ShiftingOpView = propertyEditorModule.CreateSingleProperty(this, "ShiftingOp", defaultPropertyParams);
    //const TSharedPtr<ISinglePropertyView> YDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsY", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> ShiftingOpHandle = ShiftingOpView->GetPropertyHandle();
    //TSharedPtr<class IPropertyHandle> YDivHandle = YDivPropertyView->GetPropertyHandle();

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
            CreatePropertyWidget(ShiftingOpHandle, ShiftingOpView).ToSharedRef()
        ]
/*
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(YDivHandle, YDivPropertyView).ToSharedRef()
        ]
*/;
}

FText
UOdysseyPainterEditorVectorChartTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Chart Tool");
}

#undef LOCTEXT_NAMESPACE
