// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditor.h"

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"


#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
	SetTexture(nullptr);
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mTexture(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mPaintBucketTool(nullptr)
{
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
	mRasterDrawingTool = NewObject<UOdysseyTextureEditorRasterDrawingTool>();
	mPaintBucketTool = NewObject<UOdysseyTextureEditorPaintBucketTool>();

	mRasterDrawingTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
	mTools.Add(mPaintBucketTool);
	//mTextureRasterDrawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
}

void
FOdysseyTextureEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
}

void
FOdysseyTextureEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
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

    // Notify for changes
    mTexture->AddAssetUserData( userData );
    mTexture->PostEditChange();
    return userData;
}

UOdysseyTextureEditorRasterDrawingTool*
FOdysseyTextureEditor::GetRasterDrawingTool() const
{
	return mRasterDrawingTool;
}

UOdysseyTextureEditorPaintBucketTool*
FOdysseyTextureEditor::GetPaintBucketTool() const
{
	return mPaintBucketTool;
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
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyTextureEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
	Collector.AddReferencedObject(mPaintBucketTool);
}
#undef LOCTEXT_NAMESPACE