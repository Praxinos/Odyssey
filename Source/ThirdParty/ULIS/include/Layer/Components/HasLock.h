// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasLock.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasLock* classes.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Common/LayerUtils.h" // For PIC Helper macros

ULIS_NAMESPACE_BEGIN
// We declare and define the following IHasLock* classes:
// IHasLock
// IHasPaintLock
// IHasTimeLock
// We use the ULIS_DECLARE_PIC_SIMPLE, because the implementation is trivial
// and redundant.
// This might prevent us from debugging the corresponding generated symbols,
// but they're so trivial it is deemed as unimportant.
// The expanded macros are classes, properly parsed by our doxygen documentation
// tool, that matters more to properly generate documented class diagrams.
// Note: since these are Pseudo-Interface-Composition ( PIC ) classes, that are
// meant to be inherited together at times, we need to make sure the getters and
// setters have different names, or they will shadow each other in the children
// classes client of this PIC API.
ULIS_DECLARE_PIC_SIMPLE( IHasLock, bool, false, IsLocked, SetLocked )
ULIS_DECLARE_PIC_SIMPLE( IHasPaintLock, bool, false, IsPaintLocked, SetPaintLocked )
ULIS_DECLARE_PIC_SIMPLE( IHasTimeLock, bool, false, IsTimeLocked, SetTimeLocked )
ULIS_NAMESPACE_END

