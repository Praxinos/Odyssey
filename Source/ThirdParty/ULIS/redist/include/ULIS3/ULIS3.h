// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         ULIS3.h
* @author       Clement Berthaud
* @brief        This file provides a unique way to include all the necessary library files.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
// Data
#include "Data/Block.h"
#include "Data/Pixel.h"
#include "Data/Kernel.h"
#include "Data/StructuringElement.h"
// Base
#include "Base/HostDeviceInfo.h"
#include "Base/FilePathRegistry.h"
// Thread
#include "Thread/ThreadInterface.h"
// Maths
#include "Maths/Geometry.h"
#include "Maths/Maths.h"
#include "Maths/Transform2D.h"
#include "Maths/Bezier.h"
#include "Maths/CatmullRom.h"
// Interface
#include "Conv/Conv.h"
#include "Misc/Extract.h"
#include "Misc/Filter.h"
#include "Misc/GammaCompress.h"
#include "Misc/Premult.h"
#include "Misc/SummedAreaTable.h"
#include "Misc/Swap.h"
#include "Misc/Trim.h"
#include "Misc/Sanitize.h"
#include "Blend/Blend.h"
#include "Fill/Fill.h"
#include "Fill/FillPreserveAlpha.h"
#include "Clear/Clear.h"
#include "Copy/Copy.h"
#include "Text/FontRegistry.h"
#include "Text/FontEngine.h"
#include "Text/Font.h"
#include "Text/Text.h"
#include "IO/Disk.h"
#include "IO/Clipboard.h"
#include "Raster/Draw.h"
#include "Mix/Mix.h"
#include "Transform/Transform.h"
#include "Data/Sparse/TiledBlock.h"
#include "Data/Sparse/TilePool.h"

