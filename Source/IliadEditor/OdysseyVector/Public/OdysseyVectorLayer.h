// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <ULIS>
#include "OdysseyVectorObject.h"

#include "OdysseyVectorLayer.generated.h"


class IOdysseyVectorHUD;
class IOdysseyVectorCell;
class FOdysseyVectorTag;
class UOdysseyPalette;

// Recommended for the unreal reflection system + Garbage collection
// (however it seems to work fine with IOdysseyVectorLayer declared only)
UINTERFACE(MinimalAPI, Blueprintable)
class UOdysseyVectorLayer : public UInterface
{
    GENERATED_BODY()
};

class IOdysseyVectorLayer
{
    GENERATED_BODY()

    public:
        virtual FOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) = 0;
        virtual FOdysseyVectorCell* GetLastCell() = 0;
        virtual FOdysseyVectorCell* GetFirstCell() = 0;
        virtual bool Contains( FOdysseyVectorCell* iCell ) = 0;
        virtual uint32 GetWidth() = 0;
        virtual uint32 GetHeight() = 0;
        virtual FOdysseyVectorCell* GetMaxCellFrom( uint32 iIndex ) = 0;
        virtual FOdysseyVectorCell* GetMinCellFrom( uint32 iIndex ) = 0;
        virtual FString GetPaletteSetID( UOdysseyPalette* iPalette ) = 0;
};

class ODYSSEYVECTOR_API FOdysseyVectorLayer : public FOdysseyVectorObject
{
    public:
        DECLARE_MULTICAST_DELEGATE_TwoParams( FNotifyDelegate, FOdysseyVectorLayer* iLayer, uint64 iDelegateFlags )

    private:
        static const uint32 mStaticClass = 0x442744a7; // value is crc32 FOdysseyVectorLayer

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FOdysseyVectorLayer();
        FOdysseyVectorLayer();
        FOdysseyVectorLayer( IOdysseyVectorLayer* iLayerInterface );

        void AddSharedTag( FOdysseyVectorTag* iVectorTag );
        void RemoveSharedTag( FOdysseyVectorTag* iVectorTag );
        void AddSharedObject( FOdysseyVectorObject* iVectorObject );
        void RemoveSharedObject( FOdysseyVectorObject* iVectorObject );
        bool HasSharedTag( FOdysseyVectorTag* iTag );
        FOdysseyVectorTag* GetSelectedTagByClassType( uint32 iClassType );
        void GetSelectedTagByClassType( uint32 iClassType
                                      , std::list<FOdysseyVectorTag*>& oSelectedTagList );

        std::list<FOdysseyVectorTag*>& GetSharedTagList();
        const std::list<FOdysseyVectorTag*>& GetSharedTagList() const;
        std::mutex& GetSharedTagMutex();
        virtual void InvalidateChild( FOdysseyVectorObject* iChild, uint64 iChildInvalidationFlags ) override;
        void RequestRedraw( FOdysseyVectorCell *iCell, uint64 iRedrawFlags );

        FOdysseyVectorCell* GetCellByIndex( uint32 iIndex );
        FOdysseyVectorCell* GetLastCell();
        FOdysseyVectorCell* GetFirstCell();
        FString GetPaletteSetID( UOdysseyPalette* iPalette );
        bool Contains( FOdysseyVectorCell* iCell );
        uint32 GetWidth();
        uint32 GetHeight();
        void InvalidateCell( FOdysseyVectorCell* iCell );
        virtual uint32 RemoveChild( FOdysseyVectorObject* iChild ) override;
        std::list<FOdysseyVectorCell*>& GetInvalidateCellList();
        FNotifyDelegate& OnNotifyDelegate();
        void Notify( uint64 iNotifyFlags );
        std::list<IOdysseyVectorHUD*>& GetHUDList();
        void AddHUD( IOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( IOdysseyVectorHUD* iHUDObject );
        void ClearHUD();
        void ResetHUD( FOdysseyVectorGroupPaint* iScene );

    private:
        std::list<FOdysseyVectorCell*> mInvalidatedCellList;
        std::list<FOdysseyVectorObject*> mSharedObjectList;
        std::list<FOdysseyVectorTag*> mSharedTagList;
        std::mutex mSharedTagMutex;
        IOdysseyVectorLayer* mLayerInterface;
        FNotifyDelegate mOnNotifyDelegate;
        std::list<IOdysseyVectorHUD*> mHUDList;
};
