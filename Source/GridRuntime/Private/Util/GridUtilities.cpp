#include "Util/GridUtilities.h"

uint64 UGridUtilities::GetUniqueIdByCoordinate(const FIntVector& Coord)
{
	return ((uint64)Coord.Z << 44) + ((uint64)Coord.Y << 22) + (uint64)Coord.X;
}

FVector UGridUtilities::CalcGridDecalSize(EGridType GridType, float GridSize)
{
	switch (GridType)
	{
	case EGridType::Square:
		{
			FVector DecalSize(GridSize, GridSize, GridSize);

			DecalSize /= FVector(4.f, 2.f, 2.f);

			return DecalSize;
		}
	case EGridType::Hexagon:
		{
			FVector DecalSize(GridSize, GridSize * 2, GridSize * FMath::Sqrt(3));

			DecalSize /= 2;

			return DecalSize;
		}
	default:
		LOG_ERROR(TEXT("UGridUtilities::CalcGridDecalSize unknown grid type"));
		return FVector::ZeroVector;
	}
}

FIntVector UGridUtilities::SubIntVector(const FIntVector& L, const FIntVector& R)
{
	return L - R;
}

FIntVector UGridUtilities::AddIntVector(const FIntVector& L, const FIntVector& R)
{
	return L + R;
}

FIntVector UGridUtilities::IntVectorMulInt(const FIntVector& L, int R)
{
	return L * R;
}

bool UGridUtilities::GridTraceSingleForObjects(UGrid* Grid, float TraceDistance, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Start = Grid->GetCenter() + FVector(0.f, 0.f, TraceDistance / 2.f);
	FVector End = Grid->GetCenter() - FVector(0.f, 0.f, TraceDistance / 2.f);

	return UKismetSystemLibrary::LineTraceSingleForObjects((UObject*)Grid->GridManager->GetWorld(), Start, End, ObjectTypes
		, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit
		, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
}
