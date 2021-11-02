// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerTypedefs.h
* @author       Clement Berthaud
* @brief        This file provides the declaration of specific Layer Types
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Layer/Layer.h"
#include "Layer/Layer/LayerFolder.h"
#include "Layer/Layer/LayerImage.h"
#include "Layer/Layer/LayerStack.h"
#include "Layer/Layer/LayerText.h"
#include "Layer/Common/RegularBlockAllocator.h"
#include "Layer/Common/RegularBlockRenderer.h"

ULIS_NAMESPACE_BEGIN
// Typedefs
typedef TAbstractLayerDrawable< FBlock > ILayerDrawableBlock;
typedef TLayerStack< FBlock, void, FDummySuperStack > FLayerStack;
typedef TLayerFolder< FBlock, void, FRegularBlockRenderer, FRegularBlockAllocator, FLayerStack > FLayerFolder;
typedef TLayerImage< FBlock, void, FRegularBlockRenderer, FRegularBlockAllocator, FLayerStack > FLayerImage;
typedef TLayerText< FBlock, void, FRegularBlockRenderer, FRegularBlockAllocator, FLayerStack > FLayerText;

// Exports
//template class ULIS_API TLambdaCallback< void, const FBlock* >;
//template class ULIS_API TCallbackCapable< TLambdaCallback< void, const FBlock* > >;
//template class ULIS_API TLayerFolder< FBlock, void, void, void >;
//template class ULIS_API TLayerImage< FBlock, void, void, void, FLayerStack >;
//template class ULIS_API TLayerStack< FBlock, void, FDummySuperStack >;
//template class ULIS_API TLayerText< FBlock, void, void, void >;

ULIS_NAMESPACE_END

