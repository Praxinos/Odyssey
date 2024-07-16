// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyAnimationEditorFlipSystem.generated.h"

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
    EOdysseyAnimationFlipKeys Keys = EOdysseyAnimationFlipKeys::AllFrames;

    UPROPERTY(  EditAnywhere, Category="Flip Configuration" 
                , meta = (EditCondition = "Limits == EOdysseyAnimationFlipKeys::CellMarks"
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

class FOdysseyAnimationEditorExtension;
class UOdysseyAnimation;

class FOdysseyAnimationEditorFlipSystem
    : public IInputProcessor
{
public:
    static void RegisterDetailCustomization();
    static void UnregisterDetailCustomization();

public:
    virtual ~FOdysseyAnimationEditorFlipSystem();
    FOdysseyAnimationEditorFlipSystem(FOdysseyAnimationEditorExtension* iExtension);

    void StartFlipping(const FOdysseyAnimationFlipConfiguration& iFlipConfiguration);
	
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

private:
    FOdysseyAnimationEditorExtension* mExtension;

    UOdysseyAnimation* mAnimation = nullptr;

    FKey mLastKey;
    FVector2D mMousePositionReference;

    bool mIsFlipping = false;
    int mStartFrame = 0;
    FOdysseyAnimationFlipConfiguration mFlipConfiguration;
    IOdysseyImageRenderer::eRenderType mInitialRenderType = IOdysseyImageRenderer::eRenderType::Render;
};
