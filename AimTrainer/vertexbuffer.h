#pragma once
#include <d3d12.h>
#include "comptr.h"

class VertexBuffer {
public:
    VertexBuffer(size_t size, size_t stride, const void *p_initdata); // �R���X�g���N�^�Ńo�b�t�@���쐬
    D3D12_VERTEX_BUFFER_VIEW View() const; // ���_�o�b�t�@�r���[���擾
    bool IsValid(); // �o�b�t�@�̐����ɐ������������擾

private:
    bool m_isvalid = false; // �o�b�t�@�̐����ɐ������������擾
    ComPtr<ID3D12Resource> mp_buffer = nullptr; // �o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW m_view = {}; // ���_�o�b�t�@�r���[

    VertexBuffer(const VertexBuffer &) = delete;
    void operator=(const VertexBuffer &) = delete;
};