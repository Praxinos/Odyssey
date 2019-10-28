// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyTextureDummy/OdysseyTextureDummy_ContentBrowserExtensions.h"

#include "Modules/ModuleManager.h"

#include "Misc/PackageName.h"

#include "Textures/SlateIcon.h"

#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "AssetData.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"

#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"

#include "Interfaces/IOdysseyPainterEditorModule.h"


#define LOCTEXT_NAMESPACE "OdysseyTextureDummy_ContentBrowserExtensions"


//////////////////////////////////////////////////////////////////////////


static FContentBrowserMenuExtender_SelectedAssets ContentBrowserExtenderDelegate;
static FDelegateHandle ContentBrowserExtenderDelegateHandle;


//////////////////////////////////////////////////////////////////////////
// FContentBrowserSelectedAssetExtensionBase
struct FContentBrowserSelectedAssetExtensionBase
{
public:
    TArray<struct FAssetData> SelectedAssets;

public:
    virtual void Execute() {}
    virtual ~FContentBrowserSelectedAssetExtensionBase() {}
};


//////////////////////////////////////////////////////////////////////////
// FEditTextureExtension
struct FEditTextureExtension : public FContentBrowserSelectedAssetExtensionBase
{

    FEditTextureExtension()
    {
    }

    void EditTextures(TArray<UTexture2D*>& Textures)
    {
        FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

        for (auto TextureIt = Textures.CreateConstIterator(); TextureIt; ++TextureIt)
        {
            UTexture2D* Texture = *TextureIt;
            IOdysseyPainterEditorModule* OdysseyPainterEditorModule = &FModuleManager::GetModuleChecked<IOdysseyPainterEditorModule>("OdysseyPainterEditor");
            OdysseyPainterEditorModule->CreateOdysseyPainterEditor(EToolkitMode::Standalone, NULL, Texture);
        }
    }

    virtual void Execute() override
    {
        TArray<UTexture2D*> Textures;
        for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
        {
            const FAssetData& AssetData = *AssetIt;
            if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
            {
                Textures.Add(Texture);
            }
        }

        EditTextures(Textures);
    }
};


//////////////////////////////////////////////////////////////////////////
// FOdysseyPainterContentBrowserExtensions_Impl
class FOdysseyPainterContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor(TSharedPtr<FContentBrowserSelectedAssetExtensionBase> SelectedAssetFunctor)
    {
        SelectedAssetFunctor->Execute();
    }

    static void PopulateTextureActionsMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
    {
        TSharedPtr<FEditTextureExtension> EditTextureFunctor = MakeShareable(new FEditTextureExtension());
        EditTextureFunctor->SelectedAssets = SelectedAssets;

        FUIAction Action_EditTexture(
            FExecuteAction::CreateStatic(&FOdysseyPainterContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>(EditTextureFunctor)));

        MenuBuilder.AddMenuEntry(
            LOCTEXT("CB_Extension_Texture_OpenPaintEditor", "Edit with ILIAD"),
            LOCTEXT("CB_Extension_Texture_OpenPaintEditor_Tooltip", "Open ILIAD paint editor for the selected Texture"),
            FSlateIcon("OdysseyStyle", "PainterEditor.OpenPaintEditor16"),
            Action_EditTexture,
            NAME_None,
            EUserInterfaceActionType::Button);

    }

    static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
    {
        TSharedRef<FExtender> Extender(new FExtender());

        bool bAnyTextures = false;
        for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
        {
            const FAssetData& Asset = *AssetIt;
            bAnyTextures = bAnyTextures || (Asset.AssetClass == UTexture2D::StaticClass()->GetFName());
        }

        if (bAnyTextures)
        {
            // Add the edit texture action to the menu
            Extender->AddMenuExtension(
                "GetAssetActions",
                EExtensionHook::After,
                nullptr,
                FMenuExtensionDelegate::CreateStatic(&FOdysseyPainterContentBrowserExtensions_Impl::PopulateTextureActionsMenu, SelectedAssets));
        }

        return Extender;
    }

    static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates()
    {
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
        return ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
    }
};


//////////////////////////////////////////////////////////////////////////
// FOdysseyPainterContentBrowserExtensions
void
FOdysseyPainterContentBrowserExtensions::InstallHooks()
{
    ContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FOdysseyPainterContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu);

    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FOdysseyPainterContentBrowserExtensions_Impl::GetExtenderDelegates();
    CBMenuExtenderDelegates.Add(ContentBrowserExtenderDelegate);
    ContentBrowserExtenderDelegateHandle = CBMenuExtenderDelegates.Last().GetHandle();
}


void
FOdysseyPainterContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FOdysseyPainterContentBrowserExtensions_Impl::GetExtenderDelegates();
    CBMenuExtenderDelegates.RemoveAll([](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == ContentBrowserExtenderDelegateHandle; });
}


//////////////////////////////////////////////////////////////////////////


#undef LOCTEXT_NAMESPACE

