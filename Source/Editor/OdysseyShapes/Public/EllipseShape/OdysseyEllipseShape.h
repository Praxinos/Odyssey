// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyEllipseShape.generated.h"

class FOdysseyHUDEllipse;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Ellipse Shape"))
class ODYSSEYSHAPES_API UOdysseyEllipseShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathBegin, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTo, const TArray<FOdysseyPoint>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathEnd, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE(FOnReset);

    DECLARE_DELEGATE_RetVal_OneParam(float, FAdaptStep, float);

public:
    // Destructor
    virtual ~UOdysseyEllipseShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

public:
    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnReset& OnResetDelegate() { return mOnResetDelegate; }

    FAdaptStep& AdaptStepDelegate() { return mAdaptStepDelegate; }

    virtual void Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions) override; //Draw this shape onto a block

private:
    void CommitEllipse();
    virtual bool AbortShape() override;

protected:
    // protected Data Members

    //---

    //Internal
    TArray< FOdysseyPoint >             mRawStroke; //Raw Stroke (basically mouse positions)

    bool                                mHasStrokeBegun;

    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnReset                            mOnResetDelegate;

    FAdaptStep                          mAdaptStepDelegate;

private:
    TSharedPtr<FOdysseyHUDEllipse> mEllipse;
    TArray<TSharedPtr<FOdysseyHUDHandle>> mHandles;
};
