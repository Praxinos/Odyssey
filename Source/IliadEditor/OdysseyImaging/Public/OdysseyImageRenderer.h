// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyRenderingType.h"
#include <ULIS>

class IOdysseyRenderingAbility;
DECLARE_DELEGATE_RetVal_OneParam(bool, FImageRendererFilter, const IOdysseyRenderingAbility*);

struct FOdysseyImageRendererCopyParams
{
    FOdysseyImageRendererCopyParams(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<FIntRect>& iRects, ::ULIS::FVec2I iPos = ::ULIS::FVec2I(0))
        : mBlock(iBlock)
        , mRects(iRects)
        , mPos(iPos)
    {
    }

    TSharedPtr<::ULIS::FBlock> mBlock;
    TArray<FIntRect> mRects; //The rects to render, in destination block coordinates
    ::ULIS::FVec2I mPos; //The offset of the destination block compared to the full size block (used when partially rendering in a small block)

    ::ULIS::FMat3F mTransform; //Transform to apply to the pixels before rendering them in the destination block
};

struct FOdysseyImageRendererBlendParams : public FOdysseyImageRendererCopyParams
{
    FOdysseyImageRendererBlendParams(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<FIntRect>& iRects, ::ULIS::FVec2I iPos = ::ULIS::FVec2I(0))
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
    IOdysseyImageRenderer(EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects);

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
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) = 0;

protected:
    TArray<::ULIS::FEvent> ConvertAndBlend(TSharedPtr<::ULIS::FBlock> iFront, const ::ULIS::FVec2I& iFrontOffset, const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> ConvertAndCopy(TSharedPtr<::ULIS::FBlock> iSrc, const ::ULIS::FVec2I& iSrcOffset, const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> Clear(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<FIntRect>& iRects, const TArray<::ULIS::FEvent>& iWaitList);

public:
    EOdysseyRenderingType GetRenderType() const;

private:
    EOdysseyRenderingType mRenderType;
    TArray<FIntRect> mDefaultRects;
};
