// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerStackAddLayerButton.h"

#include "EditorFontGlyphs.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerStack.h"
#include "SPositiveActionButton.h"
#include "OdysseyLayer.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

SOdysseyLayerStackAddLayerButton::~SOdysseyLayerStackAddLayerButton()
{
}


void SOdysseyLayerStackAddLayerButton::Construct(const FArguments& InArgs)
{
    mLayerStack = InArgs._LayerStack;
    mOnAdded = InArgs._OnAdded;

    ChildSlot
    [
        SNew(SPositiveActionButton)
        .Text(LOCTEXT("add-layer-button.add-layer", "Add"))
        .OnGetMenuContent(this, &SOdysseyLayerStackAddLayerButton::MakeMenu)
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget> SOdysseyLayerStackAddLayerButton::MakeMenu()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return SNullWidget::NullWidget;

    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("AddLayers");
    {
        //Default Layers
        for( UClass* layerClass : layerStack->CompatibleLayers )
        {
            UOdysseyLayer* layerCDO = layerClass->GetDefaultObject<UOdysseyLayer>();
            if (!layerCDO)
                continue;
                
            menuBuilder.AddMenuEntry(
                layerCDO->LayerTypeName,
                layerCDO->Description,
                layerCDO->Icon,
                FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyLayerStackAddLayerButton::AddLayerFromClass, FAssetData(layerClass)))
                //We give a FAssetData(layerClass) instead of the layerClass directly, because layerClass could be detroyed
                //between the moment the user clicks on the button, and the one he clicks on the menu entry.
                //FAssetData will load the layerClass again if it's the case when calling FAssetData::GetAsset()
            );
        }

        //Custom Layers
        /* TArray<UClass*> supportedLayerClasses = layerStack->FindSupportedCustomLayerClasses();
        for (UClass* layerClass : supportedLayerClasses)
        {
            UOdysseyLayer* layerCDO = layerClass->GetDefaultObject<UOdysseyLayer>();
            if (!layerCDO)
                continue;

            menuBuilder.AddMenuEntry(
                layerCDO->LayerTypeName,
                layerCDO->Description,
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyLayerStackAddLayerButton::AddLayerFromClass, FAssetData(layerClass)))
                //We give a FAssetData(layerClass) instead of the layerClass directly, because layerClass could be detroyed
                //between the moment the user clicks on the button, and the one he clicks on the menu entry.
                //FAssetData will load the layerClass again if it's the case when calling FAssetData::GetAsset()
            );
        } */
    }
    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

void
SOdysseyLayerStackAddLayerButton::AddLayerFromClass(FAssetData iAssetData)
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

	UObject* loadedAsset = iAssetData.FastGetAsset(true);
	if ( !loadedAsset )
		return;

	UClass* layerClass = Cast<UClass>(loadedAsset);
	if ( UBlueprint* blueprint = Cast<UBlueprint>(loadedAsset) )
		layerClass = blueprint->GeneratedClass;

	if ( !layerClass )
		return;  

#ifdef WITH_EDITOR
    //Allows to have a single undo if there is side effects (auto adding frames in animation layer) in mOnAdded callback
    FScopedTransaction ScopedTransaction(LOCTEXT("add-layer-button.transaction.add-layer", "Add Layer"));
#endif

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (currentLayer)
    {
        if (currentLayer->CanHaveChildren && currentLayer->DisplayChildren)
        {
			currentLayer = layerStack->AddLayer(layerClass, currentLayer);
        }
        else
        {
			UOdysseyLayer* parent = currentLayer->GetParent();
			int index = currentLayer->GetIndexInParent();
			currentLayer = layerStack->AddLayer(layerClass, parent, index);
        }
    }
    else
    {
		currentLayer = layerStack->AddLayer(layerClass);
    }
    
    mOnAdded.ExecuteIfBound(currentLayer);
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(currentLayer));
}

#undef LOCTEXT_NAMESPACE
