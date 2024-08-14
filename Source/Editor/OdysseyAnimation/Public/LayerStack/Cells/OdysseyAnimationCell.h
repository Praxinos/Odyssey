// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderingAbility.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyAnimationCell.generated.h"

class UOdysseyAnimationLayer;

USTRUCT(BlueprintType)
struct FOdysseyAnimationCellOutOfPegs
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
	FVector2D Pan = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Rotation = 0.f;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Zoom = 1.f;
};

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCell
	: public UObject
    , public FOdysseyImageRenderingAbility
{
	GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)

public:
	UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
	UOdysseyAnimationLayer* GetLayer() const;

	UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
	UOdysseyAnimation* GetAnimation() const;

	UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
	UOdysseyAnimationLayerStack* GetLayerStack() const;

	UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
	bool IsOutOfPegs() const;

	UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
	FInt32Range GetFrameRange() const;

	UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
	virtual UOdysseyAnimationCell* Break(int Frame);
    
public:
	virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;

public:
    //OutOfPegs
    ::ULIS::FMat3F OutOfPegsTransform() const;
	FOnOutOfPegsChanged& OnOutOfPegsChanged();

public:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
	virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey

protected:
	//Properties modifications
	void OutOfPegsChanged(bool iIsInteractive);
	void LengthChanged(bool iIsInteractive);

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);

public:
	UPROPERTY(BlueprintReadOnly, Category="Odyssey|Cell")
    int IndexInLayer = -1;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")//TODO: meta (minvalue 1)
    int Length = 1;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell") //TODO: GetOptions ? Is that possible ?
    int Mark = -1;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FOdysseyAnimationCellOutOfPegs OutOfPegs;

private:
    FOnOutOfPegsChanged mOnOutOfPegsChanged;
};
