// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "Blaster/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (blasterCharacter == nullptr)
	{
		blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (blasterCharacter == nullptr) return;

	FVector velocity = blasterCharacter->GetVelocity();
	velocity.Z = 0.f;
	Speed = velocity.Size();
	
	bIsInAir = blasterCharacter->GetMovementComponent()->IsFalling();
	bIsAccelerating = blasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}