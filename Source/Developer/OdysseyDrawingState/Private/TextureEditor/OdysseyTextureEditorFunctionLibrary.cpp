// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "TextureEditor/OdysseyTextureEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyLayerStack.h"
#include "TextureEditor/OdysseyTextureEditorState.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

//---

namespace
{
static
FOdysseyLayerStack*
GetStack( UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext )
        return nullptr;

    FOdysseyDrawingState* istate = BrushContext->FindState( FOdysseyTextureEditorState::GetId() );
    if( !istate )
        return nullptr;

    FOdysseyTextureEditorState* state = static_cast<FOdysseyTextureEditorState*>( istate );
    check( state );
    if( !state )
        return nullptr;

    //---

    return state->LayerStack();
}

static
TSharedPtr<FOdysseyImageLayer>
GetCurrentLayer( FOdysseyLayerStack* iStack )
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
UOdysseyTextureEditorFunctionLibrary::GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* iBrushContext, int iIndex, FOdysseyBrushRect Area )
{
    if( !iBrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( iBrushContext );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetLayerByIndex( stack, iIndex );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyBlock* src = layer->GetBlock();
    ::ul3::FRect given_rect = Area.IsInitialized() ? Area.GetValue() : src->GetBlock()->Rect();
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( given_rect.w, given_rect.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = given_rect.h > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42; //CRASH: ULIS3_PERF_SSE42 crash when copying block of 1-3 pixels wide

	//be sure we copy only the needed part //TODO: Should be done directly in ULIS
	::ul3::FRect src_rect = given_rect & src->GetBlock()->Rect();
    ::ul3::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src_rect, dst_pos );

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetBlockOfLayerByName( UOdysseyBrushAssetBase* iBrushContext, const FString& iName, FOdysseyBrushRect Area )
{
    if( !iBrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( iBrushContext );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetLayerByName( stack, iName );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyBlock* src = layer->GetBlock();
    ::ul3::FRect given_rect = Area.IsInitialized() ? Area.GetValue() : src->GetBlock()->Rect();
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( given_rect.w, given_rect.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = given_rect.h > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42; //CRASH: ULIS3_PERF_SSE42 crash when copying block of 1-3 pixels wide

	//be sure we copy only the needed part
	::ul3::FRect src_rect = given_rect & src->GetBlock()->Rect();
    ::ul3::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src_rect, dst_pos );

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* iBrushContext, FOdysseyBrushRect Area )
{
    if( !iBrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyLayerStack* stack = GetStack( iBrushContext );
    if( !stack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyImageLayer> layer = GetCurrentLayer( stack );
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    FOdysseyBlock* src = layer->GetBlock();
    ::ul3::FRect given_rect = Area.IsInitialized() ? Area.GetValue() : src->GetBlock()->Rect();
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( given_rect.w, given_rect.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = given_rect.h > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42; //CRASH: ULIS3_PERF_SSE42 crash when copying block of 1-3 pixels wide

	//be sure we copy only the needed part
	::ul3::FRect src_rect = given_rect & src->GetBlock()->Rect();
    ::ul3::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src_rect, dst_pos );

    return FOdysseyBlockProxy(dst);
}

#if 0
//static
FOdysseyBlockProxy
UOdysseyTextureEditorFunctionLibrary::GetResultBlock( UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyDrawingState* istate = BrushContext->FindState( FOdysseyTextureEditorState::GetId() );
    //FOdysseyDrawingState* istate = *BrushContext->GetStates().Find( FOdysseyTextureEditorState::GetId() );
    if( !istate )
        return FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyTextureEditorState* state = static_cast<FOdysseyTextureEditorState*>( istate );
    check( state );

    FOdysseyLayerStack* stack = state->LayerStack();
    //FOdysseyLayerStack* stack = const_cast<FOdysseyLayerStack*>( s->LayerStack() );

    FOdysseyBlock* block = stack->GetResultBlock();
    //const FOdysseyBlock* block = s->LayerStack()->GetResultBlock();
    FString id( "blabla" );

    return FOdysseyBlockProxy(block);
}
#endif
