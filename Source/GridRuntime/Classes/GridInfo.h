#pragma once

#include "CoreMinimal.h"
#include "GridInfo.generated.h"

class UGrid;

/**
 * 
 */
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridInfo : public UObject
{
	GENERATED_BODY()
	
public:
	UGridInfo();
	virtual ~UGridInfo();	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridInfo")
	void Clear();
	virtual void Clear_Implementation();

	UPROPERTY(BlueprintReadWrite, Category = "GridInfo")
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (Bitmask), Category = "GridInfo")
	int32 ExtraFlag;

	/**
		property has changed, notify GridPainter refresh grid state
	*/
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void Dirty();

	/** Index range: [0, 32) */
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetExtraFlag(int32 Index);

	/** Index range: [0, 32) */
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void ClearExtraFlag(int32 Index);

	/** Index range: [0, 32) */
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void ToggleExtraFlag(int32 Index);

	/** Index range: [0, 32) */
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	bool CheckExtraFlag(int32 Index);

	UGrid* ParentGrid;
};
