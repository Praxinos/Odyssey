// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerFolder.h"
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

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerFolder::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    if (Children.Num() <= 0)
        return iWaitList;

    ::ULIS::eFormat format = ioBlock->Format();

    //Generate the folder block, which is all children layers blended together
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> folderBlock = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, format);

    //Render children Image
    TArray<::ULIS::FEvent> eventRenderChildrenImage = RenderLayersImage(Children, folderBlock, iFrame, iRect, ::ULIS::FVec2I(0), TArray<::ULIS::FEvent>());
    eventRenderChildrenImage.Append(iWaitList);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(folderBlock->Format());

    //Convert the destination if needed and Blend the folderBlock
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, folderBlock->Format(), iRect, iPos, eventRenderChildrenImage,
        [this, &folderBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            //if ioBlock and mBlock use same format, Blend directly in ioBlock
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(folderBlock).Build();

            ctx.Blend(
                *folderBlock,
                *ioDest,
                ::ULIS::FRectI::Auto,
                iPos,
                ::ULIS::eBlendMode(BlendMode),
                ::ULIS::Alpha_Normal,
                Opacity,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );

            return { eventBlend };
        }
    );
    
    ctx.Flush();

    return eventConvertAndExecute;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerFolder::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    if (Children.Num() <= 0)
        return iWaitList;

    //ensure we use a sourceBlock with an alpha channel
    ::ULIS::eFormat format = ioBlock->Format();

    //Generate the folder block, which is all children layers blended together
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> folderBlock = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, format);

    //Render children Image
    TArray<::ULIS::FEvent> eventRenderChildrenImage = RenderLayersImage(Children, folderBlock, iFrame, iRect, ::ULIS::FVec2I(0), TArray<::ULIS::FEvent>());
    eventRenderChildrenImage.Append(iWaitList);
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(folderBlock->Format());

    //Convert the destination if needed and copy the folderBlock
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, folderBlock->Format(), iRect, iPos, eventRenderChildrenImage,
        [this, &ctx, &folderBlock](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            //if ioBlock and mBlock use same format, Copy directly in ioBlock
            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(folderBlock).Build();

            ctx.Copy(
                *folderBlock,
                *ioDest,
                ::ULIS::FRectI::Auto,
                iPos,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventCopy
            );

            return { eventCopy };
        }
    );
    
    ctx.Flush();

    return eventConvertAndExecute;
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
    RenderImageChanged(false);
}

void
UOdysseyAnimationLayerFolder::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    RenderImageChanged(false);
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

#undef LOCTEXT_NAMESPACE
