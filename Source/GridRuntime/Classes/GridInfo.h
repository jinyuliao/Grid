#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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

	/**
	property has changed, notify GridPainter refresh grid state
	*/
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void Dirty();

	UPROPERTY(BlueprintReadWrite, Category = "GridInfo")
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridInfo")
	FGameplayTagContainer GameplayTags;

	UGrid* ParentGrid;
};
