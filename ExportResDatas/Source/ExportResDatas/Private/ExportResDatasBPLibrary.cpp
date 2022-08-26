// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportResDatasBPLibrary.h"
#include "ExportResDatas.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogResExporter, Log, All);
//namespace RE
//{
//	//<<运算符重载，能对FStaticVertex_RE结构体成员变量进行操作
//	FArchive& operator<<(FArchive& Ar, FStaticVertex_RE& Value)
//	{
//		Ar << Value.Position;
//		Ar << Value.TangentZ;
//		Ar << Value.UV0;
//		Ar << Value.Color;
//		return Ar;
//	}
//
//	FArchive& operator<<(FArchive& Ar, FStaticMesh_Lod_RE& Value)
//	{
//		Ar << Value.VertStride;
//		Ar << Value.Vertice;
//		Ar << Value.Indices;
//		return Ar;
//	}
//}

UExportResDatasBPLibrary::UExportResDatasBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FStaticMeshData::FStaticMeshData(TArray<float> Vertices, TArray<int32> Indices) : Indices(Indices), Vertices(Vertices)
{
	//VsFormat =
	//	TEXT("POSITION,")
	//	TEXT("NORMAL,")
	//	TEXT("TEXCOORD0,")
	//	TEXT("TEXCOORD1");

	VerticesNum = Vertices.Num();
}

void UExportResDatasBPLibrary::WriteFile(const FString& FileString, FString OutputPath, const FString& Filename)
{
	if (OutputPath.IsEmpty())
	{
		OutputPath = FPaths::ProfilingDir();
	}

	FString Fullname = OutputPath + Filename + TEXT(".json");

	if (!FFileHelper::SaveStringToFile(FileString, *Fullname, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogResExporter, Display, TEXT("Failed to write to temp file: %s"), *Fullname);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to write to temp file: %s"), *Fullname));
	}
	else
	{
		UE_LOG(LogResExporter, Error, TEXT("Succeed to save file: %s"), *Fullname);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Succeed to save file: %s"), *Fullname));
	}
}

void UExportResDatasBPLibrary::GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output)
{
	TArray<float> Vertices;

	if (!StaticMesh || !StaticMesh->GetRenderData())
	{
		return;
	}
	if (StaticMesh->GetRenderData()->LODResources.Num() > 0)
	{
		const FStaticMeshVertexBuffers* VertexBuffers = &StaticMesh->GetRenderData()->LODResources[0].VertexBuffers;

		if (VertexBuffers)
		{
			//将顶点的位置信息，法线，uv坐标存放到Vertices中
			for (uint32 i = 0; i < VertexBuffers->PositionVertexBuffer.GetNumVertices(); i++)
			{
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).X);
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).Y);
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).Z);

				//Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).X);
				//Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).Y);
				//Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).Z);

				//Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.GetVertexUV(i, 0).X);
				//Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.GetVertexUV(i, 0).Y);
			}
		}
		else
		{
			return;
		}
	}

	Output = Vertices;
}

void UExportResDatasBPLibrary::GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output)
{
	TArray<int32> Indices = TArray<int32>();

	if (!StaticMesh || !StaticMesh->GetRenderData())
	{
		return;
	}
	if (StaticMesh->GetRenderData()->LODResources.Num() > 0)
	{
		const FRawStaticIndexBuffer* IndexBuffer = &StaticMesh->GetRenderData()->LODResources[0].IndexBuffer;
		if (IndexBuffer)
		{
			const int32 IndexCount = IndexBuffer->GetNumIndices();
			for (int32 i = 0; i < IndexCount; i++)
			{
				const int32 Index = (int32)IndexBuffer->GetIndex(i);
				Indices.Add(Index);
			}
		}
	}

	Output = Indices;
}

//指向静态网格体对象的指针由蓝图给出
void UExportResDatasBPLibrary::ExportStaticMesh(const UStaticMesh* StaticMesh, FString Path, const FString& Filename)
{
	//获得顶点数据（包含顶点坐标、顶点法线坐标以及顶点的纹理坐标）
	TArray<float> Vertices;
	GetStaticMeshVerticesData(StaticMesh, Vertices);

	//获得静态网格体的索引数据
	TArray<int32> Indices;
	GetStaticMeshIndicesData(StaticMesh, Indices);

	FStaticMeshData Data(Vertices, Indices);

	ExportStructByJsonConverter(Data, Path, Filename);
}

void UExportResDatasBPLibrary::ExportCamera(const UObject* WorldContextObject, const UCameraComponent* CameraComponent, FString OutputPath, const FString& Filename)
{
	//UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	FCameraInfo CamData;

	FTransform Trans = CameraComponent->GetComponentToWorld();
	//相机的位置
	CamData.Location = Trans.GetLocation();
	//相机的旋转
	CamData.Rotator = Trans.GetRotation().Euler();
	//相机的前向矢量
	CamData.Target = CamData.Location + CameraComponent->GetForwardVector();
	//相机的视野
	CamData.Fov = CameraComponent->FieldOfView;
	//相机的纵横比
	CamData.Aspect = CameraComponent->AspectRatio;

	ExportStructByJsonConverter(CamData, OutputPath, Filename);
}

void UExportResDatasBPLibrary::ExportActorWorldCoordinate(const AActor* StaticMeshActor, FString OutputPath, const FString& Filename)
{
	FWorldCoordinateInfo WorldCoordinateInfo;
	FTransform transform = StaticMeshActor->GetTransform();
	WorldCoordinateInfo.Location = transform.GetLocation();
	WorldCoordinateInfo.Rotation = transform.Rotator();
	WorldCoordinateInfo.Scale = transform.GetScale3D();
	FMatrix44d WorldMatrix = transform.ToMatrixWithScale();
	WorldCoordinateInfo.MatrixCol1 = FVector4(WorldMatrix.M[0][0], WorldMatrix.M[1][0], WorldMatrix.M[2][0], WorldMatrix.M[3][0]);
	WorldCoordinateInfo.MatrixCol2 = FVector4(WorldMatrix.M[0][1], WorldMatrix.M[1][1], WorldMatrix.M[2][1], WorldMatrix.M[3][1]);
	WorldCoordinateInfo.MatrixCol3 = FVector4(WorldMatrix.M[0][2], WorldMatrix.M[1][2], WorldMatrix.M[2][2], WorldMatrix.M[3][2]);
	WorldCoordinateInfo.MatrixCol4 = FVector4(WorldMatrix.M[0][3], WorldMatrix.M[1][3], WorldMatrix.M[2][3], WorldMatrix.M[3][3]);

	ExportStructByJsonConverter(WorldCoordinateInfo, OutputPath, Filename);
}


float UExportResDatasBPLibrary::ExportResDatasSampleFunction(float Param)
{
	return -1;
}