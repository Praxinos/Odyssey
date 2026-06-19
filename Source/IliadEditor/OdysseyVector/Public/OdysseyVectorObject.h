// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorBucket.h"
#include <bitset>

class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorGroup;
class FOdysseyVectorTag;
class FOdysseyVectorLayer;
class FOdysseyVectorCell;

struct FOdysseyVectorObjectInvalidationFlags
{
    static const uint32 DEFAULT                                 =  0;
    static const uint32 OBJECT_SELECTION                        =  1;
    static const uint32 SHAPE                                   =  2;
    static const uint32 COLOR                                   =  3;
    static const uint32 TOPOLOGY                                =  4;
    static const uint32 TAG_LIST                                =  5;
    static const uint32 MATRIX                                  =  6;
    static const uint32 HIERARCHY                               =  7;
    static const uint32 PARAM                                   =  8;
    static const uint32 TAG_INBETWEENER_SHAPE                   =  9;
    static const uint32 TAG_INBETWEENER_MATRIX                  = 10;
    static const uint32 TAG_INBETWEENER_MAP                     = 11;
    static const uint32 TAG_INBETWEENER_BUFFERS                 = 12;
    static const uint32 TAG_INBETWEENER_SPACING                 = 13;
    static const uint32 TAG_INBETWEENER_GRIDTYPE                = 14;
    static const uint32 TAG_INBETWEENER_INTERPOLATIONTYPE       = 15;
    static const uint32 TAG_INBETWEENER_CELLS                   = 16;
    static const uint32 TAG_INBETWEENER_ROUTES                  = 17;
    static const uint32 TAG_INBETWEENER_BREAKDOWN_LIST          = 18;
    static const uint32 TAG_INBETWEENER_CHARTHUD                = 19;
    static const uint32 FLAG_COUNT                              = 20;
    static const uint32 CHILD_DEFAULT                           = ( FLAG_COUNT + DEFAULT );
    static const uint32 CHILD_OBJECT_SELECTION                  = ( FLAG_COUNT + OBJECT_SELECTION );
    static const uint32 CHILD_SHAPE                             = ( FLAG_COUNT + SHAPE );
    static const uint32 CHILD_COLOR                             = ( FLAG_COUNT + COLOR );
    static const uint32 CHILD_TOPOLOGY                          = ( FLAG_COUNT + TOPOLOGY );
    static const uint32 CHILD_TAG_LIST                          = ( FLAG_COUNT + TAG_LIST );
    static const uint32 CHILD_MATRIX                            = ( FLAG_COUNT + MATRIX );
    static const uint32 CHILD_HIERARCHY                         = ( FLAG_COUNT + HIERARCHY );
    static const uint32 CHILD_PARAM                             = ( FLAG_COUNT + PARAM );
    static const uint32 CHILD_TAG_INBETWEENER_SHAPE             = ( FLAG_COUNT + TAG_INBETWEENER_SHAPE );
    static const uint32 CHILD_TAG_INBETWEENER_MATRIX            = ( FLAG_COUNT + TAG_INBETWEENER_MATRIX );
    static const uint32 CHILD_TAG_INBETWEENER_MAP               = ( FLAG_COUNT + TAG_INBETWEENER_MAP );
    static const uint32 CHILD_TAG_INBETWEENER_BUFFERS           = ( FLAG_COUNT + TAG_INBETWEENER_BUFFERS );
    static const uint32 CHILD_TAG_INBETWEENER_SPACING           = ( FLAG_COUNT + TAG_INBETWEENER_SPACING );
    static const uint32 CHILD_TAG_INBETWEENER_GRIDTYPE          = ( FLAG_COUNT + TAG_INBETWEENER_GRIDTYPE );
    static const uint32 CHILD_TAG_INBETWEENER_INTERPOLATIONTYPE = ( FLAG_COUNT + TAG_INBETWEENER_INTERPOLATIONTYPE );
    static const uint32 CHILD_TAG_INBETWEENER_CELLS             = ( FLAG_COUNT + TAG_INBETWEENER_CELLS );
    static const uint32 CHILD_TAG_INBETWEENER_ROUTES            = ( FLAG_COUNT + TAG_INBETWEENER_ROUTES );
    static const uint32 CHILD_TAG_INBETWEENER_BREAKDOWN_LIST    = ( FLAG_COUNT + TAG_INBETWEENER_BREAKDOWN_LIST );
    static const uint32 CHILD_TAG_INBETWEENER_CHARTHUD          = ( FLAG_COUNT + TAG_INBETWEENER_CHARTHUD );


    FOdysseyVectorObjectInvalidationFlags& Set()
    {
        bits.set();

        return *this;
    }

    FOdysseyVectorObjectInvalidationFlags& Set( uint32 iFlag )
    {
        bits[iFlag] = 1;

        return *this;
    }

    public:
        std::bitset<FLAG_COUNT*2> bits;
};


class ODYSSEYVECTOR_API FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x84cd3d16; // value is crc32 FOdysseyVectorObject

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

        static const uint32 PICK_MATH_BASED = ( 1 << 0 );
        static const uint32 PICK_MASK_BASED = ( 1 << 3 );

        // values for hierarchy change (AddChild, RemoveChild, TransferChild)
        static const uint32 HIERARCHY_CHANGE_SUCCESS   = 0;
        static const uint32 HIERARCHY_CHANGE_FORBIDDEN = 1;
        static const uint32 HIERARCHY_CHANGE_ERROR     = 2;

        // copy flags
        static const uint32 COPY_RETOPOLOGY          = ( 1 << 0 );
        static const uint32 COPY_NOTAG               = ( 1 << 2 );

        // traversal flags
        static const uint64 TRAVERSE_CONTINUE               = ( 0 );
        static const uint64 TRAVERSE_STOP                   = ( 1 << 0 );
        static const uint64 TRAVERSE_OBJECT_ACCEPTED        = ( 1 << 1 );
        static const uint64 TRAVERSE_PARENT_HASFOCUS        = ( 1 << 2 );
        static const uint64 TRAVERSE_OBJECT_IGNORE_CHILDREN = ( 1 << 3 );

        // update flags
        //static const uint32 FREQUENTUPDATES = ( 1 << 0 );
        //static const uint32 UPDATE_KEEPINVALIDATED   = ( 1 << 1 );
        static const uint32 UPDATE_PAINTGROUPS      = ( 1 <<  2 );
        static const uint32 UPDATE_FROMFILE         = ( 1 <<  3 );
        static const uint32 UPDATE_INTERACTIVE      = ( 1 <<  4 );
        static const uint32 UPDATE_NEEDPOLYLINE     = ( 1 <<  5 );
        static const uint32 UPDATE_NOINBETWEENING   = ( 1 <<  6 );
        static const uint32 UPDATE_FORCE            = ( 1 <<  7 ); // request force updating everything, not only invalidated items
        static const uint32 UPDATE_NODRAWINGLOCK    = ( 1 <<  8 );
        static const uint32 UPDATE_NOINVALIDATERECT = ( 1 << 10 );

        // invalidation flags
        static const uint64 INVALIDATE_DEFAULT        = ( 1ULL << 0 );
        static const uint64 INVALIDATE_MATRIX         = ( 1ULL << 1 );
        static const uint64 INVALIDATE_HIERARCHY      = ( 1ULL << 2 );
        static const uint64 INVALIDATE_SHAPE          = ( 1ULL << 3 );
        static const uint64 INVALIDATE_COLOR          = ( 1ULL << 4 );
        static const uint64 INVALIDATE_TOPOLOGY       = ( 1ULL << 5 );
        static const uint64 INVALIDATE_TAG            = ( 1ULL << 6 );
        static const uint64 INVALIDATE_TAG_LIST       = ( 1ULL << 7 );
        static const uint64 INVALIDATE_CHILD_SHIFT    = 15;
        static const uint64 INVALIDATE_CHILD_SHAPE    = ( INVALIDATE_SHAPE    << INVALIDATE_CHILD_SHIFT );
        static const uint64 INVALIDATE_CHILD_COLOR    = ( INVALIDATE_COLOR    << INVALIDATE_CHILD_SHIFT );
        static const uint64 INVALIDATE_CHILD_TOPOLOGY = ( INVALIDATE_TOPOLOGY << INVALIDATE_CHILD_SHIFT );
        static const uint64 INVALIDATE_CHILD_TAG      = ( INVALIDATE_TAG      << INVALIDATE_CHILD_SHIFT );
        static const uint64 INVALIDATE_CHILD_TAG_LIST = ( INVALIDATE_TAG_LIST << INVALIDATE_CHILD_SHIFT );
        static const uint64 INVALIDATE_CHILD_MATRIX   = ( INVALIDATE_MATRIX   << INVALIDATE_CHILD_SHIFT );

        static const uint8 FOREGROUNDCOLOR_DEFAULT_R = 0;
        static const uint8 FOREGROUNDCOLOR_DEFAULT_G = 0;
        static const uint8 FOREGROUNDCOLOR_DEFAULT_B = 0;
        static const uint8 FOREGROUNDCOLOR_DEFAULT_A = 255;

        static const uint8 BACKGROUNDCOLOR_DEFAULT_R = 0;
        static const uint8 BACKGROUNDCOLOR_DEFAULT_G = 0;
        static const uint8 BACKGROUNDCOLOR_DEFAULT_B = 0;
        static const uint8 BACKGROUNDCOLOR_DEFAULT_A = 0;

    public:
        static uint32 TreeToList( FOdysseyVectorObject* iObject, std::list<FOdysseyVectorObject*>& iOutList );
        static uint32 TreeToArray( FOdysseyVectorObject* iObject, std::vector<FOdysseyVectorObject*>& iOutArray );
        static ::ULIS::FRectD GetBoundingBoxFromList( std::list<FOdysseyVectorObject*>& iObjectList );
        static uint32 GetCommonClass( std::list<FOdysseyVectorObject*>& iObjectList );

        virtual ~FOdysseyVectorObject();
        FOdysseyVectorObject( const FString& iName );
        void SetName( const FString& iName );
        void CopySettings( FOdysseyVectorObject* iDestinationObject, uint64 iCopyFlags );

        /**
         * @brief Add a child object.
         * @param iChild the child object.
         * @param iInsertAfter the object the new child object is inserted after. Can be nullptr.
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        virtual uint32 AddChild( FOdysseyVectorObject* iChild
                               , FOdysseyVectorObject* iInsertAfter );

        /**
         * @brief Add a child object at the end of the list.
         * @param iChild the child object.
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        uint32 AppendChild( FOdysseyVectorObject* iChild );

        /**
         * @brief bring the object forward (in the list of children it belongs to).
         */
        void BringForward();

        /**
         * @brief Recursively copy the object
         * @return a copy of the object with copied children as well.
         */
        FOdysseyVectorObject* Copy();

        FOdysseyVectorObject* Copy( uint64 iCopyFlags
                                  , std::function<uint64(FOdysseyVectorObject*,uint64)> iPreCallback
                                  , std::function<uint64(FOdysseyVectorObject*
                                                       , FOdysseyVectorObject*,uint64)> iPostCallback );

        void RecursiveRemoveTagByType( uint32 iTagType
                                     , std::list<FOdysseyVectorTag*>& oRemovedTagList );

        /**
         * @brief Copy transformation to destination object passed as parameter.
                  Please call UpdateMatrix() afterwards.
         * @param iDestinationObject a reference to the object that will receive transformation values.
         */
        void CopyTransformation( FOdysseyVectorObject& iDestinationObject );

        /**
         * @brief Draw the object to the Blend2D context passed as parameter
         * @param iBLContext The Blend2D context to draw to
         * @param iInvalidationArea
         * @param iAncestorsOpacity The cumulated opacity from parent objects
         * @param iDrawingFlags drawing flags
         */
        virtual void Draw( BLContext* iBLContext
                         , FOdysseyVectorEngine* iEngine
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags );

        /**
         * @brief Draw children objects
         * @param iBLContext The Blend2D context to draw to
         * @param iInvalidationArea
         * @param iCombinedOpacity The cumulated opacity from parent objects
         * @param iDrawingFlags drawing flags
         */
        virtual void DrawChildren( BLContext* iBLContext
                                 , FOdysseyVectorEngine* iEngine
                                 , const ::ULIS::FRectD& iInvalidationArea
                                 , double iCombinedOpacity
                                 , uint64 iDrawingFlags );

        /**
         * @brief Export object's param (color, opacity) to the destination object.
         * @param iDestinationObject
         * @param iInvalidate true or false.
         */
        virtual void ExportParam( FOdysseyVectorObject* iDestinationObject
                                , bool iInvalidate );

        /**
         * @brief Get the background bucket.
         * @return a reference to the background bucket.
         */
        FOdysseyVectorBucket& GetBackgroundBucket();

        /**
         * @brief Get the background color, i.e the solid color or the palettized color.
         * @return the color as a FColor
         */
        FColor GetBackgroundColor();

        /**
         * @brief Set the object's bounding box in local or world coordinates.
         * @param iWorld Local or world coordinates.
         * @return the bounding box.
         */
        ::ULIS::FRectD GetBBox( bool iInDepth, bool iWorld );

        /**
         * @brief Get the list of children objects.
         * @return a reference to the list of children objects.
         */
        std::list<FOdysseyVectorObject*>& GetChildrenList();

        /**
         * @brief Get the foreground bucket.
         * @return a reference to the foreground bucket.
         */
        FOdysseyVectorBucket& GetForegroundBucket();

        /**
         * @brief Get the foreground color, i.e the solid color or the palettized color.
         * @return the color as a FColor
         */
        FColor GetForegroundColor();

        /**
         * @brief Get the object's ID. Note, this ID is never guaranted to be consistent
         *        and is there only for being used as temporary index value when working
         *        with arrays for example.
         * @return the object's ID.
         */
        uint32 GetID();

        /**
         * @brief Get the inverse local matrix
         * @return a reference to the inverse local matrix.
         */
        BLMatrix2D& GetInverseLocalMatrix();

        /**
         * @brief Get the inverse world matrix
         * @return a reference to the inverse world matrix.
         */
        BLMatrix2D& GetInverseWorldMatrix();

        /**
         * @brief Get the object's last child in the list.
         * @return a pointer to the last child, nullptr if none.
         */
        FOdysseyVectorObject* GetLastChild();

        /**
         * @brief Get the local matrix
         * @return a reference to the local matrix.
         */
        BLMatrix2D& GetLocalMatrix();

        /**
         * @brief Get the object's name
         * @return a reference to the object's name.
         */
        FString& GetName();

        /**
         * @brief Get the object's opacity.
         * @return the object's opacity.
         */
        double GetOpacity();

        /**
         * @brief Get the object's parent.
         * @return a pointer to this object's parent.
         */
        FOdysseyVectorObject* GetParent();

        /**
         * @brief Get the object's former parent.
         * @return a pointer to this object's former parent.
         */
        FOdysseyVectorObject* GetOldParent();

        /**
         * @brief Get the child object that is before the one passed as a parameter.
         * @return a pointer to that child.
         */
        FOdysseyVectorObject* GetPreviousChild( FOdysseyVectorObject* iChild );

        /**
         * @brief Retrieve Translation/Rotation/Scaling values
         * @param oTranslationX a reference to store the translation value on the x-axis
         * @param oTranslationY a reference to store the translation value on the y-axis
         * @param oRotation a reference to store the rotation value (in degrees)
         * @param oScalingX a reference to store the scaling value on the x-axis
         * @param oScalingY a reference to store the scaling value on the y-axis
         */
        void GetTransform( double& oTranslationX
                         , double& oTranslationY
                         , double& oRotation
                         , double& oScalingX
                         , double& oScalingY
                         , double& oSkewX
                         , double& oSkewY );

        /**
         * @brief Get the object's translation value on x-axis.
         * @return the translation value on x-axis.
         */
        double GetTranslationX();

        /**
         * @brief Get the object's translation value on y-axis.
         * @return the translation value on y-axis.
         */
        double GetTranslationY();

        /**
         * @brief Get the object's rotation value.
         * @return the rotation.
         */
        double GetRotation();

        /**
         * @brief Get the object's scaling value on x-axis.
         * @return the scaling value on x-axis.
         */
        double GetScalingX();

        /**
         * @brief Get the object's scaling value on y-axis.
         * @return the scaling value on y-axis.
         */
        double GetScalingY();

        /**
         * @brief Get the object's skew value on x-axis.
         * @return the skew value on x-axis.
         */
        double GetSkewX();

        /**
         * @brief Get the object's skew value on y-axis.
         * @return the skew value on y-axis.
         */
        double GetSkewY();

        /**
         * @brief Retrieve the root object, i.e the top-most displayable object.
         * @return a pointer to the root object.
         */
        FOdysseyVectorGroupPaint* GetScene();

        /**
         * @brief Get the world matrix
         * @return a reference to the world matrix.
         */
        BLMatrix2D& GetWorldMatrix();

        /**
         * @brief check if the object passed as parameter is an ancestor of this object.
         * @return true or false
         */
        bool HasAncestor( FOdysseyVectorObject* iCandidateAncestor );

        /**
         * @brief Checks if the child passed as a parameter
         *        belongs to this object by iterating the children list.
         * @param iChild the child to check.
         * @return true or false.
         */
        bool HasChild( FOdysseyVectorObject* iChild );

        /**
         * @brief Invalidates the object and its ancestor objects as well
         * @param iInvalidationFlags
         */
        virtual void Invalidate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags );

        /**
         * @brief Get the expansion status
         * @return true or false
         */
        bool IsExpanded();

        /**
         * @brief Get the invalidation status.
         * @return  If any flag is set, it will return true, false otherwise.
         */
        bool IsInvalidated();

        /**
         * @brief Get the selection status, based on the selection flag.
         * @return true or false
         */
        bool IsSelected();

        /**
         * @brief Add a child object at the beggining of the list.
         * @param iChild the child object.
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        uint32 PrependChild( FOdysseyVectorObject* iChild );

        /**
         * @brief Remove a child object. Note: The object is not freed.
         * @param iChild the child to remove.
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        virtual uint32 RemoveChild( FOdysseyVectorObject* iChild );

        /**
         * @brief Removes all children objects. Note: The objects are not freed.
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        virtual uint32 RemoveAllChildren();

        /**
         * @brief Resets transformation to identity matrix.
         */
        void ResetTransform();

        /**
         * @brief Set values for rotation. A call to UpdateMatrix() is needed after that.
         * @param iAngle
         */
        void Rotate( double iAngle );

        /**
         * @brief Set values for scaling. A call to UpdateMatrix() is needed after that.
         * @param iX
         * @param iY
         */
        void Scale( double iX, double iY );

        /**
         * @brief Send the object backward (in the list of children it belongs to).
         */
        void SendBackward();

        /**
         * @brief Set the background bucket from the one passed as parameter
         * @param iBucket the bucket to copy values from.
         */
        void SetBackgroundBucket( const FOdysseyVectorBucket& iBucket );

        /**
         * @brief Set the background solid color
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
         */
        void SetBackgroundSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Set the background solid color
         * @param iColor
         */
        void SetBackgroundSolidColor( FColor& iColor );

        /**
         * @brief Set the expansion flag (used by the hierarchy widget to display children or not)
         * @param iIsExpanded true or false
         */
        void SetExpanded( bool iIsExpanded );

        /**
         * @brief Set the foreground bucket from the one passed as parameter
         * @param iBucket the bucket to copy values from.
         */
        void SetForegroundBucket( const FOdysseyVectorBucket& iBucket );

        /**
         * @brief Set the foreground solid color
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
         */
        void SetForegroundSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Set the foreground solid color
         * @param iColor
         */
        void SetForegroundSolidColor( FColor& iColor );

        /**
         * @brief Sets the object's ID. Note, this ID is never guaranted to be consistent
         *        and is there only for being used as temporary index value when working
         *        with arrays for example.
         * @param iID the desired ID.
         */
        void SetID( uint32 iID );

        /**
         * @brief Set object's opacity.
         * @param iOpacity the desired opacity.
         */
        void SetOpacity( double iOpacity );

        /**
         * @brief Set the object's parent. Note, this function does not technically add the object
         * to the parent's list of children. It only sets the member variable for convenience reasons.
         * Use AddChild, AppendChild or PrependChild if you want to alter the hierarchy.
         * @param iObject a pointer to the desired parent.
         */
        void SetParent( FOdysseyVectorObject* iObject );

        /**
         * @brief Set the selection flag. It DOES NOT actually select the object, only sets the flag.
         * @param iIsSelected the selection status, true or false.
         */
        void SetSelected( bool iIsSelected );

        /**
         * @brief Set Translation/Rotation/Scaling values
         * @param iTranslationX The translation value on the x-axis
         * @param iTranslationY The translation value on the y-axis
         * @param iRotation The rotation value (in degrees)
         * @param iScalingX The scaling value on the x-axis
         * @param iScalingY The scaling value on the y-axis
         */
        void SetTransform( double iTranslationX
                         , double iTranslationY
                         , double iRotation
                         , double iScalingX
                         , double iScalingY
                         , double iSkewX
                         , double iSkewY );

        /**
         * @brief Set values for skew. A call to UpdateMatrix() is needed after that.
         * @param iX
         * @param iY
         */
        void Skew( double iX, double iY );

        /**
         * @brief Transfer object to the coordinates system defined by the World matrix passed as parameter
         * @iMatrix The matrix representing the world coordinates system to put the object into
         */
        void Transfer( const BLMatrix2D& iMatrix );

        /**
         * @brief Transfer an object from its current parent to this object set as a parent.
         * @param iUpdateFlags
         * @param iUpdateFlags
         * @return Hierarchy status flags (for failure, success or prohibition. Cf flags)
         */
        virtual uint32 TransferChild( FOdysseyVectorObject* iFosterChild
                                    , FOdysseyVectorObject* iInsertAfter );

        /**
         * @brief Set values for translation. A call to UpdateMatrix() is needed after that.
         * @param iX
         * @param iY
         */
        void Translate( double iX, double iY );

        /**
         * @brief Recursively updates the object's matrix (world and local)
         */
        virtual void UpdateMatrix();

        /**
         * @brief Update the object after it was invalidated
         * @param iUpdateFlags
         */
        virtual void Update( uint32 iUpdateFlags );

        virtual void ApplyTransformations();
        virtual void ApplyMatrix( BLMatrix2D& iMatrix );
        void AddTag( FOdysseyVectorTag* iTag );
        void RemoveTag( FOdysseyVectorTag* iTag );
        void DrawTags( BLContext* iBLContext
                     , FOdysseyVectorEngine* iEngine
                     , const ::ULIS::FRectD& iInvalidationArea
                     , double iCombinedOpacity
                     , uint64 iFlags );
        FOdysseyVectorTag* GetTagByType( uint32 iTagClass );

        std::list<FOdysseyVectorTag*>& GetTagList();
        FOdysseyVectorLayer* GetLayer();
        FOdysseyVectorObjectInvalidationFlags GetInvalidationFlags();
        FOdysseyVectorCell* GetCell();
        virtual void Added();
        virtual void Removed();
        bool IsSystem();
        FOdysseyVectorObject* GetAncestorByClass( uint32 iClass, bool iBaseClass, bool iSelf );
        void LockDrawing();
        void UnlockDrawing();

        static void FlipObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                               , double iXFactor
                               , double iYFactor );

        static void FlipObjectsHorizontal( const std::list<FOdysseyVectorObject*>& iObjectList );

        static void FlipObjectsVertical( const std::list<FOdysseyVectorObject*>& iObjectList );


        static FOdysseyVectorGroup* GroupObjects( FOdysseyVectorObject* iParent
                                                , const std::list<FOdysseyVectorObject*>& iObjectList
                                                , std::vector<FOdysseyVectorObject*>& oObjectArray );

        static FOdysseyVectorGroupPaint* MakePaintGroupFromObjects( FOdysseyVectorObject* iParent
                                                                  , const std::list<FOdysseyVectorObject*>& iObjectList
                                                                  , std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                                  , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray );

        static ::ULIS::FVec2D GetPositionFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList );
        static uint64 Traverse( FOdysseyVectorObject* iObject
                              , uint64 iTraversalFlags
                              , std::function<uint64(FOdysseyVectorObject*,uint64)> iCallback );
        bool IsVisible( bool iHierarchical );
        void SetVisible( bool iVisible );
        void InvalidateTree( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags );
        virtual FColor GetHUDColor();
        static void GetBucketsFromPaletteEntryRecursively( FOdysseyVectorObject* iObject
                                                         , UOdysseyPaletteEntry* iPaletteEntry
                                                         , TArray<FOdysseyVectorBucket*>& oBucketArray );
        static void GetBucketsFromPaletteEntryRecursively( FOdysseyVectorObject* iObject
                                                         , UOdysseyPaletteEntry* iPaletteEntry
                                                         , std::list<FOdysseyVectorBucket*>& oBucketList );
        FOdysseyVectorObject* Pick( FOdysseyVectorGroup* iSelectionSpace
                                  , const ::ULIS::FRectD &iRoi
                                  , const BLImage& iMaskImage );
        FOdysseyVectorObject* GetAncestorByClassAndTag( uint32 iObjectClass
                                                      , bool iAsBaseObjectClass
                                                      , uint32 iTagClass
                                                      , bool iSelf );

    protected:

        virtual void UpdateShape( uint32 iUpdateFlags );
        virtual FOdysseyVectorObject* CopyShape( uint64 iCopyFlags );
        virtual void DrawShape ( BLContext* iBLContext
                               , FOdysseyVectorEngine* iEngine
                               , const ::ULIS::FRectD& iInvalidationArea
                               , double iCombinedOpacity
                               , uint64 iFlags ){};
        virtual void InvalidateChild( FOdysseyVectorObject* iChild
                                    , const FOdysseyVectorObjectInvalidationFlags& iChildInvalidationFlags );
        void Recurse( void (FOdysseyVectorObject::*Func)() );
        void MakeInDepthBBox();
        virtual void UpdateBBox();
        virtual bool PickShape( const ::ULIS::FRectD &iRoi
                              , const BLImage& iMaskImage );

    protected:
        BLMatrix2D mLocalMatrix;
        BLMatrix2D mInverseLocalMatrix;
        BLMatrix2D mWorldMatrix;
        BLMatrix2D mInverseWorldMatrix;
        std::list<FOdysseyVectorTag*> mTagList;
        std::list<FOdysseyVectorObject*> mChildrenList;
        std::list<FOdysseyVectorObject*> mInvalidatedChildrenList;
        FOdysseyVectorObject* mOldParent;
        FOdysseyVectorObject* mParent;
        bool bSelected;
        bool bExpanded;
        bool bSystem;
        bool bVisible;
        ::ULIS::FRectD mBBox;
        ::ULIS::FRectD mInDepthBBox;
        FOdysseyVectorBucket mBackgroundBucket;
        FOdysseyVectorBucket mForegroundBucket;
        uint32 mID;
        FOdysseyVectorObjectInvalidationFlags mInvalidationFlags;

        FString mName;
        double mOpacity;
        double mTranslationX;
        double mTranslationY;
        double mRotation;
        double mScalingX;
        double mScalingY;
        double mSkewX;
        double mSkewY;
        std::mutex mDrawingMutex;
};
