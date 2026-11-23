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

    /**
     * Range within which the Flip can move.
     *
     * None: no limit, unless limited by the Keys type.
     * Animation: the overall duration of the Animation 2D.
     * Layer: the duration of the current Layer.
     * Cell Marks: between 2 Cell Marks.
     * Custom: a number of frames before and after the scrubbing cursor.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipLimits Limits = EOdysseyAnimationFlipLimits::None;

    /**
     * Cell Mark used as Flip limits.
     *
     * Choose between all Cell Marks or a specific Cell Mark.
     * Only used when Limits is set to Cell Marks.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Limits == EOdysseyAnimationFlipLimits::CellMarks"
                , EditConditionHides) )
    int LimitsCellMark = -2;

    /**
     * Number of frames before the scrubbing cursor the Flip can reach.
     *
     * Only used when Limits is set to Custom.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = ( EditCondition = "Limits == EOdysseyAnimationFlipLimits::Custom"
                , EditConditionHides
                , UIMin    = "0"
                , ClampMin    = "0"
                , LinearDeltaSensitivity = "15"
                , Delta = "1"
                ) )
    int LeftLimit = 0;

    /**
     * Number of frames after the scrubbing cursor the Flip can reach.
     *
     * Only used when Limits is set to Custom.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Limits == EOdysseyAnimationFlipLimits::Custom"
                , EditConditionHides
                , UIMin    = "0"
                , ClampMin    = "0"
                , LinearDeltaSensitivity = "15"
                , Delta = "1"
                ) )
    int RightLimit = 0;

    /**
     * Type of keys the Flip steps through.
     *
     * All Frames: the Flip occurs on all the Frames.
     * Cells: the Flip moves from Cell to Cell, ignoring their exposure.
     * Cell Marks: the Flip moves between frames carrying the chosen Cell Mark.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipKeys Keys = EOdysseyAnimationFlipKeys::Cells;

    /**
     * Cell Mark the Flip steps through.
     *
     * Choose between all Cell Marks or a specific Cell Mark.
     * Only used when Keys is set to Cell Marks.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration"
                , meta = (EditCondition = "Keys == EOdysseyAnimationFlipKeys::CellMarks"
                , EditConditionHides) )
    int KeysCellMark = -2;

    /**
     * Loop the Flip when reaching the end of the Limits.
     *
     * When off, the Flip stops at the end of the chosen Limits.
     * When on, it starts all over again within the same gesture.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool Loop = false;

    /**
     * Return the scrubbing cursor to its original position.
     *
     * When on, the cursor rolls back to its original position once the
     * keyboard shortcut is no longer held. When off, it stays on its
     * current position in the Timeline.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool Rollback = true;

    /**
     * Take the Out-of-Pegs into consideration during the Flip.
     *
     * When off, Cells that got out-of-pegged are displayed in their
     * original position while flipping.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    bool OutOfPegs = true;

    /**
     * Direction of the gesture that moves the scrubbing cursor.
     *
     * Horizontal: right steps forward, left steps backward.
     * Vertical: up steps backward, down steps forward.
     * Inverted variants do the opposite.
     */
    UPROPERTY(  EditAnywhere, Category="Flip Configuration" )
    EOdysseyAnimationFlipDirection Direction = EOdysseyAnimationFlipDirection::Horizontal;

    /**
     * Sensitivity of the gesture.
     *
     * The higher the percentage, the more sensitive (and faster)
     * the Flip.
     */
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
