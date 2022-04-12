#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <map>
#include "sharedstruct.h"

class FbxLoader {
public:
    bool FbxLoad(const char *file_name);
    void Destroy();

    std::vector<Mesh> GetMeshes(); // TODO: �����I�ɂ̓A�h���X��Ԃ��悤�ɂ�����
    void ClearMeshes();

private:
    fbxsdk::FbxManager *mp_fbx_manager = nullptr;
    fbxsdk::FbxImporter *mp_fbx_importer = nullptr;
    fbxsdk::FbxScene *mp_fbx_scene = nullptr;

    std::vector<Mesh> m_meshes; // FbxMesh��DirectX�Ŏg����悤�ɕϊ�����Mesh�̊i�[�ꏊ
};
