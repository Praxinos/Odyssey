// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditor.h"

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"


#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
	SetTexture(nullptr);
	/*UOdysseyTextureAssetUserData* userData = TextureUserData();
	if (userData)
		userData->StopEdit();
	mTexture = nullptr;*/
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mTexture(nullptr),
	mGUI(nullptr),
	mEditedBlock(nullptr)
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor(UTexture2D* iTexture) :
	FOdysseyPainterEditor(),
	mTexture(nullptr),
	mGUI(nullptr),
	mEditedBlock(nullptr)
{
	SetTexture(iTexture);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyTextureEditor::SetTexture(UTexture2D* iTexture)
{
	PaintEngine().Block(nullptr);
	delete mEditedBlock;
	mEditedBlock = nullptr;

	//Should be managed by the tool
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	UOdysseyTextureLayer::OnRenderImageChanged().RemoveAll(this);
	UOdysseyLayer::OnIsLockedChanged().RemoveAll(this);
	UOdysseyLayer::OnIsActivatedChanged().RemoveAll(this);
	PaintEngine().OnCommitDelegate().RemoveAll(this);
	PaintEngine().OnPreUpdateDelegate().Unbind();

    //close userdata
	UOdysseyTextureLayerStack* layerStack = LayerStack();
	if ( layerStack )
		layerStack->StopEdit();


	//Set the texture
    mTexture = iTexture;
	if ( !mTexture )
		return;

	layerStack = LayerStack();
	if ( layerStack )
		layerStack->StartEdit();

	
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditor::OnCurrentLayerChanged);
	UOdysseyTextureLayer::OnRenderImageChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerRenderImageChanged);
	UOdysseyLayer::OnIsLockedChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerIsLockedChanged);
	UOdysseyLayer::OnIsActivatedChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerIsActivatedChanged);
	PaintEngine().OnCommitDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPaintEngineCommit);
	PaintEngine().OnPreUpdateDelegate().BindRaw(this, &FOdysseyTextureEditor::OnPaintEnginePreUpdate);

	OnCurrentLayerChanged(LayerStack());
    //open userdata
    //TextureChanged(); 
}

UTexture2D*
FOdysseyTextureEditor::Texture() const
{
	return mTexture;
}

UOdysseyTextureLayerStack*
FOdysseyTextureEditor::LayerStack() const
{
	UOdysseyTextureAssetUserData* userData = TextureUserData();
	if (!userData)
		return nullptr;
	
	return userData->GetLayerStack();
}

FOdysseySurfaceTexture2DEditable*
FOdysseyTextureEditor::DisplaySurface() const
{
	UOdysseyTextureLayerStack* layerStack = LayerStack();
	if (!layerStack )
		return nullptr;
	
	return layerStack->GetSurface().Get();
}

UOdysseyTextureAssetUserData*
FOdysseyTextureEditor::TextureUserData() const
{
	if ( !mTexture )
		return nullptr;

    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if (userData)
        return userData;

    //Init user data
	userData = NewObject<UOdysseyTextureAssetUserData>(mTexture, NAME_None, RF_Public);

    // Notify for changes
    mTexture->AddAssetUserData( userData );
    mTexture->PostEditChange();
    return userData;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

bool
FOdysseyTextureEditor::OnCloseRequested()
{
	SetTexture(nullptr); //close properly the currently loaded texture
	return true;
}

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

void
FOdysseyTextureEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( !iLayerStack )
		return;

	UOdysseyTextureLayerStack* layerstack = LayerStack();
	if ( !layerstack || layerstack != iLayerStack )
		return;

	//Finalize the current Tool
	PaintEngine().Block(nullptr);
	delete mEditedBlock;
	mEditedBlock = nullptr;

	if ( !layerstack->CurrentLayer )
		return;

	//Define the new active tool based on the layer type
	UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
	if ( !layerImageRaster )
		return;

	const ::ULIS::FBlock* block = layerImageRaster->GetBlock();
	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
	mEditedBlock = new ::ULIS::FBlock(block->Width(), block->Height(), block->Format(), nullptr, ::ULIS::FOnInvalidBlock(&OnEditedBlockInvalidated, static_cast<void*>(this)));
	ctx.Copy(
		*block,
		*mEditedBlock
	);
	ctx.Finish();

	PaintEngine().Block(mEditedBlock);

	SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo)
{
	//Indicate UObject system that we will change LayersHierarchy property
	//(actually the block has already changed, but there no where else to call it before here and being sure that PostChangePropertyValue will be called after)

	TArray<::ULIS::FRectI> rects(iRects, iNumRects);
	FOdysseyTextureEditor* self = static_cast<FOdysseyTextureEditor*>(iInfo);

	UOdysseyTextureLayerStack* layerstack = self->LayerStack();
	if ( !layerstack )
		return;

	UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
	if ( layerImageRaster )
		layerImageRaster->SetRenderBlockOverride(self->mEditedBlock);

	for ( int i = 0; i < rects.Num(); i++ )
		layerstack->RenderImage(self->DisplaySurface()->Block(), rects[i], rects[i].Position(), TArray<::ULIS::FEvent>());

	if ( layerImageRaster )
		layerImageRaster->SetRenderBlockOverride(nullptr);

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(self->DisplaySurface()->Block()->Format());
	ctx.Finish();

	self->DisplaySurface()->Invalidate(rects);
}

void
FOdysseyTextureEditor::OnLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects)
{
	UOdysseyTextureLayerStack* layerstack = LayerStack();
	if ( !layerstack )
		return;

	//What comes under this line should be in directly in the raster painting tool
	//Reset the editedBloock and PaintEngine (usefull when undoing)
	if ( !layerstack->CurrentLayer || iLayer != layerstack->CurrentLayer )
		return;

	//Define the new active tool based on the layer type
	UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(iLayer);
	if ( !layerImageRaster )
		return;

	const ::ULIS::FBlock* block = layerImageRaster->GetBlock();
	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
	for ( int i = 0; i < iRects.Num(); i++ )
	{
		ctx.Copy(
			*block,
			*mEditedBlock,
			iRects[i],
			iRects[i].Position()
		);

		//TODO: put this Finish() outside of this loop when we can be sure all the rects don't overlap
		ctx.Finish();
	}

	PaintEngine().Reset();
}

void
FOdysseyTextureEditor::OnLayerIsLockedChanged(UOdysseyLayer* iLayer)
{
	UOdysseyLayerStack* layerStack = LayerStack();
	if ( !iLayer || !layerStack )
		return;

	if ( iLayer->GetLayerStack() != layerStack )
		return;

	SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::OnLayerIsActivatedChanged(UOdysseyLayer* iLayer)
{
	UOdysseyLayerStack* layerStack = LayerStack();
	if ( !iLayer || !layerStack )
		return;

	if ( iLayer->GetLayerStack() != layerStack )
		return;

	SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::SetSelectedToolDrawingLocked()
{
	UOdysseyTextureLayerStack* layerstack = LayerStack();
	if ( !layerstack )
		return;

	if ( !layerstack->CurrentLayer )
		return;

	UOdysseyDrawingTool* drawingTool = Cast<UOdysseyDrawingTool>(mSelectedTool);
	if ( !drawingTool )
		return;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layerstack->CurrentLayer.Get());
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layerstack->CurrentLayer.Get());
	drawingTool->IsDrawingLocked(!isActive || isLocked);
}

void
FOdysseyTextureEditor::OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles)
{
	UOdysseyTextureLayerStack* layerstack = LayerStack();
	if ( !layerstack )
		return;

	if ( iChangedTiles.Num() <= 0 )
		return;

	UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
	if ( !layer )
		return;

	GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "Paint Stroke"), nullptr);

	layer->UpdateBlock(*mEditedBlock, iChangedTiles, ::ULIS::FVec2F(0), TArray<::ULIS::FEvent>());

	GEditor->EndTransaction();
}

FOdysseyBlendParameters
FOdysseyTextureEditor::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	//Make sure we set the right value in the Paint Engine according to the editor state
	UOdysseyTextureLayerStack* layerstack = LayerStack();
	if ( !layerstack )
		return blendParameters;

	if ( !layerstack->CurrentLayer )
		return blendParameters;

	//Define the new active tool based on the layer capabilities

	UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
	if ( !layerImageRaster )
		return blendParameters;

	if ( layerImageRaster->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE