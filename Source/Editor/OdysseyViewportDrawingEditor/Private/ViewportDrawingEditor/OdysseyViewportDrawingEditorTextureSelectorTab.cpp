// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureSelectorTab.h"

#include "Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorTextureSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorTextureSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorTextureSelectorTab::~FOdysseyViewportDrawingEditorTextureSelectorTab()
{
}

FOdysseyViewportDrawingEditorTextureSelectorTab::FOdysseyViewportDrawingEditorTextureSelectorTab(FOdysseyViewportDrawingEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_TextureSelector"),
                            LOCTEXT( "OdysseyViewportDrawingEditorTextureSelectorTab", "Texture Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.TextureSelector_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorTextureSelectorTab::CreateWidget()
{
	return SNew(SObjectPropertyEntryBox)
		.ObjectPath(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::PaintTexturePath)
		.AllowedClass(UTexture2D::StaticClass())
		.OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterTextureAsset))
		.OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnObjectChanged))
		.DisplayUseSelected(false);
        //.ThumbnailPool(iCustomizationUtils.GetThumbnailPool())
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FString
FOdysseyViewportDrawingEditorTextureSelectorTab::PaintTexturePath() const
{
    if(!mEditor->Texture())
        return FString();

    return mEditor->Texture()->GetPathName();
}

bool
FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterTextureAsset(const FAssetData& iAssetData) const
{	
	return !(mEditor->SelectableTextures().ContainsByPredicate([=](const FPaintableTexture& iTexture) { return iTexture.Texture->GetFullName() == iAssetData.GetFullName(); }));
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnObjectChanged(const FAssetData& iAssetData)
{
	UTexture2D* texture = Cast<UTexture2D>(iAssetData.GetAsset());

	if (texture)
	{
		//check
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		if (texture != mEditor->Texture() && AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
		{
			FText Title = LOCTEXT("TitleSelectedTextureAlreadyOpenedTitle", "Selected Texture Already Opened");
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedTextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
			return;
		}

        mEditor->SetTexture(texture);
	}
}

#undef LOCTEXT_NAMESPACE
