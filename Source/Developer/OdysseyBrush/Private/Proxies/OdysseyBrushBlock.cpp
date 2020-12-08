// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Proxies/OdysseyBrushBlock.h"

#include "Engine/Font.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseySurfaceEditable.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth )
{
    if( !Texture )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);
    TSharedRef<FOdysseyBlock> dst = MakeShareable(NewOdysseyBlockFromUTextureData( Texture, format ));
    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::ConvertToFormat(FOdysseyBlockProxy Block, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

    ::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

    TSharedPtr<FOdysseyBlock> conv = MakeShareable(new FOdysseyBlock(Block.m->Width(), Block.m->Height(), format));
	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
    if (format == Block.m->Format())
    {
        ::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), conv->GetBlock(), Block.m->GetBlock()->Rect(), ::ul3::FVec2I(0, 0));
    }
	else
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), conv->GetBlock());
	}
    return FOdysseyBlockProxy(conv);
}

//static
EOdysseyColorModel
UOdysseyBlockProxyFunctionLibrary::GetColorModel(FOdysseyBlockProxy Block)
{
    if (!Block.m)
        return EOdysseyColorModel::kRGBA;

    return OdysseyColorModelFromULISFormat(Block.m->Format());
}

//static
EOdysseyChannelDepth
UOdysseyBlockProxyFunctionLibrary::GetChannelDepth(FOdysseyBlockProxy Block)
{
    if (!Block.m)
        return EOdysseyChannelDepth::k8;

    return OdysseyChannelDepthFromULISFormat(Block.m->Format());
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( FOdysseyBlockProxy Source
                                                    , FOdysseyBrushColor Color )
{
    if( !Source.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

	TSharedPtr<FOdysseyBlock> src = Source.m;
    ::ul3::tFormat format = src->Format();
    ::ul3::FPixelValue color = ::ul3::Conv( Color.GetValue(), format );

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new FOdysseyBlock(src->Width(), src->Height(), format));

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = src->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
    ::ul3::FillPreserveAlpha( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, dst->GetBlock()->Rect() );

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Fill( FOdysseyBlockProxy Block
                                        , FOdysseyBrushColor Color
                                        , FOdysseyBrushRect Area
										, bool PreserveAlpha
                                        , EOdysseyColorModel ColorModel
                                        , EOdysseyChannelDepth ChannelDepth)
{
    if( !Block.m )
        return FOdysseyBlockProxy::MakeNullProxy();

	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new FOdysseyBlock( Block.m->Width(), Block.m->Height(), format ));
    ::ul3::FPixelValue color = ::ul3::Conv( Color.GetValue(), format );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

    if( Area.IsInitialized() || PreserveAlpha)
    {
		if (Block.m->Format() != dst->Format())
		{
        	::ul3::Conv( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), dst->GetBlock() );
		}
		else
		{
			::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), dst->GetBlock(), Block.m->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
		}
    }

    if( !Area.IsInitialized() || Area.Width() > 0 || Area.Height() > 0 )
    {
		if (PreserveAlpha)
		{
        	::ul3::FillPreserveAlpha( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, Area.IsInitialized() ? Area.GetValue() : dst->GetBlock()->Rect());
		}
		else
		{
			::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, Area.IsInitialized() ? Area.GetValue() : dst->GetBlock()->Rect());
		}
    }
    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::BlendColor( FOdysseyBrushColor Color
                                        , FOdysseyBlockProxy Sample
										, FOdysseyBrushRect Area
                                        , float Opacity
										, EOdysseyColorModel ColorModel
                                        , EOdysseyChannelDepth ChannelDepth
                                        , EOdysseyBlendingMode BlendingMode
                                        , EOdysseyAlphaMode AlphaMode )
{
    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---
	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( Sample.m->GetBlock()->Width(), Sample.m->GetBlock()->Height(), format ));

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Sample.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	if (Sample.m->Format() != format)
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Sample.m->GetBlock(), dst->GetBlock());
	}
	else
	{
		::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Sample.m->GetBlock(), dst->GetBlock(), dst->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
	}

    if( !Area.IsInitialized() || (Area.Width() > 0 && Area.Height() > 0) )
    {
        ::ul3::BlendColor( hULIS.ThreadPool(), /* ULIS3_BLOCKING, */ perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Color.GetValue(), dst->GetBlock(), Area.IsInitialized() ? Area.GetValue() : dst->GetBlock()->Rect(), static_cast< ::ul3::eBlendingMode >( BlendingMode ), static_cast< ::ul3::eAlphaMode >( AlphaMode ), Opacity);
    }

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CreateBlock( int Width
                                              , int Height
                                              , EOdysseyColorModel ColorModel
                                              , EOdysseyChannelDepth ChannelDepth
                                              , bool InitializeData )
{
    if( Width < 1 || Height < 1 )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---
	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( Width, Height, format, nullptr, nullptr, InitializeData ));

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CropBlock( FOdysseyBlockProxy Block
                                         , FOdysseyBrushRect Area)
{
    if (!Block.m)
        return FOdysseyBlockProxy::MakeNullProxy();

	if (Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0))
		return FOdysseyBlockProxy::MakeNullProxy();

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	int w = Area.IsInitialized() ? Area.Width() : Block.m->Width();
	int h = Area.IsInitialized() ? Area.Height() : Block.m->Height();
    TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( w, h, Block.m->Format() ));
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), dst->GetBlock(), Area.IsInitialized() ? Area.GetValue() : Block.m->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Blend( FOdysseyBlockProxy Top
                                        , FOdysseyBlockProxy Back
                                        , FOdysseyBrushRect TopArea
                                        , int OffsetX
                                        , int OffsetY
                                        , float Opacity
										, EOdysseyColorModel ColorModel
                                        , EOdysseyChannelDepth ChannelDepth
                                        , EOdysseyBlendingMode BlendingMode
                                        , EOdysseyAlphaMode AlphaMode)
{
    if( !Top.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( !Back.m )
        return FOdysseyBlockProxy::MakeNullProxy();

	if (TopArea.IsInitialized() && (TopArea.Width() <= 0 || TopArea.Height() <= 0) )
		return FOdysseyBlockProxy::MakeNullProxy();

    //---
	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( Back.m->GetBlock()->Width(), Back.m->GetBlock()->Height(), format ));

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Top.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* source = Top.m->GetBlock();
	::ul3::FBlock* back = Back.m->GetBlock();

	if (TopArea.IsInitialized())
	{
		::ul3::FBlock* areaBlock = new ::ul3::FBlock(TopArea.Width(), TopArea.Height(), source->Format());
		::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, areaBlock, TopArea.GetValue(), ::ul3::FVec2I( 0, 0 ));
		source = areaBlock;	
	}

	if (source->Format() != format)
	{
		::ul3::FBlock* conv = new ::ul3::FBlock(source->Width(), source->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, conv);
		if (source != Top.m->GetBlock())
		{
			delete source;
		}
		source = conv;
	}

	if (back->Format() != format)
	{
		::ul3::FBlock* conv = new ::ul3::FBlock(back->Width(), back->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, conv);
		back = conv;
	}

    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, dst->GetBlock(), back->Rect(), ::ul3::FVec2I( 0, 0 ) );
    ::ul3::Blend( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, dst->GetBlock(), source->Rect(), ::ul3::FVec2F( OffsetX, OffsetY ), ULIS3_AA, static_cast< ::ul3::eBlendingMode >( BlendingMode ), static_cast< ::ul3::eAlphaMode >( AlphaMode ), Opacity );

	if (Top.m->GetBlock() != source)
		delete source;

	if (Back.m->GetBlock() != back)
		delete back;

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustAlpha( FOdysseyBlockProxy Block
                                                , UCurveFloat* Curve
                                                , bool PreserveNullAlpha)
{
    if (!Block.m)
        return FOdysseyBlockProxy::MakeNullProxy();

	if (!Curve)
	{
		Curve = NewObject<UCurveFloat>();
		Curve->FloatCurve.AddKey(0.0f, 0.0f);
		Curve->FloatCurve.AddKey(1.0f, 1.0f);
	}

    TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format() ));

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), dst->GetBlock(), Block.m->GetBlock()->Rect(), ::ul3::FVec2I(0, 0));

    ::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), dst->GetBlock(), [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
    {
        ::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

        if (PreserveNullAlpha && srcProxy.Alpha8() == 0.0f)
            dstProxy.SetAlpha8(0.0f);
        else
            dstProxy.SetAlpha8(Curve->GetFloatValue(srcProxy.Alpha8() / 255.f)*255);
	});

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustRGBA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveR
											, UCurveFloat* CurveG
											, UCurveFloat* CurveB
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve( NewObject<UCurveFloat>() ); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveR) CurveR = defaultCurve.Get();
	if (!CurveG) CurveG = defaultCurve.Get();
	if (!CurveB) CurveB = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kRGBA, channelDepth, ULIS3_FORMAT_RGBAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetRF(CurveR->GetFloatValue(srcProxy.RF()));
		dstProxy.SetGF(CurveG->GetFloatValue(srcProxy.GF()));
		dstProxy.SetBF(CurveB->GetFloatValue(srcProxy.BF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustGreyA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveGrey
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveGrey) CurveGrey = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kGreyA, channelDepth, ULIS3_FORMAT_GAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetGreyF(CurveGrey->GetFloatValue(srcProxy.GreyF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustHSVA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveH
											, UCurveFloat* CurveS
											, UCurveFloat* CurveV
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveH) CurveH = defaultCurve.Get();
	if (!CurveS) CurveS = defaultCurve.Get();
	if (!CurveV) CurveV = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kHSVA, channelDepth, ULIS3_FORMAT_HSVAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetHueF(CurveH->GetFloatValue(srcProxy.HueF()));
		dstProxy.SetSaturationF(CurveS->GetFloatValue(srcProxy.SaturationF()));
		dstProxy.SetValueF(CurveV->GetFloatValue(srcProxy.ValueF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustHSLA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveH
											, UCurveFloat* CurveS
											, UCurveFloat* CurveL
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveH) CurveH = defaultCurve.Get();
	if (!CurveS) CurveS = defaultCurve.Get();
	if (!CurveL) CurveL = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kHSLA, channelDepth, ULIS3_FORMAT_HSLAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetHueF(CurveH->GetFloatValue(srcProxy.HueF()));
		dstProxy.SetSaturationF(CurveS->GetFloatValue(srcProxy.SaturationF()));
		dstProxy.SetLightnessF(CurveL->GetFloatValue(srcProxy.LightnessF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustCMYKA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveC
											, UCurveFloat* CurveM
											, UCurveFloat* CurveY
											, UCurveFloat* CurveK
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveC) CurveC = defaultCurve.Get();
	if (!CurveM) CurveM = defaultCurve.Get();
	if (!CurveY) CurveY = defaultCurve.Get();
	if (!CurveK) CurveK = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kCMYKA, channelDepth, ULIS3_FORMAT_CMYKAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetCyanF(CurveC->GetFloatValue(srcProxy.CyanF()));
		dstProxy.SetMagentaF(CurveM->GetFloatValue(srcProxy.MagentaF()));
		dstProxy.SetYellowF(CurveY->GetFloatValue(srcProxy.YellowF()));
		dstProxy.SetKeyF(CurveK->GetFloatValue(srcProxy.KeyF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustLabA(FOdysseyBlockProxy Block
											, UCurveFloat* CurveL
											, UCurveFloat* CurveA
											, UCurveFloat* CurveB
											, UCurveFloat* CurveAlpha
											, bool PreserveNullAlpha)
{
	if (!Block.m)
		return FOdysseyBlockProxy::MakeNullProxy();

	TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
	defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
	defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

	if (!CurveL) CurveL = defaultCurve.Get();
	if (!CurveA) CurveA = defaultCurve.Get();
	if (!CurveB) CurveB = defaultCurve.Get();
	if (!CurveAlpha) CurveAlpha = defaultCurve.Get();

	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), Block.m->Format()));

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = Block.m->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::FBlock* src = Block.m->GetBlock();
	::ul3::FBlock* filterDst = dst->GetBlock();

	EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat(src->Format());
	::ul3::tFormat format = (EOdysseyColorModel::kLabA, channelDepth, ULIS3_FORMAT_LabAF);

	if (src->Format() != format)
	{
		src = new  ::ul3::FBlock(Block.m->GetBlock()->Width(), Block.m->GetBlock()->Height(), format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, Block.m->GetBlock(), src);
		filterDst = src;
	}

	::ul3::FilterInto(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src, filterDst, [&](const ::ul3::FBlock* iSrcBlock, const ::ul3::tByte* iSrcPtr, ::ul3::FBlock* iDstBlock, ::ul3::tByte* iDstPtr)
	{
		::ul3::FPixelProxy srcProxy(iSrcPtr, iSrcBlock->Format());
		::ul3::FPixelProxy dstProxy(iDstPtr, iDstBlock->Format());

		dstProxy.SetLF(CurveL->GetFloatValue(srcProxy.LF()));
		dstProxy.SetaF(CurveA->GetFloatValue(srcProxy.aF()));
		dstProxy.SetbF(CurveB->GetFloatValue(srcProxy.bF()));

		if (PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
			dstProxy.SetAlphaF(0.0f);
		else
			dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
	});

	if (filterDst != dst->GetBlock())
	{
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, filterDst, dst->GetBlock());
	}

	if (src != Block.m->GetBlock())
		delete src;

	return FOdysseyBlockProxy(dst);
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
UOdysseyBlockProxyFunctionLibrary::GetFontBlocks( const UFont* iFont, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
    TArray< FOdysseyBlockProxy > blocks;
    if( iFont->FontCacheType == EFontCacheType::Runtime )
        return blocks;
    
    check( iFont->Textures.Num() )

	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);
    
    for( auto texture : iFont->Textures )
    {
        TSharedPtr<FOdysseyBlock> block = MakeShareable(NewOdysseyBlockFromUTextureData( texture, format));
        FOdysseyBlockProxy prox( block );
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
void
UOdysseyBlockProxyFunctionLibrary::GetCharactersSize( const UFont* iFont, const FString& iString, TArray<float>& oWidth, TArray<float>& oHeight )
{
    oWidth.Empty();
    oHeight.Empty();

    for( auto character : iString )
    {
        float width;
        float height;
        iFont->GetCharSize( character, width, height );
        oWidth.Add( width );
        oHeight.Add( height );
    }
}

//static
bool
UOdysseyBlockProxyFunctionLibrary::GetColorAtPosition( FOdysseyBlockProxy iBlock, float iX, float iY, FOdysseyBrushColor& oColor )
{
    int x = FMath::FloorToInt( iX );
    int y = FMath::FloorToInt( iY );

	TSharedPtr<FOdysseyBlock> block = iBlock.m;
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

//static
FOdysseyBrushRect
UOdysseyBlockProxyFunctionLibrary::GetRect(FOdysseyBlockProxy Block)
{
	if (!Block.m)
		return FOdysseyBrushRect();

	return FOdysseyBrushRect(Block.m->GetBlock()->Rect());
}
