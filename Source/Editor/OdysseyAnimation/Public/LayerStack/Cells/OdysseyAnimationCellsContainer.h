// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCellsContainer
{
public:
    /**
     * @brief Delegate called when adding / removing cells
     * 
     */
    DECLARE_MULTICAST_DELEGATE(FOnCellsChanged)
    FOnCellsChanged& OnCellsChanged();

    DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<FOdysseyAnimationCell>, FCreateCell, const FName& /*iType*/, bool /*iForSzerialization*/)

public:
    ~FOdysseyAnimationCellsContainer();
    FOdysseyAnimationCellsContainer();

public:
    //Public API - Cells
    /**
     * @brief The create cell callback
     * Needs to be bound to make the container work properly
     * 
     * @param iCreateCell 
     */
    FCreateCell& CreateCellDelegate();

    /**
     * @brief Get the Cells Offset
     * 
     * @return int 
     */
    int GetOffset() const;

    /**
     * @brief Get the Range of frames covered by cells
     */
    FInt32Range GetFrameRange() const;

    /**
     * @brief Get the Cells
     * 
     * @return TArray<TSharedPtr<FOdysseyAnimationCell>>& 
     */
    const TArray<TSharedPtr<FOdysseyAnimationCell>>& GetCells() const;

    /**
     * @brief Get the Cell At Frame
     * 
     * @param iFrameIndex 
     * @param oCelFrameIndex 
     * @return TSharedPtr<FOdysseyAnimationCell> 
     */
    TSharedPtr<FOdysseyAnimationCell> GetCellAtFrame(int iFrameIndex) const;

    /**
     * @brief Get the Index of the Cell at the given frame
     * 
     * @param iFrameIndex
     */
    int GetCellIndexAtFrame(int iFrameIndex) const;

    /**
     * @brief Get the FrameIndex of the Frame at the FrameIndex in the container
     * 
     * @param iFrameIndex
     */
    int GetCellFrameAtFrame(int iFrameIndex) const;

    /**
     * @brief Get the Cell Frame Range
     * 
     * @param iIndex
     */
    FInt32Range GetCellFrameRange(int iIndex) const;

    /**
     * @brief Returns wether there is a cell covering the given frame
     * 
     * @param iFrame 
     * @return true 
     * @return false 
     */
    bool HasCellAtFrame(int iFrame) const;

    /**
     * @brief Returns wether the given frame is covered by the first frame of a cell
     * 
     * @param iFrame 
     * @return true 
     * @return false 
     */
    bool IsCellHeadAtFrame(int iFrame) const;

    /**
     * @brief Serialize
     * 
     * @param Ar 
     */
    void Serialize(FArchive& Ar);

private:
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
    int mOffset;
    FOnCellsChanged mOnCellsChanged;
    FCreateCell mCreateCell;

private:
    //Import/Export
    friend class FOdysseyAnimationCellsContainerExport;
    friend class FOdysseyAnimationCellsContainerImport;

private:
    friend class FOdysseyAnimationCellsMutator;
    friend class FOdysseyAddCellsMutation;
    friend class FOdysseyRemoveCellsMutation;
    friend class FOdysseySetCellLengthMutation;
    friend class FOdysseySetCellsOffsetMutation;
};
