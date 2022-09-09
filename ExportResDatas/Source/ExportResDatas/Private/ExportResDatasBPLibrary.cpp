// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportResDatasBPLibrary.h"
#include "ExportResDatas.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogResExporter, Log, All);
//namespace RE
//{
//	//<<��������أ��ܶ�FStaticVertex_RE�ṹ���Ա�������в���
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
			//�������λ����Ϣ�����ߣ�uv�����ŵ�Vertices��
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

	//���������е�����staticMeshActor������staticMeshActor��˵ֻ��Ψһһ��model����
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AStaticMeshActor::StaticClass(), OutActors);
	for (auto num = 0; num < OutActors.Num(); num++)
	{
		AStaticMeshActor* staticMeshActor = Cast<AStaticMeshActor>(OutActors[num]);
		FTransform transform = staticMeshActor->GetTransform();
		FMatrix44d WorldMatrix = transform.ToMatrixWithScale();
		////������staticMeshActor��λ�á���ת������
		//transform.GetLocation();
		//transform.Rotator();
		//transform.GetScale3D();

		//��ȡstaticMeshActor��modelMatrix
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

		//��ö������ݣ������������ꡢ���㷨�������Լ�������������꣩
		TArray<float> Vertices;
		GetStaticMeshVerticesData(staticMesh, Vertices);

		//��þ�̬���������������
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
		//�����λ��
		cameraData.Location = trans.GetLocation();
		//�������ת
		cameraData.Rotator = trans.GetRotation().Euler();
		//�����ǰ��ʸ��
		cameraData.Target = cameraData.Location + cameraComponent->GetForwardVector();
		//�������Ұ
		cameraData.Fov = cameraComponent->FieldOfView;
		//������ݺ��
		cameraData.Aspect = cameraComponent->AspectRatio;
		cameraData.CameraName = cameraActor->GetName();

		sceneData.cameraInfo.Add(cameraData);
	}
	FString OutputPath = TEXT("");
	const FString& Filename = TEXT("SceneMessage");
	ExportStructByJsonConverter(sceneData, OutputPath, Filename);
}


