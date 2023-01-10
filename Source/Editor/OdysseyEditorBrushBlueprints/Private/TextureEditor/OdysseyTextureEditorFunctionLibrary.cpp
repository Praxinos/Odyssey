// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include <ULIS>
#include "ULISLoaderModule.h"

//---
/*
namespace
{
static
FOdysseyLayerStack*
GetStack( UOdysseyBrushAssetBase* BrushInstance )
{
    if( !BrushInstance )
        return nullptr;

    FOdysseyTextureEditorBrushContext* context = BrushInstance->GetContext<FOdysseyTextureEditorBrushContext>("FOdysseyTextureEditorBrushContext");
    if (!context)
        return nullptr;

    //---

    return context->LayerStack();
}

static
UOdysseyLayer*
GetCurrentLayer( UOdysseyLayerStack* iStack )
{
    if( !iStack )
        return nullptr;

    TSharedPtr<IOdysseyLayer> layer = iStack->GetCurrentLayer();
    check( layer );
    if( layer->GetType() != IOdysseyLayer::eType::kImage )
        return nullptr;
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>( layer );
    if( !imageLayer )
        return nullptr;

    return imageLayer;
}

static
TSharedPtr<FOdysseyImageLayer>
GetLayerByName( FOdysseyLayerStack* iStack, const FString& iName )
{
    if( !iStack )
        return nullptr;

    TArray< TSharedPtr<IOdysseyLayer> > layers;
    iStack->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

    for( auto layer : layers )
    {
        FString layer_name( layer->GetName().ToString() );
        if( layer_name == iName )
        {
            if( layer->GetType() != IOdysseyLayer::eType::kImage )
                return nullptr;
            TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>( layer );
            if( !imageLayer )
                return nullptr;

            return imageLayer;
        }
    }

    return nullptr;
}

static
TSharedPtr<FOdysseyImageLayer>
GetLayerByIndex( FOdysseyLayerStack* iStack, int iIndex )
{
    if( !iStack )
        return nullptr;

    TSharedPtr<IOdysseyLayer> layer = iStack->GetLayerFromIndex( iIndex );
    check( layer );

    if( layer->GetType() != IOdysseyLayer::eType::kImage )
        return nullptr;
        
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>( layer );
    if( !imageLayer )
        return nullptr;

    return imageLayer;
}
}

//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushInstance, int iIndex, FOdysseyBrushRect Area )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( BrushInstance );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetLayerByIndex( stack, iIndex );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    ::ULIS::FBlock* src = layer->GetBlock();
    ::ULIS::FRectI given_rect = Area.IsInitialized() ? Area.GetValue() : src->Rect();
	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(new  ::ULIS::FBlock( given_rect.w, given_rect.h, src->Format() ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(src->Format());
    
    ::ULIS::FEvent eventClear;
    ctx.Clear(*dst, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);

	//be sure we copy only the needed part //TODO: Should be done directly in ULIS
	::ULIS::FRectI src_rect = given_rect & src->Rect();
    ::ULIS::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);

    ::ULIS::FEvent eventCopy;
    ctx.Copy(*src, *dst, src_rect, dst_pos, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventClear, &eventCopy);
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventCopy);
}

//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushInstance, const FString& iName, FOdysseyBrushRect Area )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( BrushInstance );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetLayerByName( stack, iName );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    ::ULIS::FBlock* src = layer->GetBlock();
    ::ULIS::FRectI given_rect = Area.IsInitialized() ? Area.GetValue() : src->Rect();
	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(new  ::ULIS::FBlock( given_rect.w, given_rect.h, src->Format() ));
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(src->Format());
    ::ULIS::FEvent eventClear;
    ctx.Clear(*dst, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
    
	//be sure we copy only the needed part
	::ULIS::FRectI src_rect = given_rect & src->Rect();
    ::ULIS::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);

    ::ULIS::FEvent eventCopy;
    ctx.Copy(*src, *dst, src_rect, dst_pos, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventClear, &eventCopy);
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventCopy);
}

//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushInstance, FOdysseyBrushRect Area )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( BrushInstance );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetCurrentLayer( stack );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    ::ULIS::FBlock* src = layer->GetBlock();
    ::ULIS::FRectI given_rect = Area.IsInitialized() ? Area.GetValue() : src->Rect();
	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(new  ::ULIS::FBlock( given_rect.w, given_rect.h, src->Format() ));
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(src->Format());
    ::ULIS::FEvent eventClear;
    ctx.Clear(*dst, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);

	//be sure we copy only the needed part
	::ULIS::FRectI src_rect = given_rect & src->Rect();
    ::ULIS::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);

    ::ULIS::FEvent eventCopy;
    ctx.Copy(*src, *dst, src_rect, dst_pos, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventClear, &eventCopy);
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventCopy);
}

#if 0
//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetResultBlock( UOdysseyBrushAssetBase* BrushInstance )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyDrawingState* istate = BrushInstance->FindState(FOdysseyTextureEditorDrawingState::GetId() );
    //FOdysseyDrawingState* istate = *BrushInstance->GetStates().Find( FOdysseyTextureEditorDrawingState::GetId() );
    if( !istate )
        return FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyTextureEditorDrawingState* state = static_cast<FOdysseyTextureEditorDrawingState*>( istate );
    check( state );

    FOdysseyLayerStack* stack = state->LayerStack();
    //FOdysseyLayerStack* stack = const_cast<FOdysseyLayerStack*>( s->LayerStack() );

    ::ULIS::FBlock* block = stack->GetResultBlock();
    //const ::ULIS::FBlock* block = s->LayerStack()->GetResultBlock();
    FString id( "blabla" );

    return FOdysseyBlockProxy(block);
}
#endif
*/
