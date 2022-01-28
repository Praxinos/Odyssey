// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasVisibility.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasVisibility class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Common/LayerUtils.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_PIC_SIMPLE( IHasVisibility, bool, true, IsVisible, SetVisible )
ULIS_NAMESPACE_END

