// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SceneCapturePawn.h"
#include "Classes/ClothComponent.h"
#include "SaveGame/SaveCharacterState.h"

#include "SoulNetworkProject/ItemData.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Gameframework/SpringArmComponent.h"

#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


// Sets default values
ASceneCapturePawn::ASceneCapturePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ConstructorHelpers::FObjectFinder<UMaterialInterface> MatInterface_Obj (TEXT("Game/BP/Character/Player/Capture/M_User_Inst"));
	//if (MatInterface_Obj.Succeeded())
	//{
	//	MatInterface = MatInterface_Obj.Object;
	//}

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Collision);
	Collision->SetCapsuleHalfHeight(88.0f);
	Collision->SetCapsuleRadius(34.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Collision);
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(SpringArm);

	SetupBodyComp();
	Cloth = CreateDefaultSubobject<UClothComponent>(TEXT("Cloth"));

}

// Called when the game starts or when spawned
void ASceneCapturePawn::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ASceneCapturePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASceneCapturePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UTexture2D* ASceneCapturePawn::UpdatedMeshs(USaveCharacterState* SaveData)
{
	if (!SaveData) return nullptr;

	for (FDataContent& Content : SaveData->Contents)
	{
		if(!Content.bEquip) continue;

		FInventoryData CurrentData = SearchInvenData(this, Content, false);
		if (CurrentData.IsSet())
		{
			switch (CurrentData.ItemData.ItemType)
			{
				case EItemType::Helmet:
				case EItemType::Armor:
				case EItemType::Pant:
				case EItemType::Boots:
				{
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, __FUNCTION__);
					GetClothComponent()->AddClothMesh(CurrentData.ItemData, CurrentData.ClothData);
				}
					break;
				default:
					break;
			}
		}
	}

	SceneCapture->CaptureScene();
	//if (MatInterface)
	//{
	//	 UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(MatInterface, this);
	//	 if (MatInstance)
	//	 {
	//		 MatInstance->SetTextureParameterValue(TEXT("RenderTexture"), SceneCapture->C)
	//	 }
	//}

	return GetRenderTargetToTexture(SceneCapture->TextureTarget);
}

UTexture2D* ASceneCapturePawn::GetRenderTargetToTexture(UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget) return nullptr;

	UTexture2D* Texture2D = RenderTarget->ConstructTexture2D(
		this, TEXT("texture"), EObjectFlags::RF_NoFlags, CTF_DeferCompression);
	if (!Texture2D) return nullptr;

	RenderTarget->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

#if WITH_EDITORONLY_DATA
	// 밉맵 생성 설정은 에디터 전용 데이터라 패키징 빌드에는 존재하지 않는다.
	Texture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif

	Texture2D->SRGB = 1;
	Texture2D->UpdateResource();

	return Texture2D;
}

void ASceneCapturePawn::SetupBodyComp()
{

	//Body Mesh//
	Body_Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Head"));
	Body_Head->SetupAttachment(GetRootComponent());
	Body_Head->ComponentTags.Add(TEXT("BODY_HEAD"));
	Body_Head->SetRelativeLocation(FVector(0.0f, 0.0f, -83.0f));
	Body_Head->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	Body_Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Chest"));
	Body_Chest->SetupAttachment(Body_Head);
	Body_Chest->ComponentTags.Add(TEXT("BODY_CHEST"));

	Body_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Arms"));
	Body_Arms->SetupAttachment(Body_Head);
	Body_Arms->ComponentTags.Add(TEXT("BODY_ARMS"));

	Body_hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_hands"));
	Body_hands->SetupAttachment(Body_Head);
	Body_hands->ComponentTags.Add(TEXT("BODY_HANDS"));

	Body_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Legs"));
	Body_Legs->SetupAttachment(Body_Head);
	Body_Legs->ComponentTags.Add(TEXT("BODY_LEDGS"));

	Body_Feets = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Feets"));
	Body_Feets->SetupAttachment(Body_Head);
	Body_Feets->ComponentTags.Add(TEXT("BODY_FEETS"));
	//////////

	//EquipMesh//
	Eq_Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Helmet"));
	Eq_Helmet->SetupAttachment(Body_Head);
	Eq_Helmet->ComponentTags.Add(TEXT("HELMET"));

	Eq_Armor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Armor"));
	Eq_Armor->SetupAttachment(Body_Head);
	Eq_Armor->ComponentTags.Add(TEXT("ARMOR"));

	Eq_Pant = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Pant"));
	Eq_Pant->SetupAttachment(Body_Head);
	Eq_Pant->ComponentTags.Add(TEXT("PANT"));

	Eq_Boots = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Boots"));
	Eq_Boots->SetupAttachment(Body_Head);
	Eq_Boots->ComponentTags.Add(TEXT("BOOTS"));
	/////////////
}

