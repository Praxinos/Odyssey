// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

#include "OdysseyAnimationSettingsCustomization.generated.h"

//---

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyTypeCustomizationUtils;
struct FOdysseyAnimationSettings;

//---

UENUM()
enum class EOdysseyAnimationDefaultLayerType: uint8
{
    Raster,
    Vector
};

//---

struct FOdysseyAnimationSettingsCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FOdysseyAnimationSettings* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

    int32 GetDefaultLayerType() const;
    void OnDefaultLayerTypeChanged( int32 Value, ESelectInfo::Type SelectInfo );

private:
    FOdysseyAnimationSettings* mSettings;

    EOdysseyAnimationDefaultLayerType mSelectedDefaultLayerType;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mDefaultLayerClassHandle;
};
