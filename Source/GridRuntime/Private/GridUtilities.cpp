#include "GridUtilities.h"

uint64 UGridUtilities::GetUniqueIdByCoordinate(const FIntVector& Coord)
{
	return ((uint64)Coord.Z << 44) + ((uint64)Coord.Y << 22) + (uint64)Coord.X;
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
