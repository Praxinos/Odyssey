// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class SOdysseyLayerStackTreeView;

/**
 * Implements a layer row widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerRow
    : public SMultiColumnTableRow<UOdysseyLayer*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyLayerRow)
        {}
    SLATE_END_ARGS()
    
public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer);

public:
    //Getters
    UOdysseyLayer* GetLayer();

	//Commands
	void Rename();

protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
	virtual const FSlateBrush* GetBorder() const override;

protected:
	virtual TSharedRef<SWidget> GenerateHeaderWidget();
    virtual TSharedRef<SWidget> GenerateOptionsWidget();
    TSharedRef<SWidget> GenerateExpandableHeaderWidget();
	TSharedRef<SWidget> GenerateIsOptionsDisplayedWidget();
    TSharedRef<SWidget> GenerateIsActivatedWidget();
    TSharedRef<SWidget> GenerateIsLockedWidget();
    
    void OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsActivatedCheckBoxState() const;
    void OnIsLockedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsLockedCheckBoxState() const;

	void OnLayerNameCommited(const FText& iText, ETextCommit::Type iType);
    FText GetLayerName() const;
    FSlateFontInfo GetLayerNameFont() const;



    /**
     * @brief Called when an Item is dragged over the row
     * Returns the dropzone or nothing
     * 
     * @param iEvent 
     * @param iDropZone 
     * @param iLayer 
     * @return TOptional<EItemDropZone> 
     */
    TOptional<EItemDropZone> OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer);

    /**
     * @brief Called when an Item is drop over the row
     * 
     * @param iEvent 
     * @param iDropZone 
     * @param iLayer 
     * @return TOptional<EItemDropZone> 
     */
    FReply OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer);

    /**
     * @brief Called when this row begins to be draged
     * 
     * @param iGeometry 
     * @param iEvent 
     * @return FReply 
     */
    FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView);


private:
    void OnIsOptionsDisplayedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsOptionsDisplayedCheckBoxState() const;
    EItemDropZone ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded);

private:
    UOdysseyLayer* mLayer = nullptr;
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
    bool mIsOptionsDisplayed = false;
};
