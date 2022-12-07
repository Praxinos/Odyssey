// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FOdysseyRasterDrawingToolBrushSelectorCustomization : public IDetailCustomization
{
public:
    ~FOdysseyRasterDrawingToolBrushSelectorCustomization();
    FOdysseyRasterDrawingToolBrushSelectorCustomization();

public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FOdysseyRasterDrawingToolBrushSelectorCustomization>();
    }

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface

private:
    UOdysseyRasterDrawingTool* GetRasterDrawingTool();
    void HideAllProperties();
    void AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName);
    void AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject);
    void AddBrushInstance(IDetailCategoryBuilder& iCategory);
    void AddBrushSelector(IDetailCategoryBuilder& iCategory);

private:
    UOdysseyBrush* GetBrush() const;

    void OnBrushChanged(UOdysseyBrush* iBrush);
    void OnObjectPostEditChange( UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent );

private:
    class UOdysseyRasterDrawingTool* mTool;
	IDetailLayoutBuilder* mBuilder;
    FDelegateHandle       mPropertyChangedHandle;
};
