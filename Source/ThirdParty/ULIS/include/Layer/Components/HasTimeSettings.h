// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasTimeSettings.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasTimeSettings class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/TimeSettings.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnTimeSettingsChanged, void, const FTimeSettings& )
/////////////////////////////////////////////////////
/// @class      IHasTimeSettings
/// @brief      Simple HasTimeSettings class.
class ULIS_API IHasTimeSettings
    : public FOnTimeSettingsChanged
{
protected:
    IHasTimeSettings(
          uint16 iFps = 24
        , int64 iBeginFrame = 0
        , int64 iEndFrame = 0
        , int64 iCurrentFrame = 0
        , const FOnTimeSettingsChanged& iDelegate = FOnTimeSettingsChanged()
    );

public:
    uint16 Fps();
    int64 BeginFrame();
    int64 EndFrame();
    int64 CurrentFrame();

    void SetFps( uint16 iValue );
    void SetBeginFrame( int64 iValue );
    void SetEndFrame( int64 iValue );
    void SetCurrentFrame( int64 iValue );

private:
    FTimeSettings mInfo;

};

ULIS_NAMESPACE_END

