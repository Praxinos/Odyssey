// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushBlock.h"

#include "Engine/Font.h"

#include "OdysseySurfaceEditable.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyBlockFormat Format, UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext )
        return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Texture )
        return  FOdysseyBlockProxy::MakeNullProxy();

    FString op = "Conv_" + Texture->GetName();
    ECacheLevel level = ECacheLevel::kSuper;

    if( BrushContext->KeyExistsInPool( level, op ) )
        return BrushContext->RetrieveInPool( level, op );

    //---

    ::ul3::tFormat defaultFormat = BrushContext->GetState().target_temp_buffer ? BrushContext->GetState().target_temp_buffer->Format() : ULIS3_FORMAT_RGBA8;
	::ul3::tFormat format = ULISFormatFromOdysseyBlockFormat(Format, defaultFormat);
    
    FOdysseyBlock* block = NewOdysseyBlockFromUTextureData( Texture, format );

    FOdysseyBlockProxy prox( block, op );
    BrushContext->StoreInPool( level, op, prox );
    return prox;
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( UOdysseyBrushAssetBase* BrushContext
                                                    , FOdysseyBlockProxy Source
                                                    , FOdysseyBrushColor Color
                                                    , ECacheLevel Cache )
{
    if( !BrushContext )
        return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Source.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    ::ul3::FPixelValue color = ::ul3::Conv( Color.GetValue(), Source.m->Format() );
    FString colorID = FString::FromBlob( ( const uint8* )color.Ptr(), color.Depth() );
    FString op = "FillPreserveAlpha_" + colorID + "_" + Source.id;

    if( BrushContext->KeyExistsInPool( Cache, op ) )
        return  BrushContext->RetrieveInPool( Cache, op );

    //---

    FOdysseyBlock* src = Source.m;
    FOdysseyBlock* dst = new  FOdysseyBlock( src->Width(), src->Height(), src->Format() );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = src->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
    ::ul3::FillPreserveAlpha( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, dst->GetBlock()->Rect() );

    FOdysseyBlockProxy prox( dst, op );
    BrushContext->StoreInPool( Cache, op, prox );
    return prox;
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                              , int Width
                                              , int Height
                                              , EOdysseyBlockFormat Format
                                              , const FString& ID
                                              , bool InitializeData
                                              , ECacheLevel Cache )
{
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();
    if( Width < 1 || Height < 1 )
        return FOdysseyBlockProxy::MakeNullProxy();

    FString op = "Create_" + ID + "_" + FString::FromInt( Width ) + "_" + FString::FromInt( Height );

    if( BrushContext->KeyExistsInPool( Cache, op ) )
        return  BrushContext->RetrieveInPool( Cache, op );

    //---
    ::ul3::tFormat defaultFormat = BrushContext->GetState().target_temp_buffer ? BrushContext->GetState().target_temp_buffer->Format() : ULIS3_FORMAT_RGBA8;
	::ul3::tFormat format = ULISFormatFromOdysseyBlockFormat(Format, defaultFormat);

    FOdysseyBlock* tmp = new  FOdysseyBlock( Width, Height, format, nullptr, nullptr, InitializeData );

    FOdysseyBlockProxy prox( tmp, op );
    BrushContext->StoreInPool( Cache, op, prox );
    return prox;
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Blend( UOdysseyBrushAssetBase* BrushContext
                                        , FOdysseyBlockProxy Top
                                        , FOdysseyBlockProxy Back
                                        , int X
                                        , int Y
                                        , float Opacity
										, EOdysseyBlockFormat Format
                                        , EOdysseyBlendingMode BlendingMode
                                        , EOdysseyAlphaMode AlphaMode
                                        , ECacheLevel Cache)
{
    if( !BrushContext )
        return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Top.m )
        return  FOdysseyBlockProxy::MakeNullProxy();;
    if( !Back.m )
        return  FOdysseyBlockProxy::MakeNullProxy();;

    FString op = "Blend_" + Top.id + "_" + Back.id + "_" + FString::SanitizeFloat( Opacity ) + "_" + FString::FromInt( (int32)BlendingMode ) + "_" + FString::FromInt( (int32)AlphaMode );

    if( BrushContext->KeyExistsInPool( Cache, op ) )
        return  BrushContext->RetrieveInPool( Cache, op );

    //---
    ::ul3::tFormat defaultFormat = BrushContext->GetState().target_temp_buffer ? BrushContext->GetState().target_temp_buffer->Format() : ULIS3_FORMAT_RGBA8;
	::ul3::tFormat format = ULISFormatFromOdysseyBlockFormat(Format, defaultFormat);

    FOdysseyBlock* dst = new FOdysseyBlock(Back.m->GetBlock()->Width(), Back.m->GetBlock()->Height(), format, nullptr, nullptr, false );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Top.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* source = Top.m->GetBlock();
	::ul3::FBlock* back = Back.m->GetBlock();

	if (source->Format() != format)
	{
		::ul3::FBlock* conv = new ::ul3::FBlock(back->Width(), back->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, conv);
		source = conv;
	}

	if (back->Format() != format)
	{
		::ul3::FBlock* conv = new ::ul3::FBlock(back->Width(), back->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, conv);
		back = conv;
	}

    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, dst->GetBlock(), back->Rect(), ::ul3::FVec2I( 0, 0 ) );
    ::ul3::Blend( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, dst->GetBlock(), source->Rect(), ::ul3::FVec2F( X, Y ), ULIS3_AA, static_cast< ::ul3::eBlendingMode >( BlendingMode ), static_cast< ::ul3::eAlphaMode >( AlphaMode ), Opacity );

	if (Top.m->GetBlock() != source)
		delete source;

	if (Back.m->GetBlock() != back)
		delete back;

    FOdysseyBlockProxy prox( dst, op );
    BrushContext->StoreInPool( Cache, op, prox );
    return  prox;
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetWidth( FOdysseyBlockProxy Sample )
{
    if( !Sample.m )
        return 0;

    return Sample.m->Width();
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetHeight( FOdysseyBlockProxy Sample )
{
    if( !Sample.m )
        return 0;

    return Sample.m->Height();
}

//static
TArray< FOdysseyBlockProxy >
UOdysseyBlockProxyFunctionLibrary::GetFontBlocks( UOdysseyBrushAssetBase* iBrushContext, const UFont* iFont, EOdysseyBlockFormat Format, ECacheLevel iCache)
{
    TArray< FOdysseyBlockProxy > blocks;
    if( !iBrushContext )
        return blocks;
    if( iFont->FontCacheType == EFontCacheType::Runtime )
        return blocks;
    
    check( iFont->Textures.Num() )

    ::ul3::tFormat defaultFormat = iBrushContext->GetState().target_temp_buffer ? iBrushContext->GetState().target_temp_buffer->Format() : ULIS3_FORMAT_RGBA8;
	::ul3::tFormat format = ULISFormatFromOdysseyBlockFormat(Format, defaultFormat);
    
    for( auto texture : iFont->Textures )
    {
        //blocks.Add( Conv_TextureToOdysseyBlockProxy( texture, iBrushContext ) ); // It doesn't work as the cache only use texture name which can be the same for several texture

        FString op = "FontBlocks_" + iFont->GetName() + "_" + texture->GetName();

        if( iBrushContext->KeyExistsInPool( iCache, op ) )
        {
            blocks.Add( iBrushContext->RetrieveInPool( iCache, op ) );
            continue;
        }

        FOdysseyBlock* block = NewOdysseyBlockFromUTextureData( texture, format);
        FOdysseyBlockProxy prox( block, iFont->GetName() );
        iBrushContext->StoreInPool( iCache, op, prox );
        blocks.Add( prox );
    }
    
    return blocks;
}

//static
TArray< FOdysseyFontCharacter >
UOdysseyBlockProxyFunctionLibrary::GetFontCharacterInfo( const UFont* iFont, const FString& iString )
{
    TArray< FOdysseyFontCharacter > font_characters;
    if( iFont->FontCacheType == EFontCacheType::Runtime )
        return font_characters;
    
    check( iFont->Characters.Num() )
    
    for( auto character : iString )
    {
        TCHAR index_of_character = iFont->RemapChar( character );
    
        const FFontCharacter& font_char = iFont->Characters[index_of_character];

        FOdysseyFontCharacter font_character;
        font_character.StartU = font_char.StartU;
        font_character.StartV = font_char.StartV;
        font_character.USize = font_char.USize;
        font_character.VSize = font_char.VSize;
        font_character.TextureIndex = font_char.TextureIndex;
        font_character.VerticalOffset = font_char.VerticalOffset;

        font_characters.Add( font_character );
    }
    
    return font_characters;
}

//static
bool
UOdysseyBlockProxyFunctionLibrary::GetColorAtPosition( FOdysseyBlockProxy iBlock, float iX, float iY, FOdysseyBrushColor& oColor )
{
    int x = FMath::FloorToInt( iX );
    int y = FMath::FloorToInt( iY );

    FOdysseyBlock* block = iBlock.m;
    if( !block )
        return false;

    if( x < 0 || x >= block->Width() )
        return false;
    if( y < 0 || y >= block->Height() )
        return false;

    ::ul3::FPixelValue p = block->GetBlock()->PixelValue( x, y );

    FOdysseyBrushColor color( p );
    oColor = color;

    return true;
}
