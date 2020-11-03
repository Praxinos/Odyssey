// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "AssetTypeActions_OdysseyBrush.h"
#include "ContentBrowserModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyBrushEditorModule.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "OdysseyBrushAssetBase.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_OdysseyBrush"


/////////////////////////////////////////////////////
// FAssetTypeActions_OdysseyBrush


FAssetTypeActions_OdysseyBrush::FAssetTypeActions_OdysseyBrush( EAssetTypeCategories::Type InAssetCategory )
    : OdysseyBrushAssetCategory( InAssetCategory )
{
}


FText
FAssetTypeActions_OdysseyBrush::GetName()  const
{
    return  LOCTEXT( "AssetTypeActions_OdysseyBrush", "Odyssey Brush" );
}


FColor
FAssetTypeActions_OdysseyBrush::GetTypeColor()  const
{
    return
    FColor(100, 255, 0);
}


UClass*
FAssetTypeActions_OdysseyBrush::GetSupportedClass()  const
{
    return  UOdysseyBrush::StaticClass();
}


bool
FAssetTypeActions_OdysseyBrush::HasActions( const  TArray< UObject* >&  InObjects)  const
{
    return  true;
}


void
FAssetTypeActions_OdysseyBrush::GetActions( const  TArray< UObject* >& InObjects, FMenuBuilder& MenuBuilder )
{
    /*
    auto Blueprints = GetTypedWeakObjectPtrs< UOdysseyBrush >( InObjects );

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "OdysseyBrush_Edit", "Edit" ),
        LOCTEXT( "OdysseyBrush_EditTooltip", "Opens the selected brush in the graph editor." ),
        FSlateIcon( "OdysseyStyle", "OdysseyBrush.EditBrush16" ),
        FUIAction(
            FExecuteAction::CreateSP( this, &FAssetTypeActions_OdysseyBrush::ExecuteEdit, Blueprints ),
            FCanExecuteAction()
            )
        );*/
}


void
FAssetTypeActions_OdysseyBrush::OpenAssetEditor( const  TArray< UObject* >&  InObjects, TSharedPtr< class  IToolkitHost > EditWithinLevelEditor )
{
    for( auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt )
    {
        if( UOdysseyBrush* Brush = Cast< UOdysseyBrush >( *ObjIt ) )
        {
            FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>( "Kismet" );
            TSharedRef< IBlueprintEditor > NewBlueprintEditor = BlueprintEditorModule.CreateBlueprintEditor(  EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), Brush );
        }
    }
}


uint32
FAssetTypeActions_OdysseyBrush::GetCategories()
{
    return  OdysseyBrushAssetCategory;
}


void
FAssetTypeActions_OdysseyBrush::ExecuteEdit( FWeakBlueprintPointerArray Objects )
{
    for( auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt )
    {
        if( auto Object = ( *ObjIt ).Get() )
        {
            FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>( "Kismet" );
            TSharedRef< IBlueprintEditor > NewBlueprintEditor = BlueprintEditorModule.CreateBlueprintEditor(  EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), Object );
        }
    }
}

#undef LOCTEXT_NAMESPACE
