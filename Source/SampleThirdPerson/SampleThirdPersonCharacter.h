// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SampleThirdPersonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ASampleThirdPersonCharacter : public ACharacter
{
  GENERATED_BODY()

  /** Camera boom positioning the camera behind the character */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  USpringArmComponent *CameraBoom;

  /** Follow camera */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  UCameraComponent *FollowCamera;

  /** MappingContext */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputMappingContext *DefaultMappingContext;

  /** Jump Input Action */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction *JumpAction;

  /** Move Input Action */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction *MoveAction;

  /** Look Input Action */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction *LookAction;

public:
  ASampleThirdPersonCharacter();

  /** Double Jump */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
  bool bCanDoubleJump;

  /** Double Jump Velocity */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
  float DoubleJumpZVelocity;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|Effects")
  UParticleSystem *DoubleJumpEffect;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|Effects")
  USoundBase *DoubleJumpSound;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|Effects")
  FVector EffectOffset = FVector(0.f, 0.f, 0.f);

protected:
  /** Called for movement input */
  void Move(const FInputActionValue &Value);

  /** Called for looking input */
  void Look(const FInputActionValue &Value);
  virtual void BeginPlay() override;

  // Wall Jump Properties
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Wall Jump")
  float WallJumpForce = 700.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Wall Jump")
  float WallJumpHorizontalForce = 500.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Wall Jump")
  float WallDetectionRange = 50.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Wall Jump")
  bool bCanWallJump = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Wall Jump")
  float WallJumpCooldown = 0.1f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: VFX")
  UParticleSystem *WallJumpEffect;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: SFX")
  USoundBase *WallJumpSound;

  // Helper function to detect walls
  bool IsNearWall() const;
  FVector GetWallNormal() const;
  void ResetWallJump();

protected:
  virtual void NotifyControllerChanged() override;

  virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

  /** Called for Jump input */
  virtual void Jump() override;
  virtual void Landed(const FHitResult &Hit) override;

public:
  /** Returns CameraBoom subobject **/
  FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
  /** Returns FollowCamera subobject **/
  FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};
