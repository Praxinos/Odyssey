// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Cel.h
* @author       Clement Berthaud
* @brief        This file provides the definition for a basic animation CEL.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/HasName.h"
#include "Layer/Components/HasPrettyColor.h"
#include "Layer/Components/HasExposure.h"
#include <memory>

ULIS_NAMESPACE_BEGIN
template< class T, class TFactory >
class TSequence;

/////////////////////////////////////////////////////
/// @class      TCel
/// @brief      Basic Animation Cel
template< class T >
class TCel
    : public IHasExposure
    , public IHasName
    , public IHasPrettyColor
{
public:
    virtual ~TCel() {
    }

    TCel(
          std::shared_ptr< T > iData
        , uint32 iExposure = 0
        , const FString& iName = ""
        , const FColor& iPrettyColor = FColor::Transparent

        , const FOn_uint32_Changed& iOnExposureChanged = FOn_uint32_Changed()
        , const FOnNameChanged& iOnNameChanged = FOnNameChanged()
        , const FOnColorChanged& iOnColorChanged = FOnColorChanged()
    )
        : IHasExposure( iExposure, iOnExposureChanged )
        , IHasName( iName, iOnNameChanged )
        , IHasPrettyColor( iPrettyColor, iOnColorChanged )
        , mData( iData )
    {}

    std::shared_ptr< T > Data() {
        return  mData;
    }

    const std::shared_ptr< T > Data() const {
        return  mData;
    }

    bool IsCelResourceShared() const {
        return  mData.use_count() > 1;
    }

    bool IsBlank() const {
        return  mData.get() == nullptr;
    }

private:
    std::shared_ptr< T > mData;
    // Exposure stored in IHasExposure can be zero but results in a 1-frame exposition anyway.
};

ULIS_NAMESPACE_END

