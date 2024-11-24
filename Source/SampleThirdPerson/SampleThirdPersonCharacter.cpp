// Copyright Epic Games, Inc. All Rights Reserved.

#include "SampleThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);
DEFINE_LOG_CATEGORY_STATIC(LogCharacterJump, Log, All);

//////////////////////////////////////////////////////////////////////////
// ASampleThirdPersonCharacter

ASampleThirdPersonCharacter::ASampleThirdPersonCharacter()
{
  // Add this at the very start of the constructor
  UE_LOG(LogTemp, Warning, TEXT("Character Constructor Called!"));

  // Set size for collision capsule
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  // Don't rotate when the controller rotates. Let that just affect the camera.
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // Initialize double jump variables
  bCanDoubleJump = true;
  DoubleJumpZVelocity = 700.f;

  // Configure character movement
  GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

  // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
  // instead of recompiling to adjust them
  GetCharacterMovement()->JumpZVelocity = 700.f;
  GetCharacterMovement()->AirControl = 0.35f;
  GetCharacterMovement()->MaxWalkSpeed = 500.f;
  GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

  // Create a camera boom (pulls in towards the player if there is a collision)
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
  CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

  // Create a follow camera
  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
  FollowCamera->bUsePawnControlRotation = false;                              // Camera does not rotate relative to arm

  // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
  // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASampleThirdPersonCharacter::NotifyControllerChanged()
{
  Super::NotifyControllerChanged();

  // Add Input Mapping Context
  if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
  {
    if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }
}

void ASampleThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
  // Set up action bindings
  if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
  {
    UE_LOG(LogCharacterJump, Log, TEXT("Setting up Enhanced Input"));

    // Jumping
    if (JumpAction)
    {
      UE_LOG(LogCharacterJump, Log, TEXT("Binding Jump Action"));
      EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASampleThirdPersonCharacter::Jump);
      EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    }
    else
    {
      UE_LOG(LogCharacterJump, Error, TEXT("JumpAction is null!"));
    }

    // Moving
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASampleThirdPersonCharacter::Move);

    // Looking
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASampleThirdPersonCharacter::Look);
  }
  else
  {
    UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
  }
}

void ASampleThirdPersonCharacter::Move(const FInputActionValue &Value)
{
  // input is a Vector2D
  FVector2D MovementVector = Value.Get<FVector2D>();
  UE_LOG(LogCharacterJump, Warning, TEXT("Moving Character"))

  if (Controller != nullptr)
  {
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // get forward vector
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    // get right vector
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // add movement
    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
  }
}

void ASampleThirdPersonCharacter::Look(const FInputActionValue &Value)
{
  // input is a Vector2D
  FVector2D LookAxisVector = Value.Get<FVector2D>();

  if (Controller != nullptr)
  {
    // add yaw and pitch input to controller
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
  }
}

void ASampleThirdPersonCharacter::Jump()
{
  UE_LOG(LogTemp, Warning, TEXT("Jump Called"));

  // If we're falling and near a wall, perform wall jump
  if (GetCharacterMovement()->IsFalling() && IsNearWall() && bCanWallJump)
  {
    // Get the wall normal to jump away from it
    FVector WallNormal = GetWallNormal();

    // Calculate jump direction (up + away from wall)
    FVector JumpDirection = (WallNormal * WallJumpHorizontalForce) + FVector(0, 0, WallJumpForce);

    // Launch character
    LaunchCharacter(JumpDirection, true, true);

    // Disable wall jump temporarily
    bCanWallJump = false;

    // Play effects
    if (WallJumpEffect)
    {
      UGameplayStatics::SpawnEmitterAtLocation(
          GetWorld(),
          WallJumpEffect,
          GetActorLocation(),
          WallNormal.Rotation());
    }

    if (WallJumpSound)
    {
      UGameplayStatics::PlaySoundAtLocation(
          this,
          WallJumpSound,
          GetActorLocation());
    }

    // Reset wall jump after cooldown
    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(
        UnusedHandle,
        this,
        &ASampleThirdPersonCharacter::ResetWallJump,
        WallJumpCooldown,
        false);
  }
  // If we're falling but not near a wall, try double jump
  else if (GetCharacterMovement()->IsFalling())
  {
    if (bCanDoubleJump)
    {
      UE_LOG(LogTemp, Warning, TEXT("Double Jump"));
      GetCharacterMovement()->Velocity.Z = DoubleJumpZVelocity;
      bCanDoubleJump = false;

      if (DoubleJumpEffect)
      {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            DoubleJumpEffect,
            GetActorLocation() + EffectOffset,
            GetActorRotation());
      }

      if (DoubleJumpSound)
      {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            DoubleJumpSound,
            GetActorLocation(),
            1.0f,
            1.0f,
            0.0f);
      }
    }
  }
  else
  {
    Super::Jump();
  }
}

bool ASampleThirdPersonCharacter::IsNearWall() const
{
  FVector Start = GetActorLocation();

  // Check in multiple directions around the character
  TArray<FVector> Directions = {
      GetActorRightVector(),
      -GetActorRightVector(),
      GetActorForwardVector(),
      -GetActorForwardVector()};

  for (const FVector &Direction : Directions)
  {
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    FVector End = Start + (Direction * WallDetectionRange);

    // Perform line trace
    if (GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility,
            QueryParams))
    {
      // Check if we hit something we can wall jump from
      if (HitResult.GetComponent() && HitResult.GetComponent()->IsSimulatingPhysics() == false)
      {
        return true;
      }
    }
  }

  return false;
}

FVector ASampleThirdPersonCharacter::GetWallNormal() const
{
  FVector Start = GetActorLocation();

  // Check in multiple directions around the character
  TArray<FVector> Directions = {
      GetActorRightVector(),
      -GetActorRightVector(),
      GetActorForwardVector(),
      -GetActorForwardVector()};

  for (const FVector &Direction : Directions)
  {
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    FVector End = Start + (Direction * WallDetectionRange);

    if (GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility,
            QueryParams))
    {
      if (HitResult.GetComponent() && HitResult.GetComponent()->IsSimulatingPhysics() == false)
      {
        return HitResult.Normal;
      }
    }
  }

  return FVector::UpVector;
}

void ASampleThirdPersonCharacter::ResetWallJump()
{
  bCanWallJump = true;
}

void ASampleThirdPersonCharacter::Landed(const FHitResult &Hit)
{
  Super::Landed(Hit);
  bCanDoubleJump = true;
}

// And implement it in the cpp file:
void ASampleThirdPersonCharacter::BeginPlay()
{
  Super::BeginPlay();
  UE_LOG(LogTemp, Warning, TEXT("Character BeginPlay Called!"));

  // Also verify input setup
  if (JumpAction)
  {
    UE_LOG(LogTemp, Warning, TEXT("JumpAction is valid"));
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("JumpAction is NULL - Input won't work!"));
  }
}