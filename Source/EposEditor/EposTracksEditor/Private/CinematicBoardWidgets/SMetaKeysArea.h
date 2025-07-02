// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;
class FMetaChannel;
class FScopedTransaction;

class SMetaKeysArea
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SMetaKeysArea )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

    virtual const FSlateBrush* GetBackgroundBrush() const = 0;

    virtual int32 DrawBackground( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const;
    virtual int32 DrawKeys( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const;

protected:
    enum class EDragMode
    {
        kMoveSingleKey,
        kShiftFromKey,
    };

protected:
    virtual TSharedPtr<FMetaChannel> CreateKeysUnderMouse( const FPointerEvent& MouseEvent ) const;

    virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys );

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const;
    virtual FText GetAreaTooltipText() const;

protected:
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() = 0;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const = 0;

    virtual void RebuildMetaChannel() = 0;

    virtual void ComputeClampRangePreMoveDuringDrag( TSharedPtr<FMetaChannel> iKeys, TRange<FFrameNumber>& oClampRangeInSubsequence ) const;

    virtual void OnStartDragKeys( TSharedPtr<FMetaChannel> iKeys );
    virtual void OnDragKeys( TSharedPtr<FMetaChannel> iKeys );
    virtual void OnStopDragKeys( TSharedPtr<FMetaChannel> iKeys );

    virtual void OnClickKeys( TSharedPtr<FMetaChannel> iKeys );

    virtual EDragMode InitDragMode() const;

    virtual bool ExcludeKey( FFrameNumber iFrameNumber ) const;

private:
    /** Start a transaction at mouse down */
    void BeginTransaction( const FText& iTransactionDesc );
    /** End the transaction at mouse up */
    void EndTransaction();

    /** Scoped transaction for this drag operation */
    TUniquePtr<FScopedTransaction>      mTransaction;

    /** Not virtual. Override GetKeyTooltipText and GetAreaTooltipText to set tooltip content */
    FText GetTooltipText() const;

protected:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;

    mutable EDragMode mDragMode = EDragMode::kMoveSingleKey; // mutable because modified in const InitDragMode()

private:
    enum class EState
    {
        kIdle,
        kPressing,
        kDragging,
    };
    EState mState { EState::kIdle };

    TSharedPtr<FMetaChannel> mDraggedKeys;
    TSharedPtr<FMetaChannel> mHoveredKeys;
};
