// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "GridMovementComponent.h"

#include "NavTileComponent.generated.h"


/**
* A single tile in a navigation grid
*/
UCLASS(meta = (BlueprintSpawnableComponent), Blueprintable)
class NAVGRID_API UNavTileComponent : public UBoxComponent
{
	GENERATED_BODY()
public:
	UNavTileComponent(const FObjectInitializer &ObjectInitializer);

protected:
	UPROPERTY()
	ANavGrid *Grid;
public:
	void SetGrid(ANavGrid *InGrid);
	ANavGrid* GetGrid() const;

	// UPrimitiveComponent interface
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void UpdateBodySetup() override;
	// UPrimitiveComponent interface end

// Pathing
	/* Cost of moving into this tile*/
	UPROPERTY(BlueprintReadWrite, EditAnyWhere, Category = "Pathfinding")
	float Cost = 1;
	/* Distance from starting point of path */
	float Distance;
	/* Previous tile in path */
	UPROPERTY()
	UNavTileComponent *Backpointer;
	/* Is this node in the 'visited' set? - Helper var for pathfinding */
	bool Visited;
	/* Reset variables used in pathfinding */
	virtual void ResetPath();

protected:
	UPROPERTY()
	TArray<UNavTileComponent *> Neighbours;
	void FindNeighbours();
public:
	/* Tiles that are reachable from this tile */
	virtual TArray<UNavTileComponent *> *GetNeighbours() { return &Neighbours; }
	/* Add a tile to the list of neighbouring tiles */
	void AddNeighbour(UNavTileComponent *Neighbour) { Neighbours.AddUnique(Neighbour); }
	/* Remove a til from the list of neighbouring tiles */
	void RemoveNeighbour(UNavTileComponent *Neighbour) { Neighbours.Remove(Neighbour); }

	FCollisionShape NeighbourhoodShape;
	/* movement modes that are legal (or make sense) for this tile */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSet<EGridMovementMode> MovementModes;

	/* is there anything blocking an actor from moving from FromPos to this tile? Uses the capsule for collision testing */
	virtual bool Obstructed(const FVector &FromPos, const UCapsuleComponent &CollisionCapsule) const;
	/* is there anything blocking an actor from moving between From and To? Uses the capsule for collision testing */
	virtual bool Obstructed(const FVector &From, const FVector &To, const UCapsuleComponent &CollisionCapsule) const;
	/* Return the neighbours that are not Obstructed() */
	virtual void GetUnobstructedNeighbours(const UCapsuleComponent &CollisionCapsule, TArray<UNavTileComponent *> &OutNeighbours);
	/* Can a pawn traverse this tile?
	*
	* MaxWalkAngle: the pawns MaxWalkAngle
	* PawnMovementModes: movement modes availabe for the pawn
	*/
	virtual bool Traversable(float MaxWalkAngle, const TSet<EGridMovementMode> &PawnMovementModes) const;
	/* Can a pawn end its turn on this tile?*/
	virtual bool LegalPositionAtEndOfTurn(float MaxWalkAngle, const TSet<EGridMovementMode> &PawnMovementModes) const;

	/* Placement for pawn occupying this tile in world space */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Default")
	virtual FVector GetPawnLocation() const;
	/* Set offset in local space for pawns occupynig this tile */
	UFUNCTION(BlueprintCallable, Category = "Default")
	void SetPawnLocationOffset(const FVector &Offset);
protected:
	/* Offset in local space for any pawn occupying this tile */
	FVector PawnLocationOffset;

public:
// User interface
	UFUNCTION()
	void Clicked(UPrimitiveComponent* TouchedComponent, FKey Key);
	UFUNCTION()
	void CursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void EndCursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void TouchEnter(ETouchIndex::Type Type, UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void TouchLeave(ETouchIndex::Type Type, UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void TouchEnd(ETouchIndex::Type Type, UPrimitiveComponent* TouchedComponent);

	/*
	* Add points for moving into this tile from FromPos
	*
	* OutSpline - the spline to add the points to
	* OutPathSegments - the array we should add our path segment to
	* EndTile - true if this is the last tile in the path
	*/
	virtual void AddPathSegments(USplineComponent &OutSpline, TArray<FPathSegment> &OutPathSegments, bool EndTile) const;
	/* Return a suitable upvector for a splinemesh moving across this tile */
	virtual FVector GetSplineMeshUpVector();

	/* Set a highlight for this tile */
	virtual void SetHighlight(FName NewHighlightType);
};