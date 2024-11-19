// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyVectorRoot.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorRoot::FOdysseyVectorRoot( IOdysseyVectorLayer* iLayer
                                      , IOdysseyVectorCell* iCell
                                      , FOdysseyVectorGroupPaint* iScene
                                      , uint32 iPreferredWidth
                                      , uint32 iPreferredHeight )
    : FOdysseyVectorObject("ROOT")
    , mEngine ( iLayer, iCell, this, iPreferredWidth, iPreferredHeight )
    , mScene ( nullptr )
{
    bIsSystem = true;

    SetScene( iScene );
}

bool
FOdysseyVectorRoot::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

FOdysseyVectorGroupPaint*
FOdysseyVectorRoot::GetScene()
{
    return static_cast<FOdysseyVectorGroupPaint*>(mChildrenList.front());
}

FOdysseyVectorEngine*
FOdysseyVectorRoot::GetEngine()
{
    return &mEngine;
}

void
FOdysseyVectorRoot::Invalidate( uint64 iInvalidationFlags )
{
    FOdysseyVectorObject::Invalidate( iInvalidationFlags );

    mEngine.Invalidate( 0 );
}

void
FOdysseyVectorRoot::SetScene( FOdysseyVectorGroupPaint* iScene )
{
    if( mScene )
    {
        RemoveChild( mScene );
    }

    AppendChild( iScene );

    mEngine.ResetHUD();

    mScene = iScene;
    mScene->UpdateMatrix();
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}
