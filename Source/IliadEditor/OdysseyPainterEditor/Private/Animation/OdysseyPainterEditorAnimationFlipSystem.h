// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "InputCoreTypes.h"
#include "OdysseyRenderingType.h"
#include "OdysseyPainterEditorAnimationFlipSystem.generated.h"

UENUM()
enum class EOdysseyAnimationFlipLimits : uint8
{
    None,
    Animation,
    Layer,
    CellMarks,
    Custom
};

UENUM()
enum class EOdysseyAnimationFlipKeys : uint8
{
    AllFrames,
    Cells,
    CellMarks
};

UENUM()
enum class EOdysseyAnimationFlipRollback : uint8
{
    None,
    Instantaneous,
};

UENUM()
enum class EOdysseyAnimationFlipDirection : uint8
{
    Horizontal,
    HorizontalInverted UMETA(DisplayName = "Horizontal (Inverted)"),
    Vertical,
    VerticalInverted UMETA(DisplayName = "Vertical (Inverted)")
};

USTRUCT()
struct FOdysseyAnimationFlipConfiguration
{
    GENERATED_BODY()

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipLimits Limits = EOdysseyAnimationFlipLimits::None;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Limits == EOdysseyAnimationFlipLimits::CellMarks"
                , EditConditionHides) )
    int LimitsCellMark = -2;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = ( EditCondition = "Limits == EOdysseyAnimationFlipLimits::Custom"
                , EditConditionHides
                , UIMin    = "0"
                , ClampMin    = "0"
                , LinearDeltaSensitivity = "15"
                , Delta = "1"
                ) )
    int LeftLimit = 0;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Limits == EOdysseyAnimationFlipLimits::Custom"
                , EditConditionHides
                , UIMin    = "0"
                , ClampMin    = "0"
                , LinearDeltaSensitivity = "15"
                , Delta = "1"
                ) )
    int RightLimit = 0;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipKeys Keys = EOdysseyAnimationFlipKeys::Cells;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Keys == EOdysseyAnimationFlipKeys::CellMarks"
                , EditConditionHides) )
    int KeysCellMark = -2;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool Loop = false;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool Rollback = true;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool OutOfPegs = true;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipDirection Direction = EOdysseyAnimationFlipDirection::Horizontal;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent") )
    float Sensibility = 40.f;
};

class UOdysseyAnimation;
class FOdysseyPainterEditor;

class FOdysseyPainterEditorAnimationFlipSystem
    : public IInputProcessor
{
public:
    static void RegisterDetailCustomization();
    static void UnregisterDetailCustomization();

public:
    virtual ~FOdysseyPainterEditorAnimationFlipSystem();
    FOdysseyPainterEditorAnimationFlipSystem(FOdysseyPainterEditor* iEditor);

    void StartFlipping(const FOdysseyAnimationFlipConfiguration& iFlipConfiguration);
    bool IsFlipping() const;
    uint64 GetRenderType() const;

private:
    void EndFlipping();
    void FlipTo(int iDelta);

    void GetKeyFrame(int iDelta, int& oFrame);
    void GetLimits(EOdysseyAnimationFlipLimits iLimits, int& oLeftLimit, int& oRightLimit); //Returns INDEX_NONE if the Limit does not apply

private:
    //needed to compile against IInputProcessor
    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

    /** Key down input */
    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    /** Key up input */
    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    /** Mouse movement input */
    virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    /** Analog axis input */
    virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

    /** Mouse button press */
    virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    /** Mouse button release */
    virtual bool HandleMouseButtonUpEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    /** Mouse button double clicked. */
    virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    /** Mouse wheel input */
    virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override;

    /** Called when a motion-driven device has new input */
    virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override;

private:
    FOdysseyPainterEditor* mEditor;

    UOdysseyAnimation* mAnimation = nullptr;

    FKey mLastKey;
    FVector2D mMousePositionReference;

    bool mIsFlipping = false;
    int mStartFrame = 0;
    FOdysseyAnimationFlipConfiguration mFlipConfiguration;
    uint64 mInitialRenderType = EOdysseyRenderingType::Render;
};
