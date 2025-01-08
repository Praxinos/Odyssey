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
#include "OdysseyVectorRoot.h"
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
    , EditionMode ( eChartEditionMode::OneByOne )
    , ChartType ( eChartType::Partial )
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
                                                           , const FKeyEvent& InKeyEvent
                                                           , uint64& oSignalFlags )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        EditionModeAtKeyDown = EditionMode;

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            //EditionMode  = eChartEditionMode::Relative;
            EditionMode = ( EditionMode == eChartEditionMode::Relative ) ? eChartEditionMode::OneByOne
                                                                         : eChartEditionMode::Relative;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            //EditionMode  = eChartEditionMode::EaseInOrOut;
            EditionMode = ( EditionMode == eChartEditionMode::EaseInOrOut ) ? eChartEditionMode::OneByOne
                                                                            : eChartEditionMode::EaseInOrOut;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            //EditionMode  = eChartEditionMode::Magnet;
            EditionMode = ( EditionMode == eChartEditionMode::Magnet ) ? eChartEditionMode::OneByOne
                                                                       : eChartEditionMode::Magnet;
        }

        // force redraw
        iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorChartTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKeyEvent& InKeyEvent
                                                         , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    FKey key = InKeyEvent.GetKey();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
      || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand )
      || ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   )
      || ( key == EKeys::LeftAlt     ) || ( key == EKeys::RightAlt     ) )
    {
        // redraw
        iScene->GetEngine()->Invalidate( 0 );
    }

    // first reset display mode
    EditionMode = EditionModeAtKeyDown;


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

    mPickedBreakdown = nullptr;
    mPickedInbetween = nullptr;
    mPickedBezierPoint = nullptr;
    mEasing = 0.0f;

    mMouseAtDown.x = iPointInTexture.x;
    mMouseAtDown.y = iPointInTexture.y;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mChartHUD->GetBreakdownList().size() )
        {
            if( ( EditionMode == eChartEditionMode::OneByOne    )
            ||  ( EditionMode == eChartEditionMode::Relative    )
            ||  ( EditionMode == eChartEditionMode::EaseInOrOut )
            ||  ( EditionMode == eChartEditionMode::Magnet      ) )
            {
                mPickedInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                           , iPointInTexture.y );

                if( mPickedInbetween )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();

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
                else
                {
                    mPickedBreakdown = mChartHUD->PickBreakdown( iPointInTexture.x
                                                               , iPointInTexture.y
                                                               , PickingRadius );
                }
            }

            if( EditionMode == eChartEditionMode::Reshape )
            {
                mPickedBezierPoint = mChartHUD->PickBezierPoint( iPointInTexture.x
                                                               , iPointInTexture.y
                                                               , PickingRadius );

                if( mPickedBezierPoint )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedBezierPoint->GetHUDBezier()->GetChart()->GetBreakdown()->GetInbetweenerTag();

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

    if( ( EditionMode == eChartEditionMode::OneByOne    )
     || ( EditionMode == eChartEditionMode::Relative    )
     || ( EditionMode == eChartEditionMode::Magnet      ) )
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
        if( mChartHUD->GetBreakdownList().size() )
        {
            if( EditionMode == eChartEditionMode::OneByOne )
            {
                if( mPickedInbetween )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();
                    double newT = mPickedInbetween->GetChart()->GetHUDBezier()->HitTest( ::ULIS::FVec2D( pointInTexture.x
                                                                                                       , pointInTexture.y )
                                                                                       , INT_MAX );

                    inbetweenerTag->MoveInbetween( mPickedInbetween
                                                 , newT
                                                 , false );
                }
            }

            if( EditionMode == eChartEditionMode::Relative )
            {
                if ( mPickedInbetween )
                {
                    double newT = mPickedInbetween->GetChart()->GetHUDBezier()->HitTest( ::ULIS::FVec2D( pointInTexture.x
                                                                                                        , pointInTexture.y )
                                                                                        , INT_MAX );

                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();

                    inbetweenerTag->MoveInbetween( mPickedInbetween
                                                    , newT
                                                    , true );
                }
            }

            if( EditionMode == eChartEditionMode::EaseInOrOut )
            {
                // applies to the current breakdown, not only the one next to a click.
                for( FInbetweenerBreakdown* breakdown : mChartHUD->GetBreakdownList() )
                {
                    if( pointInTexture.x < mMouseAtDown.x )
                    {
                        breakdown->EaseIn( mEasing );

                        mEasing = std::clamp( ( pointInTexture.deltaPosition.X < 0.0f ) ? mEasing + 0.2f
                                                                                        : mEasing - 0.2f, 0.0f, 1.0f );
                    }

                    if( pointInTexture.x > mMouseAtDown.x )
                    {
                        breakdown->EaseOut( mEasing );

                        mEasing = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mEasing + 0.2f
                                                                                        : mEasing - 0.2f, 0.0f, 1.0f );
                    }
                }
            }

            if( EditionMode == eChartEditionMode::Magnet )
            {
                if( mPickedInbetween )
                {
                    mPickedInbetween->GetChart()->GetBreakdown()->EaseInAndOut( mEasing, mPickedInbetween );

                    mEasing = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mEasing + 0.2f
                                                                                    : mEasing - 0.2f, -1.0f, 1.0f );
                }
            }

            if( EditionMode == eChartEditionMode::Reshape )
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
                    // we will move all breakdowns chart if no point is picked.
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mChartHUD->GetBreakdownList().size() ? mChartHUD->GetBreakdownList().front()->GetInbetweenerTag()
                                                                                                        : nullptr;

                    for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
                    {
                        FInbetweenerChart::HUDBezier* HUDBezier = breakdown->GetChart()->GetHUDBezier();
                        ::ULIS::FVec2D pointPosition[3] = { HUDBezier->GetPoints()[0].GetPosition() + deltaPosition
                                                          , HUDBezier->GetPoints()[1].GetPosition() + deltaPosition
                                                          , HUDBezier->GetPoints()[2].GetPosition() + deltaPosition };

                        if( breakdown->GetIndex() == 0 )
                        {
                            HUDBezier->GetPoints()[0].SetPosition( pointPosition[0].x, pointPosition[0].y );
                        }

                        HUDBezier->GetPoints()[1].SetPosition( pointPosition[1].x, pointPosition[1].y );
                        HUDBezier->GetPoints()[2].SetPosition( pointPosition[2].x, pointPosition[2].y );
                    }
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

uint64
UOdysseyPainterEditorVectorChartTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    // force redraw for this scene
    iScene->Invalidate( 0 );
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return 0;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorChartTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;

    const TSharedPtr<ISinglePropertyView> EditionModeView = propertyEditorModule.CreateSingleProperty( this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorChartTool, EditionMode ), defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> ChartTypePropertyView = propertyEditorModule.CreateSingleProperty(this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorChartTool, ChartType ), defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> EditionModeHandle = EditionModeView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> ChartTypeHandle = ChartTypePropertyView->GetPropertyHandle();

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
            CreatePropertyWidget(EditionModeHandle, EditionModeView).ToSharedRef()
        ]

        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(ChartTypeHandle, ChartTypePropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorChartTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Chart Tool");
}

#undef LOCTEXT_NAMESPACE
