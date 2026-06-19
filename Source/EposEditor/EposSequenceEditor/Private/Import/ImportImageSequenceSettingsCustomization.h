// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

//---

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyTypeCustomizationUtils;
struct FImportImageSequenceOptions;

//---

struct FImportImageSequenceOptionsCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FImportImageSequenceOptions* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

private:
    FImportImageSequenceOptions*    mOptions;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle>     mPatternHandle;
};
