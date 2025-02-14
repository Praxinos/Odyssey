// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <ULIS>
#include "OdysseyVectorObject.h"

#include "OdysseyVectorLayer.generated.h"

class IOdysseyVectorCell;
class FOdysseyVectorTag;

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
};

class ODYSSEYVECTOR_API FOdysseyVectorLayer : public FOdysseyVectorObject
{
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
        bool Contains( FOdysseyVectorCell* iCell );
        uint32 GetWidth();
        uint32 GetHeight();
        void InvalidateCell( FOdysseyVectorCell* iCell );
        virtual uint32 RemoveChild( FOdysseyVectorObject* iChild ) override;

    private:
        std::list<FOdysseyVectorCell*> mInvalidatedCellList;
        std::list<FOdysseyVectorObject*> mSharedObjectList;
        std::list<FOdysseyVectorTag*> mSharedTagList;
        std::mutex mSharedTagMutex;
        IOdysseyVectorLayer* mLayerInterface;
};
