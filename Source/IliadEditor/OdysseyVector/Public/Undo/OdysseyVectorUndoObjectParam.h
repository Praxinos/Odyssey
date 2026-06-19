// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"



class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectParam : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectParam();
         FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                      , FOdysseyVectorObject* iObject
                                      , const FName& iCategoryName
                                      , const FName& iMemberPropertyName
                                      , const FName& iPropertyName );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                      , const std::vector<FOdysseyVectorObject*>& iObjectArray
                                      , const FName& iCategoryName
                                      , const FName& iMemberPropertyName
                                      , const FName& iPropertyName );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                      , const std::list<FOdysseyVectorObject*>& iObjectList
                                      , const FName& iCategoryName
                                      , const FName& iMemberPropertyName
                                      , const FName& iPropertyName );
        FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                     , FOdysseyVectorObject* iObject );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        FSnapshotObject* CreateObjectSnapshot( FOdysseyVectorObject* iObject
                                             , const FName& iCategoryName
                                             , const FName& iMemberPropertyName
                                             , const FName& iPropertyName );
        FSnapshotObject* CreateObjectSnapshot( FOdysseyVectorObject* iObject
                                             , uint64 iObjectParamFlags );

    protected:
        std::vector<FSnapshotObject*> mObjectSnapshotArray;
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectVisibility : public FOdysseyVectorUndoObjectParam
{
    public:
        ~FOdysseyVectorUndoObjectVisibility();
         FOdysseyVectorUndoObjectVisibility( FOdysseyVectorLayer* iLayer
                                           , FOdysseyVectorObject* iObject );
};
