// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include "ULISLoaderModule.h"

#include "OdysseyTool.generated.h"

UCLASS(Abstract)
class ODYSSEYTOOLS_API UOdysseyTool : public UObject
{
    GENERATED_BODY()

public:
    virtual void Init() {};

public:
    //Drawing the HUD associated to the tool
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) {};

public:
    //Mouse events
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY);
    virtual FReply InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply);
    virtual void CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY);
    virtual ::ULIS::TArray<::ULIS::FVec2I> GenerateToolPoints() { return ::ULIS::TArray<::ULIS::FVec2I>(); };

public:
    //Getters
    bool IsReadyToBeApplied() const;

protected:
    bool mIsReadyToBeApplied;
};
