// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    // Tree Node Added Event
    // - TSharedPtr<tDerived> is the added node
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyTreeNodeAdded, TSharedPtr<tDerived>);

    // Tree Node Removed Event
    // TSharedPtr<tDerived> is the removed node
    // TSharedPtr<tDerived> is the previous parent node
    // int is the index of the removed node in the previous parent node
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOdysseyTreeNodeRemoved, TSharedPtr<tDerived>, TSharedPtr<tDerived>, int);
    
    // Tree Node Moved Event
    // TSharedPtr<tDerived> is the moved node
    // TSharedPtr<tDerived> is the previous parent node
    // int is the index of the removed node in the previous parent node
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOdysseyTreeNodeMoved, TSharedPtr<tDerived>, TSharedPtr<tDerived>, int);

//Construction Destruction
public:
    FOdysseyNTreeShared();
    virtual ~FOdysseyNTreeShared() = 0;
    
public:
    virtual void AddNode( TSharedPtr<tDerived> iNode, int iIndexEmplace = -1 );
    virtual void DeleteNode( int iIndex );

    void DeleteNodeIfExist( TSharedPtr<tDerived> iNodeToDelete );
    void MoveNodeTo( TSharedPtr<tDerived> iNewPositionInTree, ePosition iPosition = ePosition::kAfter );
    
    //Find the first node (depthSearch) which content is iToFind
    //FOdysseyNTreeShared<T>* FindNode( T iToFind );
    
    TSharedPtr<tDerived> GetParent() const;
    int GetIndexInParent() const;
    // T GetNodeContent() const;
    // T* GetNodeContentPtr() const;
    
    TArray<TSharedPtr<tDerived>> GetNodes() const;
    TSharedPtr<tDerived> GetNode(int iIndex) const;
    
    void DepthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot = true );
    void BreadthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot = true );
    
    int GetNumberParents() const;
    
    //Check if iParentToSearch is a parent (direct or indirect) of the currentNode
    bool HasForParent( TSharedPtr<tDerived> iParentToSearch ) const;

    FOdysseyTreeNodeAdded& NodeAdded();
    FOdysseyTreeNodeRemoved& NodeRemoved();
    FOdysseyTreeNodeMoved& NodeMoved();

	void BroadcastNodeAdded(TSharedPtr<tDerived> iNode);
	void BroadcastNodeRemoved(TSharedPtr<tDerived> iNode, TSharedPtr<tDerived> iParent, int iIndex);
	void BroadcastNodeMoved(TSharedPtr<tDerived> iNode, TSharedPtr<tDerived> iParent, int iIndex);

private:
    TArray<TSharedPtr<tDerived>> mNodes;
    TWeakPtr<tDerived> mParent;
    
    FOdysseyTreeNodeAdded mNodeAdded;
    FOdysseyTreeNodeRemoved mNodeRemoved;
    FOdysseyTreeNodeMoved mNodeMoved;
};

//Implementation
#include "OdysseyTreeShared.inl"
