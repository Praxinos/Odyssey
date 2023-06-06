// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

class ODYSSEYIMAGING_API IOdysseyImageRenderer
{
public:
    enum class eRenderType
    {
        Render, //renders only the expected final render result
        Editor //renders what is expected in an editor (can render the animation lighttable for example)
    };

    virtual ~IOdysseyImageRenderer() {};
    IOdysseyImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects);

public:
    virtual TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) = 0;
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FEvent>& iWaitList);

    virtual TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) = 0;
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList);

    //TSharedPtr<::ULIS::FBlock> CopyInNewBlock(::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents);

protected:
    TArray<::ULIS::FEvent> ConvertAndBlend(TSharedPtr<::ULIS::FBlock> iFront, TSharedPtr<::ULIS::FBlock> iBack, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> ConvertAndCopy(TSharedPtr<::ULIS::FBlock> iSrc, TSharedPtr<::ULIS::FBlock> iDst, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Clear(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList);

public:
    IOdysseyImageRenderer::eRenderType GetRenderType() const;
    void AddHandle(TSharedPtr<IOdysseyHandle> iHandle);

private:
    IOdysseyImageRenderer::eRenderType mRenderType;
    TArray<TSharedPtr<IOdysseyHandle>> mHandles;
    TArray<::ULIS::FRectI> mDefaultRects;
};
