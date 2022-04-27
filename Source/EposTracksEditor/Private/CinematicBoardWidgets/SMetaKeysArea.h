// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

protected:
    virtual TSharedPtr<FMetaChannel> CreateKeysUnderMouse( const FPointerEvent& MouseEvent ) const;

    virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys );

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const;
    virtual FText GetAreaTooltipText() const;

protected:
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() = 0;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const = 0;

    virtual void RebuildMetaChannel() = 0;

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

private:
    enum class EState
    {
        kIdle,
        kDragging,
    };
    EState mState { EState::kIdle };

    TSharedPtr<FMetaChannel> mDraggedKeys;
    TSharedPtr<FMetaChannel> mHoveredKeys;
};
