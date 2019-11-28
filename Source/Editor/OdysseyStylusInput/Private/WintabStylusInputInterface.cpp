// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "WintabStylusInputInterface.h"

#if PLATFORM_WINDOWS

// Done in its own file

#else

TSharedPtr<IStylusInputInterfaceInternal>
CreateStylusInputInterfaceWintab()
{
    return TSharedPtr<IStylusInputInterfaceInternal>();
}

#endif
