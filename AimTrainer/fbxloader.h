#pragma once
#include "sharedstruct.h"
#include <fbxsdk.h>
#include <map>
#include <string>
#include <vector>

class FbxLoader {
public:
    bool FbxLoad(const char *file_name);
    void Destroy();

    std::vector<Mesh> GetMeshes(); // TODO: 将来的にはアドレスを返すようにしたい
    void ClearMeshes();

private:
    fbxsdk::FbxManager *mp_fbx_manager = nullptr;
    fbxsdk::FbxImporter *mp_fbx_importer = nullptr;
    fbxsdk::FbxScene *mp_fbx_scene = nullptr;

    std::vector<Mesh> m_meshes; // FbxMeshをDirectXで使えるように変換したMeshの格納場所
};
