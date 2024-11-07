// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Matrix.h
* @author       Clement Berthaud
* @brief        This file provides a central point to include matrix types.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Matrix2.h"
#include "Math/Geometry/Matrix3.h"
#include "Math/Geometry/Matrix4.h"

ULIS_NAMESPACE_BEGIN
template class ULIS_API TMatrix2< float >;
template class ULIS_API TMatrix3< float >;
template class ULIS_API TMatrix4< float >;
ULIS_NAMESPACE_END

