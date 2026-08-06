// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTextureRenderingAbility.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyBlendingMode.h"

#include "OdysseyLayerCell.generated.h"

class UOdysseyLayer;
class UOdysseyLayerStack;
class UTexture2D;

USTRUCT(BlueprintType)
struct FOdysseyLayerCellOutOfPegs
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FVector2D Pan = FVector2D(0, 0);

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Rotation = 0.f;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Zoom = 100.f;
};

UENUM(BlueprintType)
enum class ECellNameIfEmpty: uint8
{
    None,
    IndexInLayer,
};

USTRUCT(BlueprintType)
struct FCellMark
{
    GENERATED_BODY()

    /**
     * The value of the cell mark
     */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    int32 Index = 0;
};


UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYLAYERSTACK_API UOdysseyLayerCell
    : public UObject
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

#if WITH_EDITOR
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)
#endif

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate( EDuplicateMode::Type iDuplicateMode ) override;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayer* GetLayer() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayerStack* GetLayerStack() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual FInt32Range GetFrameRange() const override;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetIndexInLayer() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetExposure() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetExposure(int Value);

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UTexture2D* GetRenderTexture() const;

#if WITH_EDITOR
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    FString GetName( ECellNameIfEmpty iCellNameIfEmpty ) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetName(FString Value);

    UFUNCTION( BlueprintPure, Category = "Odyssey|Cell" )
    bool HasNoName() const;

    UE_DEPRECATED( 5.7, "Use GetMarks() to return all marks" )
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetMark() const;

    UE_DEPRECATED( 5.7, "Use SetMarks() to set all marks" )
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetMark(int Value);

    UFUNCTION( BlueprintPure, Category = "Odyssey|Cell" )
    TMap<int, FCellMark> GetMarks() const;

    UFUNCTION( BlueprintCallable, Category = "Odyssey|Cell" )
    void SetMarks( const TMap<int, FCellMark>& iMarks );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear);

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    FOdysseyLayerCellOutOfPegs GetOutOfPegs() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetOutOfPegs(FOdysseyLayerCellOutOfPegs Value);

    //Clears all the data from the cell
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual void Clear() {};
#endif

public:
    int32 FrameInLayerToIndexInCell( FFrameNumber iFrame );
    FFrameNumber IndexInCellToFrameInLayer( int32 iIndex );

public:
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;
#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) override;
#endif
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const override;

#if WITH_EDITOR
    virtual void InitTexture();
    virtual FOdysseyMediaProvider GetMediaProvider() const;
#endif

public:
#if WITH_EDITOR
    // UObject overrides
    virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif

public:
#if WITH_EDITOR
    FMatrix OutOfPegsTransform() const;
    FOnOutOfPegsChanged& OnOutOfPegsChanged();
    void SetOutOfPegsInteractive(FOdysseyLayerCellOutOfPegs Value);
    void SetExposureInteractive(int Value);
#endif

private:
    void ExposureChanged(bool iIsInteractive);
#if WITH_EDITOR
    void OutOfPegsChanged(bool iIsInteractive);
#endif

private:
    void UpdateTextureSize();

public: //Public needed for the shortcuts
    UPROPERTY()//TODO: meta (minvalue 1)
    int Exposure = 1;

protected:
    friend class UOdysseyLayer;
    friend class FOdysseyLayerCellImport;

    UPROPERTY()
    int IndexInLayer = -1;

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    FString Name;

    UPROPERTY()
    int Mark_DEPRECATED = -1;

    // The key is the index of the mark in the cell
    // (index 0 means the first frame of the cell, wherever the cell is in the layer)
    UPROPERTY()
    TMap<int, FCellMark> Marks;

    UPROPERTY(NonTransactional, DuplicateTransient)
    FOdysseyLayerCellOutOfPegs OutOfPegs;
#endif

private:
    UPROPERTY(NonTransactional)
    mutable TObjectPtr<UTexture2D> Texture; //mutable is temporary, will be removed when layers will be 100% GPU based and there's no more dependency on ULIS

#if WITH_EDITOR
private:
    FOnOutOfPegsChanged mOnOutOfPegsChanged;
#endif
};
