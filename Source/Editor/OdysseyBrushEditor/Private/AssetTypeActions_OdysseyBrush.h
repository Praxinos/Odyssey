// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"
#include "OdysseyBrushBlueprint.h"
#include "Toolkits/IToolkitHost.h"

class FMenuBuilder;

class FAssetTypeActions_OdysseyBrush : public FAssetTypeActions_Base
{
public:
    FAssetTypeActions_OdysseyBrush( EAssetTypeCategories::Type InAssetCategory );

    // IAssetTypeActions interface
    virtual  FText      GetName()  const  override;
    virtual  FColor     GetTypeColor()  const  override;
    virtual  UClass*    GetSupportedClass()  const  override;
    virtual  bool       HasActions( const TArray< UObject* >&  InObjects )  const  override;
    virtual  void       GetActions( const TArray< UObject* >&  InObjects, FMenuBuilder& MenuBuilder)  override;
    virtual  void       OpenAssetEditor( const TArray< UObject*>&  InObjects, TSharedPtr< class  IToolkitHost> EditWithinLevelEditor = TSharedPtr< IToolkitHost >() )  override;
    virtual  uint32     GetCategories()  override;
    virtual  bool       CanLocalize()  const  override { return  false; }
    // End of IAssetTypeActions interface

private:
    EAssetTypeCategories::Type OdysseyBrushAssetCategory;

protected:
    typedef  TArray< TWeakObjectPtr< class  UOdysseyBrush > > FWeakBlueprintPointerArray;

    void ExecuteEdit( FWeakBlueprintPointerArray Objects );
};
