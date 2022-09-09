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

FStaticMeshData::FStaticMeshData(FString nameParameter, TArray<float>& Vertices, TArray<int32>& Indices, TArray<float>& modelMatrix):
	Vertices(Vertices),Indices(Indices), VerticesNum(Vertices.Num()),StaticMeshName(nameParameter),ModelMatrix(modelMatrix)
{
	//VsFormat =
	//	TEXT("POSITION,")
	//	TEXT("NORMAL,")
	//	TEXT("TEXCOORD0,")
	//	TEXT("TEXCOORD1");
}


void UExportResDatasBPLibrary::WriteFile(const FString& FileString, FString OutputPath, const FString& Filename)
{
	if (OutputPath.IsEmpty())
	{
		OutputPath = FPaths::ProfilingDir();
	}

	FString Fullname = OutputPath + Filename + TEXT(".json");

	if (!FFileHelper::SaveStringToFile("\n" + FileString, *Fullname, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(),EFileWrite::FILEWRITE_Append))
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

void UExportResDatasBPLibrary::ExportScene(const UObject* WorldContextObject)
{
	FSceneData sceneData;

	TSubclassOf<AStaticMeshActor> staticMeshActorClass;
	TSubclassOf<ACameraActor> cameraActorClass;
	TArray<AActor*> OutActors;

	//遍历场景中的所有staticMeshActor，对于staticMeshActor来说只有唯一一个model矩阵
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AStaticMeshActor::StaticClass(), OutActors);
	for (auto num = 0; num < OutActors.Num(); num++)
	{
		AStaticMeshActor* staticMeshActor = Cast<AStaticMeshActor>(OutActors[num]);
		FTransform transform = staticMeshActor->GetTransform();
		FMatrix44d WorldMatrix = transform.ToMatrixWithScale();
		////场景中staticMeshActor的位置、旋转、缩放
		//transform.GetLocation();
		//transform.Rotator();
		//transform.GetScale3D();

		//获取staticMeshActor的modelMatrix
		TArray<float> modelMatrix;
		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 4; j++)
			{
				modelMatrix.Add(WorldMatrix.M[i][j]);
			}
		}

		UStaticMeshComponent* staticMeshComponent = staticMeshActor->GetStaticMeshComponent();
		UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();

		//获得顶点数据（包含顶点坐标、顶点法线坐标以及顶点的纹理坐标）
		TArray<float> Vertices;
		GetStaticMeshVerticesData(staticMesh, Vertices);

		//获得静态网格体的索引数据
		TArray<int32> Indices;
		GetStaticMeshIndicesData(staticMesh, Indices);

		FString name = staticMesh->GetName();
		FStaticMeshData Data(name, Vertices, Indices, modelMatrix);

		sceneData.StaticMeshData.Add(Data);
	}

	OutActors.Reset();
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ACameraActor::StaticClass(), OutActors);
	for (auto i = 0; i < OutActors.Num(); i++)
	{
		ACameraActor* cameraActor = Cast<ACameraActor>(OutActors[i]);
		UCameraComponent* cameraComponent = cameraActor->GetCameraComponent();

		FCameraInfo cameraData;
		FTransform trans = cameraComponent->GetComponentToWorld();
		//相机的位置
		cameraData.Location = trans.GetLocation();
		//相机的旋转
		cameraData.Rotator = trans.GetRotation().Euler();
		//相机的前向矢量
		cameraData.Target = cameraData.Location + cameraComponent->GetForwardVector();
		//相机的视野
		cameraData.Fov = cameraComponent->FieldOfView;
		//相机的纵横比
		cameraData.Aspect = cameraComponent->AspectRatio;
		cameraData.CameraName = cameraActor->GetName();

		sceneData.cameraInfo.Add(cameraData);
	}
	FString OutputPath = TEXT("");
	const FString& Filename = TEXT("SceneMessage");
	ExportStructByJsonConverter(sceneData, OutputPath, Filename);
}


