// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorExtension.h"

#include "LayerStack/OdysseyTextureLayer.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorExtension"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyTextureEditorExtension::~FOdysseyTextureEditorExtension()
{
}

FOdysseyTextureEditorExtension::FOdysseyTextureEditorExtension(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorExtension(iEditor)
	, mTextureSource(nullptr)
	, mGUI(nullptr)
{
}

void
FOdysseyTextureEditorExtension::Initialize()
{
	mGUI = MakeShared<FOdysseyTextureEditorGUI>(this);
	mGUI->Init();

    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyTextureEditorExtension::OnSourceChanged);
}

void
FOdysseyTextureEditorExtension::Finalize()
{
    GetEditor()->OnSourceChanged().RemoveAll(this);
}

void
FOdysseyTextureEditorExtension::ExtendMenu( FToolMenuOwner iOwnerFName, FName iMenuName )
{
	mGUI->ExtendMenu(iOwnerFName, iMenuName);
}

void
FOdysseyTextureEditorExtension::BindShortcuts(FBaseToolkit* iToolkit)
{
	mGUI->BindShortcuts(iToolkit);
}

void
FOdysseyTextureEditorExtension::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    mGUI->BuildLayout(iBuilder);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

void
FOdysseyTextureEditorExtension::OnSourceChanged()
{
	mTextureSource = nullptr;
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);

    //Is the source an texture
    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
	if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())	
		return;

	mTextureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorExtension::OnCurrentLayerChanged);
}

UTexture2D*
FOdysseyTextureEditorExtension::Texture() const
{
	if (!mTextureSource)
		return nullptr;

	return mTextureSource->GetTexture();
}

void
FOdysseyTextureEditorExtension::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != mTextureSource->GetLayerStack() )
		return;

    // PATCH : We have to redraw all layers in order to draw all layers without the HUD of the tool.
    // This will be removed when we'll have a dedicated HUD layer.
    UOdysseyTextureLayer* layerRoot = static_cast<UOdysseyTextureLayer*>(iLayerStack->LayerRoot);
	layerRoot->RenderImageChanged(false);
}

#undef LOCTEXT_NAMESPACE