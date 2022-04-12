#include "fbxloader.h"
#include "sharedstruct.h"
#include <d3dx12.h>
#include <DirectXMath.h>

using namespace fbxsdk;
using namespace std;
using namespace DirectX;

// string(マルチバイト文字列)からwstring(ワイド文字列)を得る
wstring ToWideString(const string &str) {
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

/// <summary>
/// Fbxファイルをロード
/// </summary>
/// <param name="p_filename">fbxファイル名</param>
/// <returns>成功：true 失敗：false</returns>
bool FbxLoader::FbxLoad(const char *file_name) {
    // FbxManeger作成
    mp_fbx_manager = FbxManager::Create();
    if (mp_fbx_manager == nullptr) {
        printf("Fbxマネージャーの作成に失敗");

        return false;
    }

    // FbxImporter作成
    mp_fbx_importer = FbxImporter::Create(mp_fbx_manager, "FbxImporter");
    if (mp_fbx_importer == nullptr) {
        printf("Fbxインポーターの作成に失敗");
        Destroy();

        return false;
    }

    // FbxScene作成
    mp_fbx_scene = FbxScene::Create(mp_fbx_manager, "FbxScene");
    if (mp_fbx_scene == nullptr) {
        printf("Fbxシーンの作成に失敗");
        Destroy();

        return false;
    }

    // ファイル初期化
    if (mp_fbx_importer->Initialize(file_name) == false) {
        printf("ファイルの初期化に失敗");
        Destroy();

        return false;
    }

    // ファイルインポート
    if (mp_fbx_importer->Import(mp_fbx_scene) == false) {
        printf("ファイルのインポートに失敗");
        Destroy();

        return false;
    }

    // ポリゴンを三角形にする
    FbxGeometryConverter converter(mp_fbx_manager);
    converter.Triangulate(mp_fbx_scene, true);

    // Mesh取得
    int meshcount = mp_fbx_scene->GetSrcObjectCount<FbxMesh>();
    vector<FbxMesh *>fbx_meshes; // Fbxファイルから取り出したMeshの格納場所
    fbx_meshes.resize(meshcount);

    // Material取得
    vector<FbxSurfaceMaterial *> fbx_materials;
    fbx_materials.resize(meshcount);

    for (int i = 0; i < meshcount; ++i) {
        FbxMesh *p_mesh = mp_fbx_scene->GetSrcObject<FbxMesh>(i);
        fbx_meshes[i] = p_mesh;

        FbxSurfaceMaterial *p_material = mp_fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i);
        fbx_materials[i] = p_material;
    }

    m_meshes.clear();
    m_meshes.resize(meshcount);
    // 各メッシュを対象に、DirectXで使えるように変換
    for (size_t i = 0; i < fbx_meshes.size(); ++i) {
        // メッシュ取得
        FbxMesh *p_mesh = fbx_meshes[i];
        // マテリアル取得
        FbxSurfaceMaterial *p_material = fbx_materials[i];

        Vertex vertex = {};


        // 頂点バッファ取得
        FbxVector4 *p_vertices = p_mesh->GetControlPoints();

        // インデックスバッファ取得
        int *p_indices = p_mesh->GetPolygonVertices();

        // 法線リスト取得
        FbxArray<FbxVector4> normals;
        p_mesh->GetPolygonVertexNormals(normals);

        // UVSetの名前保存用
        FbxStringList uvset_names;
        // UVSetの名前リストを取得
        p_mesh->GetUVSetNames(uvset_names);
        // 入れ物作成
        FbxArray<FbxVector2> uvs;
        // UVSet取得
        p_mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uvs);

        // カラー取得
        FbxDouble3 color;
        FbxDouble factor;
        if (p_material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
            FbxProperty prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuse); // Diffuseプロパティを取得
            color = prop.Get<FbxDouble3>();

            prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor); // DiffuseFactor(重み)プロパティを取得
            factor = prop.Get<FbxDouble>();
        }

        // TODO: tangentの取得方法が判明したら処理を記述する

        // 頂点数取得
        int polygon_vertex_count = p_mesh->GetPolygonVertexCount();
        m_meshes[i].vertices.resize(polygon_vertex_count);

        // Mesh情報取得
        for (int j = 0; j < polygon_vertex_count; ++j) {
            // インデックスバッファから頂点番号を取得
            int index = p_indices[j];

            // 取得
            auto &position = p_vertices[index]; // 頂点
            auto normal = &normals[j];
            auto uv = &uvs[j];

            // DirectXで使えるように変換
            vertex.position = XMFLOAT3(-(static_cast<float>(position[0])), static_cast<float>(position[2]), -static_cast<float>(position[1]));
            vertex.normal = XMFLOAT3(-(static_cast<float>(*normals[0])), static_cast<float>(*normals[1]), -static_cast<float>(*normals[2]));
            vertex.uv = XMFLOAT2(static_cast<float>(*uvs[0]), (1.0f - static_cast<float>(*uvs[1])));
            vertex.color = XMFLOAT4(static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]), static_cast<float>(factor));

            // 追加
            m_meshes[i].vertices[j] = vertex;
        }

        // ポリゴン数の取得
        size_t polygon_count = p_mesh->GetPolygonCount();
        m_meshes[i].indices.resize(polygon_count * 3);
        // インデックス番号取得
        for (size_t j = 0; j < polygon_count; ++j) {
            m_meshes[i].indices[j * 3] = j * 3 + 2;
            m_meshes[i].indices[j * 3 + 1] = j * 3 + 1;
            m_meshes[i].indices[j * 3 + 2] = j * 3 + 0;
        }

        // テクスチャ取得
        FbxProperty prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuse); // Diffuseプロパティを取得
        FbxFileTexture *p_texture = nullptr;

        // テクスチャ読み込み
        int texture_count = prop.GetSrcObjectCount<FbxFileTexture>();
        if (texture_count > 0) {
            p_texture = prop.GetSrcObject<FbxFileTexture>(0);
        } else {
            // FbxLayeredTextureからFbxFiletextureを取得
            int layer_count = prop.GetSrcObjectCount<FbxLayeredTexture>();
            if (layer_count > 0) {
                p_texture = prop.GetSrcObject<FbxFileTexture>(0);
            }
        }

        // テクスチャパス取得
        if (p_texture != nullptr) {
            string file_path = p_texture->GetFileName();
            m_meshes[i].diffusemap = ToWideString(file_path);
        }
    }

    Destroy();

    return true;
}

/// <summary>
/// FbxManager、FbxImporter、FbxSceneを破棄
/// </summary>
void FbxLoader::Destroy() {
    //FbxImporter破棄
    if (mp_fbx_importer != nullptr) {
        mp_fbx_importer->Destroy();
    }

    // FbxSceneを破棄
    if (mp_fbx_scene != nullptr) {
        mp_fbx_scene->Destroy();
    }

    // FbxManagerを破棄
    if (mp_fbx_manager != nullptr) {
        mp_fbx_manager->Destroy();
    }
}

/// <summary>
/// 読み込んだfbxファイルを渡す
/// </summary>
/// <returns></returns>
vector<Mesh> FbxLoader::GetMeshes() {
    return m_meshes;
}

/// <summary>
/// 読み込んだfbxを保持していた変数をクリアする
/// </summary>
void FbxLoader::ClearMeshes() {
    m_meshes.clear();
}
