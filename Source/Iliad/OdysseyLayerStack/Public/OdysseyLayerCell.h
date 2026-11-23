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

    /** Out-of-Pegs pan of the Cell display on the X and Y axes. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FVector2D Pan = FVector2D(0, 0);

    /** Out-of-Pegs rotation of the Cell display. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Rotation = 0.f;

    UPROPERTY(meta=(DeprecatedProperty, DeprecationMessage = "Use the new Scale property instead." ))
    float Zoom_DEPRECATED = 100.f;

    /** Out-of-Pegs scale (Zoom) of the Cell display, in percent. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FVector2D Scale = FVector2D( 100.f, 100.f );
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
    /** Return the layer that contains this Cell. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayer* GetLayer() const;

    /** Return the layer stack that contains this Cell. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayerStack* GetLayerStack() const;

    /** Return the range of frames covered by this Cell in its layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual FInt32Range GetFrameRange() const override;

    /** Return the index of this Cell in its layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetIndexInLayer() const;

    /**
     * Return the exposure of this Cell.
     *
     * The exposure is the duration of the Cell, in frames.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetExposure() const;

    /**
     * Set the exposure of this Cell.
     *
     * The exposure is the duration of the Cell, in frames. The next
     * Cells are moved accordingly.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetExposure(int Value);

    /** Return the texture holding the rendered content of this Cell. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UTexture2D* GetRenderTexture() const;

#if WITH_EDITOR
    /**
     * Return the name of this Cell.
     *
     * If the Cell has no name, CellNameIfEmpty selects what to return:
     * - an empty string
     * - the index of the Cell in its layer
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    FString GetName( ECellNameIfEmpty iCellNameIfEmpty ) const;

    /** Set the name of this Cell. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetName(FString Value);

    /** Return true if no name has been given to this Cell. */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Cell" )
    bool HasNoName() const;

    UE_DEPRECATED( 5.7, "Use GetMarks() to return all marks" )
    /**
     * Return the Mark set on the first frame of this Cell.
     *
     * Return -1 if there is none. Deprecated: use GetMarks instead.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetMark() const;

    UE_DEPRECATED( 5.7, "Use SetMarks() to set all marks" )
    /**
     * Set the Mark on the first frame of this Cell.
     *
     * Use -1 to remove it. Deprecated: use SetMarks instead.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetMark(int Value);

    /**
     * Return all the Marks of this Cell.
     *
     * Marks are tags on frames to tell them apart (Keys, Breakdowns,
     * Retakes, etc). The key is the frame index inside the Cell.
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Cell" )
    TMap<int, FCellMark> GetMarks() const;

    /**
     * Set all the Marks of this Cell.
     *
     * Marks are tags on frames to tell them apart (Keys, Breakdowns,
     * Retakes, etc). The key is the frame index inside the Cell.
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Cell" )
    void SetMarks( const TMap<int, FCellMark>& iMarks );

    /**
     * Cut this Cell into two Cells at the given frame.
     *
     * Like the Cutting tool: if Clear is true the new Cell is empty,
     * otherwise it keeps the content. Return the new Cell.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear);

    /** Return true if an Out-of-Pegs transform is applied to this Cell. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

    /** Return the Out-of-Pegs transform (pan, rotation, scale). */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    FOdysseyLayerCellOutOfPegs GetOutOfPegs() const;

    /**
     * Set the Out-of-Pegs transform (pan, rotation, scale) of this Cell.
     *
     * Out-of-Pegs moves the display of the Cell out of the peg bar,
     * also known as "Shift and Trace".
     */
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
    virtual bool IsSelectedInEditor() const override;
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

#if WITH_EDITOR
namespace OdysseyLayerStackSelection {
    /** Callback for editor layer selection. This must be in runtime instead of editor for UOdysseyLayer::IsSelectedInEditor to work */
    extern ODYSSEYLAYERSTACK_API TFunction<bool(const UObject*)> GIsCellSelectedInEditor;
}
#endif
