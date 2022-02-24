#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "comptr.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Engine {
public:
    enum {
        FRAME_BUFFER_COUNT = 2 // �_�u���o�b�t�@�����O����̂�2
    };

public:
    bool Init(HWND hwnd, UINT window_width, UINT window_height); // �G���W��������

    void BeginRender(); // �`��̊J�n����
    void EndRender(); // �`��̏I������

public:
    // �O����A�N�Z�X�������̂�Getter�Ƃ��Č��J�������
    ID3D12Device6 *Device();
    ID3D12GraphicsCommandList *CommandList();
    UINT CurrentBackBufferIndex();

private:
    // DirextX12�������Ɏg���֐�����
    bool CreateDevice(); // �f�o�C�X�𐶐�
    bool CreateCommandQueue(); // �R�}���h�L���[�𐶐�
    bool CreateSwapChain(); // �X���b�v�`�F�C���𐶐�
    bool CreateCommandList(); // �R�}���h���X�g�ƃR�}���h�A���P�[�^�𐶐�
    bool CreateFence(); // �t�F���X�𐶐�
    void CreateViewPort(); // �r���[�|�[�g�𐶐�
    void CreateScissorRect(); //�V�U�[�Z�`�𐶐�

private:
    // �`��Ɏg��DirectX12�̃I�u�W�F�N�g����
    HWND m_hwnd;
    UINT m_framebuffer_width = 0;
    UINT m_framebuffer_height = 0;
    UINT m_currentbackbuffer_index = 0;

    ComPtr<ID3D12Device6> mp_device = nullptr; // �f�o�C�X
    ComPtr<ID3D12CommandQueue> mp_queue = nullptr; // �R�}���h�L���[
    ComPtr<IDXGISwapChain3> mp_swapchain = nullptr; // �X���b�v�`�F�C��
    ComPtr<ID3D12CommandAllocator> mp_allocator[FRAME_BUFFER_COUNT] = {nullptr}; // �R�}���h�A���P�[�^
    ComPtr<ID3D12GraphicsCommandList> mp_commandlist = nullptr; // �R�}���h���X�g
    HANDLE m_fenceevent = nullptr; // �t�F���X�Ŏg���C�x���g
    ComPtr<ID3D12Fence> mp_fence = nullptr; // �t�F���X
    UINT64 m_fencevalue[FRAME_BUFFER_COUNT]; // �t�F���X�̒l(�_�u���o�b�t�@�����O�p��2��)
    D3D12_VIEWPORT m_viewport; // �r���[�|�[�g
    D3D12_RECT m_scissor; // �V�U�[�Z�`

private:
    // �`��Ɏg���I�u�W�F�N�g�Ƃ��̐����֐�����
    bool CreateRenderTarget(); // �����_�[�^�[�Q�b�g�𐶐�
    bool CreateDepthStencil(); // �[�x�X�e���V���o�b�t�@�𐶐�

    UINT m_rtvdescriptor_size = 0; // �����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�T�C�Y
    ComPtr<ID3D12DescriptorHeap> mp_rtvheap = nullptr; // �����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12Resource> mp_renderTargets[FRAME_BUFFER_COUNT] = {nullptr}; // �����_�[�^�[�Q�b�g(�_�u���o�b�t�@�����O����̂�2��)

    UINT m_dsvdescriptor_size = 0; // �[�x�X�e���V���̃f�B�X�N���v�^�T�C�Y
    ComPtr<ID3D12DescriptorHeap> mp_dsvheap = nullptr; // �[�x�X�e���V���̃f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12Resource> mp_depthstencilbuffer = nullptr; // �[�x�X�e���V���o�b�t�@

private:
    // �`�惋�[�v�Ŏg�p�������
    ID3D12Resource *mp_currentrendertarget = nullptr; // ���݂̃t���[���̃����_�[�^�[�Q�b�g���ꎞ�I�ɕۑ�
    void WaitRender(); // �`�抮����҂���
};

extern Engine *gp_engine; // �ǂ�����ł��Q�Ƃ������̂ŃO���[�o���ɂ���