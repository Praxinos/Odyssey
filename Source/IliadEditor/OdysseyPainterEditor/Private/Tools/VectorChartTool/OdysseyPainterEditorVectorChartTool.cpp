// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    , mUndo( nullptr )
    , PickingRadius( 10.0f )
    , EditionMode ( eVectorChartEditionMode::OneByOne )
    , Factor( 1 )
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
                                                           , const FKeyEvent& InKeyEvent )
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
                EditionMode  = eVectorChartEditionMode::Reshape;
            }
            else
            {
                EditionMode  = eVectorChartEditionMode::Relative;
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
                EditionMode  = eVectorChartEditionMode::Reshape;
            }
            else
            {
                EditionMode  = eVectorChartEditionMode::EaseInOrOut;
            }

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            EditionMode  = eVectorChartEditionMode::Magnet;

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorChartTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKeyEvent& InKeyEvent )
{
    EditionMode = eVectorChartEditionMode::OneByOne;

    return false;
}

void
UOdysseyPainterEditorVectorChartTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                         , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        // we need to reset when a tag is removed
        if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_BREAKDOWN_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_BREAKDOWN_LIST] ) )
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
UOdysseyPainterEditorVectorChartTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

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
            if( ( EditionMode == eVectorChartEditionMode::OneByOne    )
            ||  ( EditionMode == eVectorChartEditionMode::Relative    )
            ||  ( EditionMode == eVectorChartEditionMode::Magnet      ) )
            {
                mPickedInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                           , iPointInTexture.y );

                if( mPickedInbetween )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedInbetween->GetChart()->GetBreakdown()->GetInbetweenerTag();
                    mUndo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                          , inbetweenerTag );
                    mUndo->Begin(); // snapshot beforce changes

                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-chart-tool.transaction.edit-chart","Vector Chart Tool"));
                    if( GUndo )
                    {
                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();
                }
            }

            if( EditionMode == eVectorChartEditionMode::EaseInOrOut )
            {
                mPickedBreakdown = mChartHUD->PickBreakdown( iPointInTexture.x
                                                           , iPointInTexture.y
                                                           , PickingRadius );
            }

            if( EditionMode == eVectorChartEditionMode::Reshape )
            {
                mPickedBezierPoint = mChartHUD->PickBezierPoint( iPointInTexture.x
                                                               , iPointInTexture.y
                                                               , PickingRadius );

                if( mPickedBezierPoint )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = mPickedBezierPoint->GetHUDBezier()->GetChart()->GetBreakdown()->GetInbetweenerTag();
                    mUndo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                          , inbetweenerTag );
                    mUndo->Begin(); // snapshot beforce changes

                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-chart-tool.transaction.edit-chart","Vector Chart Tool"));
                    if( GUndo )
                    {
                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

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

    return true;
}

FInbetweenerChart::Inbetween*
UOdysseyPainterEditorVectorChartTool::GetHoveredInbetween()
{
    return mHoveredInbetween;
}

void
UOdysseyPainterEditorVectorChartTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    mHoveredInbetween = nullptr;

    if( ( EditionMode == eVectorChartEditionMode::OneByOne    )
     || ( EditionMode == eVectorChartEditionMode::Relative    )
     || ( EditionMode == eVectorChartEditionMode::Magnet      ) )
    {
        // TODO: highlight grid handles ?
        mHoveredInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                    , iPointInTexture.y );
    }
}

void
UOdysseyPainterEditorVectorChartTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );
    FOdysseyPoint pointInTexture = iPointInTexture;

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
            if( EditionMode == eVectorChartEditionMode::OneByOne )
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

            if( EditionMode == eVectorChartEditionMode::Relative )
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

            if( EditionMode == eVectorChartEditionMode::EaseInOrOut )
            {
                // applies to the current breakdown, not only the one next to a click.
                if ( mPickedBreakdown )
                {
                    if( pointInTexture.x < mMouseAtDown.x )
                    {
                        mPickedBreakdown->EaseIn( mStrength, Factor );

                        mStrength = std::clamp( ( pointInTexture.deltaPosition.X < 0.0f ) ? mStrength + 0.2f
                                                                                          : mStrength - 0.2f, 0.0f, 1.0f );
                    }

                    if( pointInTexture.x > mMouseAtDown.x )
                    {
                        mPickedBreakdown->EaseOut( mStrength, Factor );

                        mStrength = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mStrength + 0.2f
                                                                                          : mStrength - 0.2f, 0.0f, 1.0f );
                    }
                }
            }

            if( EditionMode == eVectorChartEditionMode::Magnet )
            {
                if( mPickedInbetween )
                {
                    mPickedInbetween->GetChart()->GetBreakdown()->EaseInAndOut( mStrength, Factor, mPickedInbetween );

                    mStrength = std::clamp( ( pointInTexture.deltaPosition.X > 0.0f ) ? mStrength + 0.2f
                                                                                      : mStrength - 0.2f, -1.0f, 1.0f );
                }
            }

            if( EditionMode == eVectorChartEditionMode::Reshape )
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
}

bool
UOdysseyPainterEditorVectorChartTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

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

        if( mUndo )
        {
            mUndo->End();

            mUndo = nullptr;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorChartTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FName& iPropertyName )
{
    iScene->GetLayer()->ResetHUD( iScene );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

eVectorChartEditionMode
UOdysseyPainterEditorVectorChartTool::GetEditionMode()
{
    return EditionMode;
}

void
UOdysseyPainterEditorVectorChartTool::SetEditionMode( eVectorChartEditionMode iMode )
{
    EditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorChartTool::GetBackgroundColor( eVectorChartEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == EditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorChartTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorChartEditionMode>)
           .Value_Lambda( [this]{ return EditionMode; } )
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
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.ctrlshift.name", "Reshape (CTRL + SHIFT)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-chart-tool.edition-mode.cmdshift.name", "Reshape (CMD + SHIFT)") )
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
UOdysseyPainterEditorVectorChartTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateModifierSegmentControl(),
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorChartTool, ChartType ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorChartTool::GetTooltip() const
{
    return LOCTEXT("vector-chart-tool.tooltip", "Chart Tool");
}

#undef LOCTEXT_NAMESPACE
