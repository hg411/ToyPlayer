#include "pch.h"
#include "Imgui.h"
#include "Engine.h"
#include "ToyPlayer.h"

// imgui_impl_win32.cpp�� ���ǵ� �޽��� ó�� �Լ��� ���� ���� ����
// VCPKG�� ���� IMGUI�� ����� ��� �����ٷ� ��� �� �� ����
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

Imgui::Imgui() {}

Imgui::~Imgui() {
    // CleanUp
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Imgui::Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context) {
    const WindowInfo &window = GEngine->GetWindowInfo();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(window.width), float(window.height));
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplDX11_Init(device.Get(), context.Get());
    ImGui_ImplWin32_Init(window.hwnd);
}

void Imgui::Update() {
    ImGui_ImplDX11_NewFrame(); // GUI ������ ����
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame(); // � �͵��� ������ ���� ��� ����
    ImGui::Begin("Scene Control");

    // ImGui�� �������ִ� Framerate ���
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    // TODO::
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        bool useToneMapping = static_cast<bool>(GEngine->GetToyPlayer()->GetUseToneMapping()); 
        ImGui::Checkbox("Use ToneMapping", &useToneMapping);
        if (useToneMapping)
            GEngine->GetToyPlayer()->SetUseToneMapping(1);
        else
            GEngine->GetToyPlayer()->SetUseToneMapping(0);

        ImGui::TreePop();
    }


    // ImGui ��ġ ����
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

    ImGui::End();
}

void Imgui::Render() {
    ImGui::Render();                                     // �������� �͵� ��� ��
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI ������
}
