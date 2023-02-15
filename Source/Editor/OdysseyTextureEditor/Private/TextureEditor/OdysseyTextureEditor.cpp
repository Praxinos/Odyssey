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
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	SetTexture(nullptr);
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mTexture(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mVectorPathDrawingTool(nullptr),
	mVectorPathEditTool(nullptr),
	mVectorPathCutTool(nullptr),
	mVectorObjectPickTool(nullptr),
	mVectorObjectMoveTool(nullptr),
	mVectorObjectRotateTool(nullptr),
	mVectorObjectScaleTool(nullptr),
	mVectorSceneScaleTool(nullptr),
	mVectorScenePanTool(nullptr),
	mVectorEraserTool(nullptr),
	mPaintBucketTool(nullptr),
	mColorPickerTool(nullptr)
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
	mRasterDrawingTool = NewObject<UOdysseyTextureEditorRasterDrawingTool>();
	mVectorPathDrawingTool = NewObject<UOdysseyTextureEditorVectorPathDrawingTool>();
	mVectorPathEditTool = NewObject<UOdysseyTextureEditorVectorPathEditTool>();
	mVectorPathCutTool = NewObject<UOdysseyTextureEditorVectorPathCutTool>();
	mVectorObjectPickTool = NewObject<UOdysseyTextureEditorVectorObjectPickTool>();
	mVectorObjectMoveTool = NewObject<UOdysseyTextureEditorVectorObjectMoveTool>();
	mVectorObjectRotateTool = NewObject<UOdysseyTextureEditorVectorObjectRotateTool>();
	mVectorObjectScaleTool = NewObject<UOdysseyTextureEditorVectorObjectScaleTool>();
    mVectorSceneScaleTool = NewObject<UOdysseyTextureEditorVectorSceneScaleTool>();
    mVectorScenePanTool = NewObject<UOdysseyTextureEditorVectorScenePanTool>();
    mVectorEraserTool = NewObject<UOdysseyTextureEditorVectorEraserTool>();
	mPaintBucketTool = NewObject<UOdysseyTextureEditorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyTextureEditorColorPickerTool>();

	mRasterDrawingTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
    mVectorObjectPickTool->SetEditor(this);
    mVectorObjectMoveTool->SetEditor(this);
    mVectorObjectRotateTool->SetEditor(this);
    mVectorObjectScaleTool->SetEditor(this);
    mVectorSceneScaleTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
    mTools.Add(mVectorPathDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorPathCutTool);
    mTools.Add(mVectorObjectPickTool);
    mTools.Add(mVectorObjectMoveTool);
    mTools.Add(mVectorObjectRotateTool);
    mTools.Add(mVectorObjectScaleTool);
    mTools.Add(mVectorSceneScaleTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
	mTools.Add(mPaintBucketTool);
	mTools.Add(mColorPickerTool);
	//mTextureRasterDrawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
}

void
FOdysseyTextureEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mVectorPathDrawingTool->BindShortcuts(iToolkit);
	mVectorPathEditTool->BindShortcuts(iToolkit);
	mVectorPathCutTool->BindShortcuts(iToolkit);
	mVectorObjectPickTool->BindShortcuts(iToolkit);
	mVectorObjectMoveTool->BindShortcuts(iToolkit);
	mVectorObjectRotateTool->BindShortcuts(iToolkit);
	mVectorObjectScaleTool->BindShortcuts(iToolkit);
	mVectorSceneScaleTool->BindShortcuts(iToolkit);
	mVectorScenePanTool->BindShortcuts(iToolkit);
	mVectorEraserTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
	mColorPickerTool->BindShortcuts(iToolkit);
}

void
FOdysseyTextureEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathEditTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathCutTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectPickTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectMoveTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectRotateTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectScaleTool->ExtendMenu(iOwner, iMenuName);
	mVectorSceneScaleTool->ExtendMenu(iOwner, iMenuName);
	mVectorScenePanTool->ExtendMenu(iOwner, iMenuName);
	mVectorEraserTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
	mColorPickerTool->ExtendMenu(iOwner, iMenuName);
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

UOdysseyTextureEditorVectorPathDrawingTool*
FOdysseyTextureEditor::GetVectorPathDrawingTool() const
{
	return mVectorPathDrawingTool;
}

UOdysseyTextureEditorVectorPathEditTool*
FOdysseyTextureEditor::GetVectorPathEditTool() const
{
    return mVectorPathEditTool;
}

UOdysseyTextureEditorVectorPathCutTool*
FOdysseyTextureEditor::GetVectorPathCutTool() const
{
    return mVectorPathCutTool;
}

UOdysseyTextureEditorVectorObjectPickTool*
FOdysseyTextureEditor::GetVectorObjectPickTool() const
{
    return mVectorObjectPickTool;
}

UOdysseyTextureEditorVectorObjectMoveTool*
FOdysseyTextureEditor::GetVectorObjectMoveTool() const
{
    return mVectorObjectMoveTool;
}

UOdysseyTextureEditorVectorObjectRotateTool*
FOdysseyTextureEditor::GetVectorObjectRotateTool() const
{
    return mVectorObjectRotateTool;
}

UOdysseyTextureEditorVectorObjectScaleTool*
FOdysseyTextureEditor::GetVectorObjectScaleTool() const
{
    return mVectorObjectScaleTool;
}

UOdysseyTextureEditorVectorSceneScaleTool*
FOdysseyTextureEditor::GetVectorSceneScaleTool() const
{
    return mVectorSceneScaleTool;
}

UOdysseyTextureEditorVectorScenePanTool*
FOdysseyTextureEditor::GetVectorScenePanTool() const
{
    return mVectorScenePanTool;
}

UOdysseyTextureEditorVectorEraserTool*
FOdysseyTextureEditor::GetVectorEraserTool() const
{
    return mVectorEraserTool;
}

UOdysseyTextureEditorPaintBucketTool*
FOdysseyTextureEditor::GetPaintBucketTool() const
{
	return mPaintBucketTool;
}

UOdysseyTextureEditorColorPickerTool*
FOdysseyTextureEditor::GetColorPickerTool() const
{
    return mColorPickerTool;
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
	//TODO: Maybe this should be done differently later, but we don't have time for that now
	if (iLayerStack == LayerStack())
		SelectDefaultTool(); //Refresh the current tool when we change layer
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyTextureEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
	Collector.AddReferencedObject(mVectorObjectPickTool);
	Collector.AddReferencedObject(mVectorObjectMoveTool);
	Collector.AddReferencedObject(mVectorObjectRotateTool);
	Collector.AddReferencedObject(mVectorObjectScaleTool);
    Collector.AddReferencedObject(mVectorSceneScaleTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
}
#undef LOCTEXT_NAMESPACE