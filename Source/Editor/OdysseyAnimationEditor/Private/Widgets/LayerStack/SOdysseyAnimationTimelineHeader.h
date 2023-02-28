// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimation;
class UMediaPlayer;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineHeader : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnOffsetChanged, float)
	DECLARE_DELEGATE_OneParam(FOnZoomChanged, float)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineHeader)
		: _Animation(nullptr)
		, _MediaPlayer(nullptr)
		, _FrameWidth(50.f)
		, _Offset(0.0f)
		, _Zoom(1.0f)
	{}
		SLATE_ARGUMENT(UOdysseyAnimation*, Animation)
		SLATE_ARGUMENT(UMediaPlayer*, MediaPlayer)
		SLATE_ATTRIBUTE(float, FrameWidth)
		SLATE_ATTRIBUTE(float, Zoom)
		SLATE_ATTRIBUTE(float, Offset)
		SLATE_EVENT(FOnOffsetChanged, OnOffsetChanged)
		SLATE_EVENT(FOnZoomChanged, OnZoomChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

private:
	//Events
	FOnZoomChanged 				mOnZoomChanged;
	FOnOffsetChanged 			mOnOffsetChanged;

	//Attributes
	UOdysseyAnimation*			mAnimation;
	UMediaPlayer*				mMediaPlayer;
	TAttribute<float> 			mFrameWidth;
	TAttribute<float> 			mZoom;
	TAttribute<float> 			mOffset;

	//State
	FVector2D 					mOffsetMousePosition;
	bool 						mIsOffsetting;
    bool 						mIsScrubbing;
};
