// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

//---

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyTypeCustomizationUtils;
struct FExportImageSequenceOptions;

//---

struct FExportImageSequenceOptionsCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FExportImageSequenceOptions* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

    void OnImageSizeChanged();

private:
    FExportImageSequenceOptions* mOptions;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
    TSharedPtr<IPropertyHandle> mImageSizeHandle;
    TSharedPtr<IPropertyHandle> mAspectRatioHandle;

    FIntPoint mCacheImageSize;
};
