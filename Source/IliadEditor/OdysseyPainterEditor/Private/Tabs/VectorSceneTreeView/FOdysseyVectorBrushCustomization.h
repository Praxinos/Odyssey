// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

struct FOdysseyVectorBrush;
class SButton;

class FOdysseyVectorBrushCustomization : public IPropertyTypeCustomization
{
public:
    static void Register();
    static void Unregister();

    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new FOdysseyVectorBrushCustomization());
    }

    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> StructPropertyHandle
                                , class FDetailWidgetRow& HeaderRow
                                , IPropertyTypeCustomizationUtils& StructCustomizationUtils ) override;

    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> StructPropertyHandle
                                  , class IDetailChildrenBuilder& StructBuilder
                                  , IPropertyTypeCustomizationUtils& StructCustomizationUtils ) override;

    private:
        void OnAssetSelected( const FAssetData& AssetData
                            , TSharedRef<IPropertyHandle> StructPropertyHandle );
        void OnChildPropertyValueChanged( TSharedRef<IPropertyHandle> StructPropertyHandle );
        FReply OnClicked( TSharedRef<IPropertyHandle> StructPropertyHandle );
        bool FilterAsset( const struct FAssetData& InAssetData );
        const FSlateBrush* UpdateButtonImage( TSharedRef<IPropertyHandle> StructPropertyHandle ) const;
        FText UpdateButtonToolTip( TSharedRef<IPropertyHandle> StructPropertyHandle ) const;
        FOdysseyVectorBrush* GetVectorBrush( TSharedRef<IPropertyHandle> StructPropertyHandle ) const;

    private:
        TSharedPtr<SButton> mBrushButton;
        TSharedPtr<FSlateBrush> mBrushIcon;
};
