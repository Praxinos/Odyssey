// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditor.h"
/*
#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "Undo/OdysseyVectorUndoEngineClear.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UTexture2D*
FOdysseyTextureEditor::Texture() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
	if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
		return nullptr;

	TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
	return textureSource->GetTexture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyTextureEditorGUI*
FOdysseyTextureEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyTextureEditorGUI(this));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyTextureEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyTextureEditor", "Odyssey Texture2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}
//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
FOdysseyTextureEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;

    // PATCH : We have to redraw all layers in order to draw all layers without the HUD of the tool.
    // This will be removed when we'll have a dedicated HUD layer.
    UOdysseyTextureLayer* layerRoot = static_cast<UOdysseyTextureLayer*>(iLayerStack->LayerRoot);
	layerRoot->RenderImageChanged(false);
}

#undef LOCTEXT_NAMESPACE */