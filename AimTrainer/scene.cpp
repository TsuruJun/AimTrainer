#include "scene.h"
#include "engine.h"
#include "app.h"
#include <d3dx12.h>
#include "sharedstruct.h"
#include "vertexbuffer.h"
#include "constantbuffer.h"
#include "rootsignature.h"
#include "pipelinestate.h"
#include "indexbuffer.h"
#include "fbxloader.h"
#include <filesystem>
#include "descriptorheap.h"
#include "texture2D.h"
#include "enemybot.h"
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
ConstantBuffer *gp_constantbuffer[Engine::FRAME_BUFFER_COUNT];
IndexBuffer *gp_indexbuffer;
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;
DescriptorHeap *gp_descriptor_heap;
EnemyBot g_enemy_bot{};

float rotateX = 0.0f;
const char *gp_mode = "+";

const char *gp_model_file = "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\enemy_bot.fbx";
vector<Mesh> g_meshes; // ���b�V���̔z��
vector<VertexBuffer *> gp_vertex_buffers; // ���b�V���̐����̒��_�o�b�t�@
vector<IndexBuffer *> gp_index_buffers; // ���b�V���̐����̃C���f�b�N�X�o�b�t�@
vector<DescriptorHandle *> gp_material_handles; // �e�N�X�`���p�̃n���h���ꗗ

// �g���q��u�������鏈��
wstring ReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}

bool Scene::Init() {
    // EnemyBot�ǂݍ���
    g_enemy_bot.EnemyLoad(gp_model_file, g_meshes);

    // ���b�V���̐��������_�o�b�t�@��p�ӂ���
    gp_vertex_buffers.reserve(g_meshes.size());
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto size = sizeof(Vertex) * g_meshes[i].vertices.size();
        auto stride = sizeof(Vertex);
        auto vertices = g_meshes[i].vertices.data();
        auto p_vertex_buffer = new VertexBuffer(size, stride, vertices);
        if (!p_vertex_buffer->IsValid()) {
            printf("���_�o�b�t�@�̐����Ɏ��s\n");
            return false;
        }

        gp_vertex_buffers.emplace_back(p_vertex_buffer);
    }

    // ���b�V���̐������C���f�b�N�X�o�b�t�@��p�ӂ���
    gp_index_buffers.reserve(g_meshes.size());
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto size = sizeof(uint32_t) * g_meshes[i].indices.size();
        auto indices = g_meshes[i].indices.data();
        auto p_index_buffer = new IndexBuffer(size, indices);
        if (!p_index_buffer->IsValid()) {
            printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s");
            return false;
        }

        gp_index_buffers.emplace_back(p_index_buffer);
    }

    auto eyeposition = XMVectorSet(0.0f, 0.0, 10.0, 0.0f); // ���_�̈ʒu
    auto targetposition = XMVectorSet(0.0f, 0.0, 0.0, 0.0f);; // ���_����������W
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // �������\���x�N�g��
    constexpr auto fov = XMConvertToRadians(60); // ����p
    auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // �A�X�y�N�g��

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; ++i) {
        gp_constantbuffer[i] = new ConstantBuffer(sizeof(Transform));
        if (!gp_constantbuffer[i]->IsValid()) {
            printf("�ϊ��s��p�萔�o�b�t�@�̐����Ɏ��s");
            return false;
        }

        // �ϊ��s��̓o�^
        auto ptr = gp_constantbuffer[i]->GetPtr<Transform>();
        ptr->world = XMMatrixIdentity();
        ptr->view = XMMatrixLookAtRH(eyeposition, targetposition, upward);
        ptr->proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.f);
    }

    // �}�e���A���ǂݍ���
    gp_material_handles.clear();
    gp_descriptor_heap = new DescriptorHeap();

    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto texture_path = ReplaceExtension(g_meshes[i].diffusemap, "tga"); // // ���Ƃ��Ƃ�psd�ɂȂ��Ă��Ă�����Ƃ߂�ǂ������̂ŁA��������Ă���tga��ǂݍ���
        auto main_texture = Texture2D::Get(texture_path);
        auto handle = gp_descriptor_heap->Register(main_texture);
        gp_material_handles.emplace_back(handle);
    }

    gp_rootsignature = new RootSignature();
    if (!gp_rootsignature->IsValid()) {
        printf("���[�g�V�O�l�`���̐����Ɏ��s");
        return false;
    }

    gp_pipelinestate = new PipelineState();
    gp_pipelinestate->SetInputLayout(Vertex::inputlayout);
    gp_pipelinestate->SetRootSignature(gp_rootsignature->Get());
    gp_pipelinestate->SetVertexShader(L"../x64/Debug/simplevertexshader.cso");
    gp_pipelinestate->SetPixelShader(L"../x64/Debug/simplepixelshader.cso");
    gp_pipelinestate->Create();
    if (!gp_pipelinestate->IsValid()) {
        printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s");
        return false;
    }

    printf("�V�[���̏������ɐ���");
    return true;
}

void Scene::Update() {
    g_enemy_bot.InitXYZ(0.0f, 0.0f, -10.0f);

    // EnemyBot������������
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto currenttransform = gp_constantbuffer[currentindex]->GetPtr<Transform>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾
    g_enemy_bot.RoundTripX(0.0025f, 8.0f, currenttransform);
}

void Scene::Draw() {
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandlist = gp_engine->CommandList(); // �R�}���h���X�g
    auto material_heap = gp_descriptor_heap->GetHeap(); // �f�B�X�N���v�^�q�[�v

    // ���b�V���̐������C���f�b�N�X���̕`����������s����������
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto vertexbufferview = gp_vertex_buffers[i]->View(); // ���_�o�b�t�@�r���[
        auto indexbufferview = gp_index_buffers[i]->View(); // �C���f�b�N�X�o�b�t�@�r���[

        commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ���[�g�V�O�l�`�����Z�b�g
        commandlist->SetPipelineState(gp_pipelinestate->Get()); // �p�C�v���C���X�e�[�g���Z�b�g
        commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffer[currentindex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

        commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`��`�悷��ݒ�ɂ���
        commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // ���_�o�b�t�@���X���b�g0�Ԃ��g����1�����ݒ肷��
        commandlist->IASetIndexBuffer(&indexbufferview); // �C���f�b�N�X�o�b�t�@���Z�b�g����

        commandlist->SetDescriptorHeaps(1, &material_heap); // �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
        commandlist->SetGraphicsRootDescriptorTable(1, gp_material_handles[i]->m_handle_GPU); // ���̃��b�V���ɑΉ�����f�B�X�N���v�^�e�[�u�����Z�b�g

        commandlist->DrawIndexedInstanced(g_meshes[i].indices.size(), 1, 0, 0, 0); // �C���f�b�N�X�̐�����`�悷��
    }
}
