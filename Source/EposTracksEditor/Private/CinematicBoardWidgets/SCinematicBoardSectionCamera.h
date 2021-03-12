// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionCamera
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionCamera )
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
    //virtual TOptional<TSharedRef<SWidget>> OnMapCursor( const FCursorReply& CursorReply ) const;

public:

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    typedef TMovieSceneChannelHandle<FMovieSceneFloatChannel> FFloatChannelHandle;

    template <typename ValueType>
    struct THandleMapKeyFuncs : BaseKeyFuncs< TPair<FFloatChannelHandle, ValueType>, const FFloatChannelHandle* > //https://docs.unrealengine.com/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/TMap/index.html#keyfuncs
    {
    private:
        typedef BaseKeyFuncs< TPair<FFloatChannelHandle, ValueType>, const FFloatChannelHandle* > Super;

    public:
        typedef typename Super::ElementInitType ElementInitType;
        typedef typename Super::KeyInitType     KeyInitType;

        static KeyInitType  GetSetKey( ElementInitType Element )    { return &Element.Key; }
        static bool         Matches( KeyInitType A, KeyInitType B ) { return A == B; }
        static uint32       GetKeyHash( KeyInitType Key )           { return GetTypeHash( Key ); }
    };

    typedef TMap<FFloatChannelHandle, int32, FDefaultSetAllocator, THandleMapKeyFuncs<int>> FMapFloatChannelHandleToKeyIndex;

private:
    FMapFloatChannelHandleToKeyIndex GetKeysUnderCursor( const FPointerEvent& MouseEvent ) const;

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMapFloatChannelHandleToKeyIndex mKeyIndexForChannel;
};
