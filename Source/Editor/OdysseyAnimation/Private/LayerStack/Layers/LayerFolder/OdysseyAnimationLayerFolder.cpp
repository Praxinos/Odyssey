// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolderImageRenderingAbility.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerFolder"

UOdysseyAnimationLayerFolder::FOnBlendModeChanged&
UOdysseyAnimationLayerFolder::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyAnimationLayerFolder::FOnOpacityChanged&
UOdysseyAnimationLayerFolder::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyAnimationLayerFolder::UOdysseyAnimationLayerFolder()
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Folder Layer");
    DefaultName = LOCTEXT("DefaultName", "Folder");
    Icon = *FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderClosed" );
    IconExpanded = *FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderOpen" );
    CanHaveChildren = true;
}

TSet<UClass*>
UOdysseyAnimationLayerFolder::GetMergeDefaultLayerTypes() const
{
    TSet<UClass*> types;
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return types;

    return layerStack->FindLayersMergeTypes(Children);
}

TSet<UClass*>
UOdysseyAnimationLayerFolder::GetMergeLayerTypesFromTypes(TSet<UClass*> iLayerTypes) const
{
    TSet<UClass*> types;
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return types;

    for (UOdysseyLayer* layer : Children )
    {
        TSet<UClass*> layerTypes = layer->GetMergeLayerTypesFromTypes(iLayerTypes);
        types.Append(layerTypes);
    }

    return types;
}

void
UOdysseyAnimationLayerFolder::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !imageRenderAbility )
        return;
    {
        //TODO: react to interactive events by not commiting immediately
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
        imageRenderAbility->Changed(imageRenderAbility->GetId(), {rect});
        imageRenderAbility->Commited(imageRenderAbility->GetId(), {rect});
    }
}

void
UOdysseyAnimationLayerFolder::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !imageRenderAbility )
        return;
    {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
        imageRenderAbility->Changed(imageRenderAbility->GetId(), {rect});
        imageRenderAbility->Commited(imageRenderAbility->GetId(), {rect});
    }
}

void
UOdysseyAnimationLayerFolder::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

void
UOdysseyAnimationLayerFolder::PostInitProperties()
{
    Super::PostInitProperties();

    SetAbility(MakeShared<FOdysseyAnimationLayerFolderImageRenderingAbility>(this));
}

#undef LOCTEXT_NAMESPACE
