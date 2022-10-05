// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

enum class ePosition : char
{
    kIn,
    kBefore,
    kAfter,
    kInvalid
};

template< typename tDerived >
class FOdysseyNTreeShared : public TSharedFromThis<FOdysseyNTreeShared<tDerived>>
{
public:
    //Events
    // Tree Child Added Event
    // - TSharedPtr<tDerived> is the added Child
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChildAdded, TSharedPtr<tDerived>);

    // Tree Child Removed Event
    // TSharedPtr<tDerived> is the removed Child
    // TSharedPtr<tDerived> is the previous parent
    // int is the index of the removed Child in the previous parent
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChildRemoved, TSharedPtr<tDerived>, TSharedPtr<tDerived>, int);

//Construction Destruction
public:
    FOdysseyNTreeShared();
    virtual ~FOdysseyNTreeShared() = 0;
    
public:
    //Adds a Child to this node
    virtual void AddChild( TSharedPtr<tDerived> iChild, int iIndexEmplace = -1 );

    //Removes a child from this node
    virtual void RemoveChild( int iIndex );

    //Moves this node to somewhere else in the tree (can go from a parent to another)
    void MoveTo( TSharedPtr<tDerived> iNode, ePosition iPosition = ePosition::kAfter );
    
    //Returns the Parent node
    TSharedPtr<tDerived> GetParent() const;
    
    //Returns the number of parent until reaching the root
    int GetNumberParents() const;
    
    //Check if iParentToSearch is a parent (direct or indirect) of the currentNode
    bool HasForParent( TSharedPtr<tDerived> iParentToSearch ) const;

    //Called when this node's parent changed
    virtual void OnParentChanged(TSharedPtr<tDerived> iOldParent);

    //Returns the position of this node in its parent
    int GetIndexInParent() const;
    
    //Retuns this node's children
    TArray<TSharedPtr<tDerived>> GetChildren() const;

    //Returns this node's child at given index
    TSharedPtr<tDerived> GetChild(int iIndex) const;
    
    //Returns the children in depth order
    void DepthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot = true );

    //Returns the children in breadth order
    void BreadthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot = true );

    FOnChildAdded& OnChildAddedDelegate();
    FOnChildRemoved& OnChildRemovedDelegate();

private:
	void BroadcastChildAdded(TSharedPtr<tDerived> iChild);
	void BroadcastChildRemoved(TSharedPtr<tDerived> iChild, TSharedPtr<tDerived> iParent, int iIndex);

private:
    TArray<TSharedPtr<tDerived>> mChildren;
    TWeakPtr<tDerived> mParent;
    
    FOnChildAdded mOnChildAddedDelegate;
    FOnChildRemoved mOnChildRemovedDelegate;
};

//Implementation
#include "OdysseyTreeShared.inl"
