// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"


/**
 * Implements actions for UBoardSequence assets.
 */
class FBoardSequenceActions
    : public FAssetTypeActions_Base
{
public:

    // IAssetTypeActions interface
    virtual uint32 GetCategories() override;
    virtual FText GetName() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual FColor GetTypeColor() const override;
    virtual void OpenAssetEditor( const TArray<UObject*>& iObjects, TSharedPtr<class IToolkitHost> iEditWithinLevelEditor = TSharedPtr<IToolkitHost>() ) override;
    virtual bool ShouldForceWorldCentric() override;
    virtual bool CanLocalize() const override;
    //virtual bool HasActions( const TArray<UObject*>& iObjects ) const override;
    //virtual void GetActions( const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder ) override;
};
