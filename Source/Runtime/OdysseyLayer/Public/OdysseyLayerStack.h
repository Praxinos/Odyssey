// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <ULIS_FDECL>
#include <ULIS_BLENDINGMODES>

class FOdysseyBlock;
class IOdysseyLayer;
class FOdysseyImageLayer;


class ODYSSEYLAYER_API FOdysseyLayerStack
{
public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();
    FOdysseyLayerStack( int iWidth, int iHeight );

public:
    // Public API
    void               Init( int iWidth, int iHeight );
    void               InitFromData( FOdysseyBlock* iData );
    FOdysseyBlock*     GetResultBlock();
    void               ComputeResultBlock();
    void               ComputeResultBlock( const ::ULIS::FRect& iRect );
    void               ComputeResultBlockWithTempBuffer( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity = 1.f, ::ULIS::eBlendingMode iMode = ::ULIS::eBlendingMode::kNormal, ::ULIS::eAlphaMode iAlphaMode = ::ULIS::eAlphaMode::kNormal );
    void               BlendTempBufferOnCurrentBlock( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity = 1.f, ::ULIS::eBlendingMode iMode = ::ULIS::eBlendingMode::kNormal, ::ULIS::eAlphaMode iAlphaMode = ::ULIS::eAlphaMode::kNormal );
    int                Width() const;
    int                Height() const;
    FVector2D          Size() const;

public:
    // Public Array Tampon Methods
    FOdysseyImageLayer*                     AddLayer( int iAtIndex = -1 );
    FOdysseyImageLayer*                     AddLayerFromData( FOdysseyBlock* iData, FName iName = FName(), int iAtIndex = -1 );
    TArray< TSharedPtr< IOdysseyLayer > >*  GetLayers();

    void                                    DeleteLayer( IOdysseyLayer* ILayerToDelete );
    void                                    DeleteLayer( int iIndexLayerToDelete );
    void                                    MergeDownLayer( IOdysseyLayer* ILayerToMergeDown );
    void                                    DuplicateLayer( IOdysseyLayer* ILayerToDuplicate );

    int                                     GetCurrentLayerIndex() const;
    void                                    SetCurrentLayerIndex( int iIndex );
    TSharedPtr< IOdysseyLayer >             GetCurrentLayer() const;
    void                                    ClearCurrentLayer();
    void                                    FillCurrentLayerWithColor( const ::ULIS::CColor& iColor );

    TArray< TSharedPtr< FText > >           GetBlendingModesAsText();

private:
    // Private API
    FName                                   GetNextLayerName();
    void                                    InitResultAndTempBlock();

private:
    // Private Data Members
    FOdysseyBlock*                          mResultBlock;
    FOdysseyBlock*                          mTempBlock; // Temporary block for the blend of the tempBuffer into the image
    TArray< TSharedPtr< IOdysseyLayer > >   mLayers;
    int                                     mCurrentIndex;
    int                                     mWidth;
    int                                     mHeight;
    ETextureSourceFormat                    mTextureSourceFormat;
    bool                                    mIsInitialized;
};
