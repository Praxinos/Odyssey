// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditor.h"

#include "TextureEditor/OdysseyTextureEditorGUI.h"
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
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	SetTexture(nullptr);
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mTexture(nullptr),
	mGUI(nullptr)
{
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditor::OnCurrentLayerChanged);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditor::InitData(UObject* iEditedObject)
{
	
	UTexture2D* texture = Cast<UTexture2D>(iEditedObject);
	if (texture)
		SetTexture(texture);

	//Call it there so that tools are initialized after basic data
	FOdysseyPainterEditor::InitData(iEditedObject);
}

void
FOdysseyTextureEditor::InitTools()
{
	FOdysseyPainterEditor::InitTools();
	UpdateToolContext();
	
	/* mToolContext->GetRasterBlockAttribute().BindRaw(this, &FOdysseyTextureEditor::GetCurrentRasterBlock);
	mToolContext->GetIsRasterBlockReadOnlyAttribute().BindRaw(this, &FOdysseyTextureEditor::IsRasterBlockReadOnly);
    mToolContext->GetVectorEngineAttribute().BindRaw(this, &FOdysseyTextureEditor::GetCurrentVectorEngine); */
}

void
FOdysseyTextureEditor::Tick(float iDeltaTime)
{
	UpdateToolContext();
	FOdysseyPainterEditor::Tick(iDeltaTime);
}

void
FOdysseyTextureEditor::UpdateToolContext()
{
	FOdysseyPainterEditorToolContext::FParams toolContextParams;
	toolContextParams.mRasterBlock = GetCurrentRasterBlock();
	toolContextParams.mIsRasterBlockReadOnly = IsRasterBlockReadOnly();
	toolContextParams.mVectorEngine = GetCurrentVectorEngine();
	
	mToolContext->Set(toolContextParams);
}

void
FOdysseyTextureEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mVectorPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPathDrawingTool->BindShortcuts(iToolkit);
	mVectorPathEditTool->BindShortcuts(iToolkit);
	mVectorPathCutTool->BindShortcuts(iToolkit);
	mVectorPickTool->BindShortcuts(iToolkit);
	mVectorSceneScaleTool->BindShortcuts(iToolkit);
	mVectorScenePanTool->BindShortcuts(iToolkit);
	mVectorEraserTool->BindShortcuts(iToolkit);
	mVectorPathPushTool->BindShortcuts(iToolkit);
	mVectorPathWidthTool->BindShortcuts(iToolkit);
	mVectorPathSmoothTool->BindShortcuts(iToolkit);
	mVectorPathStitchTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
	mColorPickerTool->BindShortcuts(iToolkit);
	mVectorGridTool->BindShortcuts(iToolkit);
	mVectorTransformTool->BindShortcuts(iToolkit);
}

void
FOdysseyTextureEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathEditTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathCutTool->ExtendMenu(iOwner, iMenuName);
	mVectorPickTool->ExtendMenu(iOwner, iMenuName);
	mVectorSceneScaleTool->ExtendMenu(iOwner, iMenuName);
	mVectorScenePanTool->ExtendMenu(iOwner, iMenuName);
	mVectorEraserTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathPushTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathWidthTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathSmoothTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathStitchTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
	mColorPickerTool->ExtendMenu(iOwner, iMenuName);
	mVectorGridTool->ExtendMenu(iOwner, iMenuName);
	mVectorTransformTool->ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyTextureEditor::SetTexture(UTexture2D* iTexture)
{
	if (mSelectedTool)
		mSelectedTool->Inactivate();

    //Inactivate Fast Update
	UOdysseyTextureLayerStack* layerStack = LayerStack();
	if ( layerStack )
		layerStack->InactivateTextureFastUpdate();

	//Set the texture
    mTexture = iTexture;
	if ( !mTexture )
	{
		SetSelectedTool(nullptr);
		mLayerStackPreloadHandle = nullptr;
		return;
	}

	layerStack = LayerStack();
	if ( layerStack )
	{
		//Do it in 3 lines to avoid unexpected handles destruction in the process
		mLayerStackPreloadHandle = layerStack->Preload();
		
		layerStack->ActivateTextureFastUpdate();
	}

	if ( mSelectedTool && mSelectedTool->IsActivable() )
	{
		//just reload the tool
		mSelectedTool->Inactivate();
		mSelectedTool->Activate();
	}
	else
	{
		//select the best tool
		SelectDefaultTool();
	}
}

UTexture2D*
FOdysseyTextureEditor::Texture() const
{
	return mTexture;
}

UOdysseyTextureLayerStack*
FOdysseyTextureEditor::LayerStack() const
{
	UOdysseyTextureLayerStackUserData* userData = TextureUserData();
	if (!userData)
		return nullptr;
	
	return userData->GetLayerStack();
}

UTexture*
FOdysseyTextureEditor::DisplayTexture() const
{
	return mTexture;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyTextureEditor::GetDisplayBlock()
{
	UOdysseyTextureLayerStack* layerStack = LayerStack();
	if (!layerStack)
		return nullptr;

	return layerStack->GetSurface()->Block();
}

UOdysseyTextureLayerStackUserData*
FOdysseyTextureEditor::TextureUserData() const
{
	if ( !mTexture )
		return nullptr;

    UOdysseyTextureLayerStackUserData* userData = Cast<UOdysseyTextureLayerStackUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    if (userData)
        return userData;

    //Init user data
	userData = NewObject<UOdysseyTextureLayerStackUserData>(mTexture, NAME_None, RF_Public);
    userData->InitWithDefaultLayerStack();

    // Notify for changes
    mTexture->AddAssetUserData( userData );
    mTexture->PostEditChange();
    return userData;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyTextureEditor::GetCurrentRasterBlock() const
{
	if ( !LayerStack() )
		return nullptr;

    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(LayerStack()->CurrentLayer.Get());

	if (!currentLayerRaster)
		return nullptr;
	
	return currentLayerRaster->GetRasterBlock();
}

bool
FOdysseyTextureEditor::IsRasterBlockReadOnly() const
{
	UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerRaster)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(currentLayerRaster);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(currentLayerRaster);

	return !isActive || isLocked;
}

FOdysseyVectorEngine*
FOdysseyTextureEditor::GetCurrentVectorEngine() const
{
	if ( !LayerStack() )
		return nullptr;

    UOdysseyTextureLayerImageVector* currentLayerVector = Cast<UOdysseyTextureLayerImageVector>(LayerStack()->CurrentLayer.Get());

	if (!currentLayerVector)
		return nullptr;
	
	return currentLayerVector->GetEngine();
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

	//TODO: Maybe this should be done differently later, but we don't have time for that now
	SelectDefaultTool(); //Refresh the current tool when we change layer
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyTextureEditor::Clear()
{
	if ( !LayerStack() )
		return;

    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(LayerStack()->CurrentLayer.Get());
    UOdysseyTextureLayerImageVector* currentLayerVector = Cast<UOdysseyTextureLayerImageVector>(LayerStack()->CurrentLayer.Get());

	if (currentLayerRaster)
	{		
	#ifdef WITH_EDITOR
		FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Clear Canvas"));
	#endif
		
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = currentLayerRaster->GetRasterBlock();
		FOdysseyRasterBlockMutator mutator(rasterBlock);
		mutator.EditTilesFromRects(
			{ ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
			FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
				[&](const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
				{
					TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = rasterBlock->GetBlock();
					::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
					::ULIS::FEvent clearEvent;
					ctx.Clear(*ULISRasterBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &clearEvent);
					return { clearEvent };
				}
			)
		);
		mutator.Commit();
	}

    if( currentLayerVector )
    {
        FOdysseyVectorEngine* vectorEngine = currentLayerVector->GetEngine();

        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("ClearVectorScene", "Clear Vector Scene"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoEngineClear( vectorEngine );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        vectorEngine->SetScene( new FOdysseyVectorScene("Scene") );

        currentLayerVector->RenderImageChanged(false);
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyTextureEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
	Collector.AddReferencedObject(mVectorPickTool);
    Collector.AddReferencedObject(mVectorSceneScaleTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
    Collector.AddReferencedObject(mVectorPathPushTool);
    Collector.AddReferencedObject(mVectorPathWidthTool);
    Collector.AddReferencedObject(mVectorPathSmoothTool);
    Collector.AddReferencedObject(mVectorPathStitchTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
	Collector.AddReferencedObject(mVectorGridTool);
	Collector.AddReferencedObject(mVectorTransformTool);
}
#undef LOCTEXT_NAMESPACE