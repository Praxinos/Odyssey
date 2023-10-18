// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

class FOdysseyVectorBrushCustomization : public IPropertyTypeCustomization
{
public:
    static void Register();

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
        void OnPropertyValueChanged( TSharedRef<IPropertyHandle> StructPropertyHandle );
        FReply OnClicked( TSharedRef<IPropertyHandle> StructPropertyHandle );
        bool FilterAsset( const struct FAssetData& InAssetData );
        const FSlateBrush* UpdateButton( TSharedRef<IPropertyHandle> StructPropertyHandle );

    private:
        TSharedPtr<SButton> mBrushButton;
        TSharedPtr<FSlateBrush> mBrushIcon;
        TSharedPtr<FAssetThumbnailPool> mAssetThumbnailPool;
};
