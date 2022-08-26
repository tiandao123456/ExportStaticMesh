// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Engine.h"
#include "Serialization/Archive.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExportResDatasBPLibrary.generated.h"

//struct FVertexStruct
//{
//	//���������
//	int32 Index = 0;
//
//	//�����λ�ã���ʼ��Ϊ(0,0,0)
//	FVector Position = FVector::ZeroVector;
//
//	//����ķ���,��ʼ��Ϊ(0,0,0)
//	FVector Normal = FVector::ZeroVector;
//
//	//������������꣬Ϊʲô������?
//	FVector2D TexCoord0 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord1 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord2 = FVector2D::ZeroVector;
//};

USTRUCT(BlueprintType)
struct FStaticMeshData
{
	GENERATED_USTRUCT_BODY()

		//��������
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		int32 VerticesNum;

	//��������
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<int32> Indices;

	////�ı���Ӧ
	//UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
	//	FString VsFormat;

	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<float> Vertices;

public:
	FStaticMeshData() = default;
	FStaticMeshData(TArray<float> Vertices, TArray<int32> Indices);
};

USTRUCT(BlueprintType)
struct FCameraInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Target;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Rotator;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Fov;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Aspect;

public:
	FCameraInfo() : Location(FVector()), Target(FVector()), Rotator(FVector()), Fov(0.f), Aspect(0.f) {}
};

USTRUCT(BlueprintType)
struct FWorldCoordinateInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector Scale;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol1;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol2;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol3;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol4;
};

//namespace RE
//{
//	//����
//	struct FStaticVertex_RE
//	{
//		//����λ�á����ߡ��������ꡢ������ɫ
//		FVector Position;
//		FVector TangentZ;
//		FVector2D UV0;
//		FVector4 Color;
//	};
//
//	struct FStaticMesh_Lod_RE
//	{
//		int32 VertStride;
//		TArray<FStaticVertex_RE> Vertice;
//		TArray<int32> Indices;
//	};
//}
//
//using namespace RE;
/**
 *
 */

UCLASS()
class UExportResDatasBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	//������̬����������
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void ExportStaticMesh(const UStaticMesh* StaticMesh, FString OutputPath = TEXT(""), const FString& Filename = TEXT("StaticMeshMessage"));

	//��������������json����ʽд���ļ���
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void WriteFile(const FString& FileString, FString OutputPath, const FString& Filename);

	//��þ�̬������Ķ�������
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output);

	//��þ�̬���������������
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output);

	//�������������
	UFUNCTION(BlueprintCallable, Category = "ResExport", meta = (WorldContext = WorldContextObject))
		static void ExportCamera(const UObject* WorldContextObject, const UCameraComponent* CameraComponent, FString OutputPath = TEXT(""), const FString& Filename = TEXT("CameraDataMessage"));

	//������̬�������ƽ����ת���ŵ���Ϣ
	UFUNCTION(BlueprintCallable, Category = "ResExport", meta = (WorldContext = WorldContextObject))
		static void ExportActorWorldCoordinate(const AActor* StaticMeshActor, FString OutputPath = TEXT(""), const FString& Filename = TEXT("WorldCoordinate"));


	//����ģ��
	template <typename TargetStruct>
	static void ExportStructByJsonConverter(const TargetStruct& Target, FString OutputPath, const FString& Filename )
	{
		FString OutputString;
		//���Զ���ṹ��ת��ΪJson�ַ���
		//����struct�Ľṹд��
		FJsonObjectConverter::UStructToJsonObjectString(Target, OutputString);
		//�����ָ���ļ���
		WriteFile(OutputString, OutputPath, Filename);
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "ExportResDatas sample test testing"), Category = "ExportResDatasTesting")
		static float ExportResDatasSampleFunction(float Param);
};
