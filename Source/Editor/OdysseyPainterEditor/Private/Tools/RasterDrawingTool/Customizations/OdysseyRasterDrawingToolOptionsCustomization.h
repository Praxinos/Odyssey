// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FOdysseyRasterDrawingToolOptionsCustomization : public IDetailCustomization
{
public:
    ~FOdysseyRasterDrawingToolOptionsCustomization();
    FOdysseyRasterDrawingToolOptionsCustomization();

public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FOdysseyRasterDrawingToolOptionsCustomization>();
    }

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface

private:
    UOdysseyRasterDrawingTool* GetRasterDrawingTool();
    void HideAllProperties();
    void AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName);
    void AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject);
    void AddSelectedShapeInstance(IDetailCategoryBuilder& iCategory);
    void AddBlendParameters();

private:
    void OnObjectPostEditChange(UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent);

private:
    class UOdysseyRasterDrawingTool* mTool;
    IDetailLayoutBuilder* mBuilder;
    FDelegateHandle       mPropertyChangedHandle;
};
