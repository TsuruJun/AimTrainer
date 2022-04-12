#include "fbxloader.h"
#include "sharedstruct.h"
#include <d3dx12.h>
#include <DirectXMath.h>

using namespace fbxsdk;
using namespace std;
using namespace DirectX;

// string(�}���`�o�C�g������)����wstring(���C�h������)�𓾂�
wstring ToWideString(const string &str) {
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

/// <summary>
/// Fbx�t�@�C�������[�h
/// </summary>
/// <param name="p_filename">fbx�t�@�C����</param>
/// <returns>�����Ftrue ���s�Ffalse</returns>
bool FbxLoader::FbxLoad(const char *file_name) {
    // FbxManeger�쐬
    mp_fbx_manager = FbxManager::Create();
    if (mp_fbx_manager == nullptr) {
        printf("Fbx�}�l�[�W���[�̍쐬�Ɏ��s");

        return false;
    }

    // FbxImporter�쐬
    mp_fbx_importer = FbxImporter::Create(mp_fbx_manager, "FbxImporter");
    if (mp_fbx_importer == nullptr) {
        printf("Fbx�C���|�[�^�[�̍쐬�Ɏ��s");
        Destroy();

        return false;
    }

    // FbxScene�쐬
    mp_fbx_scene = FbxScene::Create(mp_fbx_manager, "FbxScene");
    if (mp_fbx_scene == nullptr) {
        printf("Fbx�V�[���̍쐬�Ɏ��s");
        Destroy();

        return false;
    }

    // �t�@�C��������
    if (mp_fbx_importer->Initialize(file_name) == false) {
        printf("�t�@�C���̏������Ɏ��s");
        Destroy();

        return false;
    }

    // �t�@�C���C���|�[�g
    if (mp_fbx_importer->Import(mp_fbx_scene) == false) {
        printf("�t�@�C���̃C���|�[�g�Ɏ��s");
        Destroy();

        return false;
    }

    // �|���S�����O�p�`�ɂ���
    FbxGeometryConverter converter(mp_fbx_manager);
    converter.Triangulate(mp_fbx_scene, true);

    // Mesh�擾
    int meshcount = mp_fbx_scene->GetSrcObjectCount<FbxMesh>();
    vector<FbxMesh *>fbx_meshes; // Fbx�t�@�C��������o����Mesh�̊i�[�ꏊ
    fbx_meshes.resize(meshcount);

    // Material�擾
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
    // �e���b�V����ΏۂɁADirectX�Ŏg����悤�ɕϊ�
    for (size_t i = 0; i < fbx_meshes.size(); ++i) {
        // ���b�V���擾
        FbxMesh *p_mesh = fbx_meshes[i];
        // �}�e���A���擾
        FbxSurfaceMaterial *p_material = fbx_materials[i];

        Vertex vertex = {};


        // ���_�o�b�t�@�擾
        FbxVector4 *p_vertices = p_mesh->GetControlPoints();

        // �C���f�b�N�X�o�b�t�@�擾
        int *p_indices = p_mesh->GetPolygonVertices();

        // �@�����X�g�擾
        FbxArray<FbxVector4> normals;
        p_mesh->GetPolygonVertexNormals(normals);

        // UVSet�̖��O�ۑ��p
        FbxStringList uvset_names;
        // UVSet�̖��O���X�g���擾
        p_mesh->GetUVSetNames(uvset_names);
        // ���ꕨ�쐬
        FbxArray<FbxVector2> uvs;
        // UVSet�擾
        p_mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uvs);

        // �J���[�擾
        FbxDouble3 color;
        FbxDouble factor;
        if (p_material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
            FbxProperty prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuse); // Diffuse�v���p�e�B���擾
            color = prop.Get<FbxDouble3>();

            prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor); // DiffuseFactor(�d��)�v���p�e�B���擾
            factor = prop.Get<FbxDouble>();
        }

        // TODO: tangent�̎擾���@�����������珈�����L�q����

        // ���_���擾
        int polygon_vertex_count = p_mesh->GetPolygonVertexCount();
        m_meshes[i].vertices.resize(polygon_vertex_count);

        // Mesh���擾
        for (int j = 0; j < polygon_vertex_count; ++j) {
            // �C���f�b�N�X�o�b�t�@���璸�_�ԍ����擾
            int index = p_indices[j];

            // �擾
            auto &position = p_vertices[index]; // ���_
            auto normal = &normals[j];
            auto uv = &uvs[j];

            // DirectX�Ŏg����悤�ɕϊ�
            vertex.position = XMFLOAT3(-(static_cast<float>(position[0])), static_cast<float>(position[2]), -static_cast<float>(position[1]));
            vertex.normal = XMFLOAT3(-(static_cast<float>(*normals[0])), static_cast<float>(*normals[1]), -static_cast<float>(*normals[2]));
            vertex.uv = XMFLOAT2(static_cast<float>(*uvs[0]), (1.0f - static_cast<float>(*uvs[1])));
            vertex.color = XMFLOAT4(static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]), static_cast<float>(factor));

            // �ǉ�
            m_meshes[i].vertices[j] = vertex;
        }

        // �|���S�����̎擾
        size_t polygon_count = p_mesh->GetPolygonCount();
        m_meshes[i].indices.resize(polygon_count * 3);
        // �C���f�b�N�X�ԍ��擾
        for (size_t j = 0; j < polygon_count; ++j) {
            m_meshes[i].indices[j * 3] = j * 3 + 2;
            m_meshes[i].indices[j * 3 + 1] = j * 3 + 1;
            m_meshes[i].indices[j * 3 + 2] = j * 3 + 0;
        }

        // �e�N�X�`���擾
        FbxProperty prop = p_material->FindProperty(FbxSurfaceMaterial::sDiffuse); // Diffuse�v���p�e�B���擾
        FbxFileTexture *p_texture = nullptr;

        // �e�N�X�`���ǂݍ���
        int texture_count = prop.GetSrcObjectCount<FbxFileTexture>();
        if (texture_count > 0) {
            p_texture = prop.GetSrcObject<FbxFileTexture>(0);
        } else {
            // FbxLayeredTexture����FbxFiletexture���擾
            int layer_count = prop.GetSrcObjectCount<FbxLayeredTexture>();
            if (layer_count > 0) {
                p_texture = prop.GetSrcObject<FbxFileTexture>(0);
            }
        }

        // �e�N�X�`���p�X�擾
        if (p_texture != nullptr) {
            string file_path = p_texture->GetFileName();
            m_meshes[i].diffusemap = ToWideString(file_path);
        }
    }

    Destroy();

    return true;
}

/// <summary>
/// FbxManager�AFbxImporter�AFbxScene��j��
/// </summary>
void FbxLoader::Destroy() {
    //FbxImporter�j��
    if (mp_fbx_importer != nullptr) {
        mp_fbx_importer->Destroy();
    }

    // FbxScene��j��
    if (mp_fbx_scene != nullptr) {
        mp_fbx_scene->Destroy();
    }

    // FbxManager��j��
    if (mp_fbx_manager != nullptr) {
        mp_fbx_manager->Destroy();
    }
}

/// <summary>
/// �ǂݍ���fbx�t�@�C����n��
/// </summary>
/// <returns></returns>
vector<Mesh> FbxLoader::GetMeshes() {
    return m_meshes;
}

/// <summary>
/// �ǂݍ���fbx��ێ����Ă����ϐ����N���A����
/// </summary>
void FbxLoader::ClearMeshes() {
    m_meshes.clear();
}
