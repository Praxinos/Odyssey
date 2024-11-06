// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

class FOdysseyImageRenderingAbility;
DECLARE_DELEGATE_RetVal_OneParam(bool, FImageRendererFilter, const FOdysseyImageRenderingAbility*);

struct FOdysseyImageRendererCopyParams
{
    FOdysseyImageRendererCopyParams(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects, ::ULIS::FVec2I iPos = ::ULIS::FVec2I(0))
        : mBlock(iBlock)
        , mRects(iRects)
        , mPos(iPos)
    {
    }

    TSharedPtr<::ULIS::FBlock> mBlock;
    TArray<::ULIS::FRectI> mRects; //The rects to render, in destination block coordinates
    ::ULIS::FVec2I mPos; //The offset of the destination block compared to the full size block (used when partially rendering in a small block)

    ::ULIS::FMat3F mTransform; //Transform to apply to the pixels before rendering them in the destination block
};

struct FOdysseyImageRendererBlendParams : public FOdysseyImageRendererCopyParams
{
    FOdysseyImageRendererBlendParams(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects, ::ULIS::FVec2I iPos = ::ULIS::FVec2I(0))
        : FOdysseyImageRendererCopyParams(iBlock, iRects, iPos)
    {
    }

    FOdysseyImageRendererBlendParams(const FOdysseyImageRendererCopyParams& iParams)
        : FOdysseyImageRendererCopyParams(iParams)
    {
    }

    ::ULIS::eBlendMode mBlendMode = ::ULIS::Blend_Normal;
    float mOpacity = 1.f;
};

class ODYSSEYIMAGING_API IOdysseyImageRenderer
{
public:
    enum class eRenderType
    {
        Render, //renders only the expected final render result
        RenderOutOfPegs,
        Editor, //renders what is expected in an editor (can render the animation lighttable for example)
    };

    virtual ~IOdysseyImageRenderer() {};
    IOdysseyImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects);

public:
    /**
     * @brief Initializes the renderer to load everything it needs prior to any rendering (like image blocks)
     */
    virtual void Init() = 0;

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    virtual bool IsGameThreadOnly() = 0;

    /**
     * @brief Locks any data that should avoid changing while Init() or any rendering.
     * Should be called before calling Init().
     */
    virtual void Lock();

    /**
     * @brief Unlocks any data previously locked by Lock()
     */
    virtual void Unlock();

public:

    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) = 0;
    /* TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FEvent>& iWaitList); */

    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) = 0;
    /* TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList); */

    //TSharedPtr<::ULIS::FBlock> CopyInNewBlock(::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents);

protected:
    TArray<::ULIS::FEvent> ConvertAndBlend(TSharedPtr<::ULIS::FBlock> iFront, const ::ULIS::FVec2I& iFrontOffset, const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> ConvertAndCopy(TSharedPtr<::ULIS::FBlock> iSrc, const ::ULIS::FVec2I& iSrcOffset, const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Clear(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);

public:
    IOdysseyImageRenderer::eRenderType GetRenderType() const;

private:
    IOdysseyImageRenderer::eRenderType mRenderType;
    TArray<::ULIS::FRectI> mDefaultRects;
};
