// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Cel.h
* @author       Clement Berthaud
* @brief        This file provides the definition for a basic animation CEL.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
enum eCelExtensionBehaviour {
      CelExtensionBehaviour_None
    , CelExtensionBehaviour_Hold
    , CelExtensionBehaviour_FauxFixe
};

enum eCelFauxFixeMode {
      CelFauxFixeMode_Repeat
    , CelFauxFixeMode_PingPong
    , CelFauxFixeMode_Random
};

enum eCelExtensionExposureType {
      CelExtensionExposureType_Frames
    , CelExtensionExposureType_Instances
};

/////////////////////////////////////////////////////
/// @class      FCelFauxFixe
/// @brief      Faux fixe behaviour for Cel extension
class ULIS_API FCelFauxFixe {
public:
    FCelFauxFixe(
          eCelFauxFixeMode iMode = CelFauxFixeMode_Repeat
        , uint32 iExposure = ULIS_UINT32_MAX
        , eCelExtensionExposureType iType = CelExtensionExposureType_Frames
    );

public:
    bool operator==( const FCelFauxFixe& iOther ) const;
    eCelFauxFixeMode Mode() const;
    uint32 Exposure() const;
    eCelExtensionExposureType Type() const;
    void SetMode( eCelFauxFixeMode iValue );
    void SetExposure( uint32 iValue );
    void SetType( eCelExtensionExposureType iValue );

private:
    eCelFauxFixeMode mMode;
    uint32 mExposure;
    eCelExtensionExposureType mType;
};

/////////////////////////////////////////////////////
/// @class      FCelExtension
/// @brief      Extension behaviour for Cel Info
class ULIS_API FCelExtension {
public:
    FCelExtension(
          eCelExtensionBehaviour iBehaviour = CelExtensionBehaviour_None
        , const FCelFauxFixe& iFauxFixe = FCelFauxFixe()
    );

public:
    bool operator==( const FCelExtension& iOther ) const;
    eCelExtensionBehaviour Behaviour() const;
    const FCelFauxFixe& FauxFixe() const;
    void SetBehaviour( eCelExtensionBehaviour iValue );
    void SetFauxFixe( const FCelFauxFixe& iValue );

private:
    eCelExtensionBehaviour mBehaviour;
    FCelFauxFixe mFauxFixe;
};

/////////////////////////////////////////////////////
/// @class      FCelInfo
/// @brief      Basic Animation Cel Info
class ULIS_API FCelInfo {
public:
    FCelInfo(
          uint32 iExposure = 1
        , const FCelExtension& iPreBehaviour = FCelExtension()
        , const FCelExtension& iPostBehaviour = FCelExtension()
    );

    bool operator==( const FCelInfo& iOther ) const;
    uint32 Exposure() const;
    FCelExtension& PreBehaviour();
    const FCelExtension& PreBehaviour() const;
    FCelExtension& PostBehaviour();
    const FCelExtension& PostBehaviour() const;

private:
    uint32 mExposure;
    FCelExtension mPreBehaviour;
    FCelExtension mPostBehaviour;
};

/////////////////////////////////////////////////////
/// @class      TCel
/// @brief      Basic Animation Cel
template< class Type >
class TCel {

public:
    virtual ~TCel() {
        delete  mData;
    }

    template< class ... Args >
    TCel( const FCelInfo& iInfo = FCelInfo(), Args&& ... args )
        : mInfo( iInfo )
        , mData( new Type( std::forward< Args >( args ) ... ) )
    {}

    bool operator==( const TCel< Type >& iOther ) const {
        return  mInfo == iOther.mInfo && mData == iOther.mData;
    }

    Type& Data() {
        return  *mData;
    }

    const Type& Data() const {
        return  *mData;
    }

private:
    FCelInfo mInfo;
    Type* mData;
};

#define ULIS_EXPORT_CEL_CLASSES( CLASS )                \
    template class ULIS_API TCel< CLASS >;              \
    template class ULIS_API TArray< TCel< CLASS >* >;   \
    template class ULIS_API TSequence< CLASS >;

ULIS_NAMESPACE_END

