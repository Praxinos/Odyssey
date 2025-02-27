// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "SOdysseySinglePropertyView.h"
#include "Widgets/Input/SSegmentedControl.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorChartTool::~UOdysseyPainterEditorVectorChartTool()
{
}

UOdysseyPainterEditorVectorChartTool::UOdysseyPainterEditorVectorChartTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorChartToolHUD>( this ), false, true )
    , PickingRadius( 10.0f )
    , Factor( 1 )
    , mEditionMode ( eVectorChartEditionMode::OneByOne )
    , ChartType ( eChartType::Partial )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Chart64");

    mChartHUD = static_cast<FOdysseyPainterEditorVectorChartToolHUD*>( mBaseHUD.Get() );
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
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorChartTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

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

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
            {
                mEditionMode  = eVectorChartEditionMode::Reshape;
            }
            else
            {
                mEditionMode  = eVectorChartEditionMode::Relative;
            }

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            if ( FSlateApplication::Get().GetModifierKeys().IsCommandDown() ||
                 FSlateApplication::Get().GetModifierKeys().IsControlDown() )
            {
                mEditionMode  = eVectorChartEditionMode::Reshape;
            }
            else
            {
                mEditionMode  = eVectorChartEditionMode::EaseInOrOut;
            }

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            mEditionMode  = eVectorChartEditionMode::Magnet;

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorChartTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKeyEvent& InKeyEvent
                                                         , uint64& oSignalFlags )
{
    mEditionMode = eVectorChartEditionMode::OneByOne;

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

    uint64 notificationFlags = 0;

    mPickedBreakdown = nullptr;
    mPickedInbetween = nullptr;
    mPickedBezierPoint = nullptr;
    mStrength = 0.0f;

    mMouseAtDown.x = iPointInTexture.x;
    mMouseAtDown.y = iPointInTexture.y;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mChartHUD->GetBreakdownList().size() )
        {
            if( ( mEditionMode == eVectorChartEditionMode::OneByOne    )
            ||  ( mEditionMode == eVectorChartEditionMode::Relative    )
            ||  ( mEditionMode == eVectorChartEditionMode::EaseInOrOut )
            ||  ( mEditionMode == eVectorChartEditionMode::Magnet      ) )
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

            if( mEditionMode == eVectorChartEditionMode::Reshape )
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

    // Calling Update via Root will request a redraw even if root is not invalidated
    //iScene->GetRoot()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

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

    if( ( mEditionMode == eVectorChartEditionMode::OneByOne    )
     || ( mEditionMode == eVectorChartEditionMode::Relative    )
     || ( mEditionMode == eVectorChartEditionMode::Magnet      ) )
    {
        // TODO: highlight grid handles ?
        mHoveredInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                    , iPointInTexture.y );
    }
}

void
UOdysseyPainterEditorVectorChartTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , uint64& oSignalFlags )
{
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );
    FOdysseyPoint pointInTexture = iPointInTexture;
    uint64 notificationFlags = 0;

    // because we ignore some events, we need to accumulate the delta
    deltaPositionCumul += iPointInTexture.deltaPosition;

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( iScene->GetCell()->PendingRedraw()  )
        return;

    pointInTexture.deltaPosition = deltaPositionCumul;

    if( pointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mChartHUD->GetBreakdownList().size() )
        {
            if( mEditionMode == eVectorChartEditionMode::OneByOne )
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

            if( mEditionMode == eVectorChartEditionMode::Relative )
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

            if( mEditionMode == eVectorChartEditionMode::EaseInOrOut )
            {
                // applies to the current breakdown, not only the one next to a click.
                for( FInbetweenerBreakdown* breakdown : mChartHUD->GetBreakdownList() )
                {
                    if( pointInTexture.x < mMouseAtDown.x )
                    {
                        breakdown->EaseIn( mStrength, Factor );

                        mStrength = std::clamp( ( pointInTexture.deltaPosition.X < 0.0f ) ? mStrength + 0.2f
                                                                                          : mStrength - 0.2f, 0.0f, 1.0f );
                    }

                    if( pointInTexture.x > mMouseAtDown.x )
                    {
                        breakdown->EaseOut( mStrength, Factor );

                        mStrength = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mStrength + 0.2f
                                                                                          : mStrength - 0.2f, 0.0f, 1.0f );
                    }
                }
            }

            if( mEditionMode == eVectorChartEditionMode::Magnet )
            {
                if( mPickedInbetween )
                {
                    mPickedInbetween->GetChart()->GetBreakdown()->EaseInAndOut( mStrength, Factor, mPickedInbetween );

                    mStrength = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mStrength + 0.2f
                                                                                      : mStrength - 0.2f, -1.0f, 1.0f );
                }
            }

            if( mEditionMode == eVectorChartEditionMode::Reshape )
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
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

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

    uint64 retFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedInbetween )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();

            // we need to manually redraw because no object is modified
            //inbetweenerTag->RedrawCells();

             // update ALL impacted scenes
            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
        }
    }

    oSignalFlags = retFlags;

    return true;
}

uint64
UOdysseyPainterEditorVectorChartTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FName& iPropertyName )
{
    iScene->GetCell()->ResetHUD();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

eVectorChartEditionMode
UOdysseyPainterEditorVectorChartTool::GetEditionMode()
{
    return mEditionMode;
}

void
UOdysseyPainterEditorVectorChartTool::SetEditionMode( eVectorChartEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorChartTool::GetBackgroundColor( eVectorChartEditionMode iMode ) const
{
    static FSlateColorBrush orange = FSlateColorBrush( FLinearColor( 1.0f, 0.5f, 0.0f, 0.5f ) );

    return ( iMode == mEditionMode ) ? &orange : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorChartTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorChartEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<eVectorChartEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorChartTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorChartEditionMode>::Slot( eVectorChartEditionMode::OneByOne )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartOnyByOne20") )
           .ToolTip( LOCTEXT("vector-trajectory-tool.edition-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorChartTool::GetBackgroundColor, eVectorChartEditionMode::OneByOne  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartOnyByOne20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<eVectorChartEditionMode>::Slot( eVectorChartEditionMode::Relative )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartRelative20") )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.ctrl.name", "Relative (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.cmd.name", "Relative (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorChartTool::GetBackgroundColor, eVectorChartEditionMode::Relative  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartRelative20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<eVectorChartEditionMode>::Slot( eVectorChartEditionMode::EaseInOrOut )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartEaseInOrOut20") )
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.shift.name", "EaseInOrOut (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorChartTool::GetBackgroundColor, eVectorChartEditionMode::EaseInOrOut  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartEaseInOrOut20") )
               ]
           ]
           // ALT
           + SSegmentedControl<eVectorChartEditionMode>::Slot( eVectorChartEditionMode::Magnet )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartMagnet20") )
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.alt.name", "Magnet (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorChartTool::GetBackgroundColor, eVectorChartEditionMode::Magnet  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartMagnet20") )
               ]
           ]
           // CTRL + SHIFT
           + SSegmentedControl<eVectorChartEditionMode>::Slot( eVectorChartEditionMode::Reshape )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartReshape20") )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.alt.name", "Reshape (CTRL + SHIFT)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.alt.name", "Reshape (CMD + SHIFT)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorChartTool::GetBackgroundColor, eVectorChartEditionMode::Reshape  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.ChartReshape20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorChartTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        CreateModifierSegmentControl()
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorChartTool, ChartType ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorChartTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Chart Tool");
}

#undef LOCTEXT_NAMESPACE
