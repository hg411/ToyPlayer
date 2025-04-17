#include "pch.h"
#include "Imgui.h"
#include "Engine.h"
#include "ToyPlayer.h"

// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// VCPKG를 통해 IMGUI를 사용할 경우 빨간줄로 경고가 뜰 수 있음
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
    ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
    ImGui::Begin("Scene Control");

    // ImGui가 측정해주는 Framerate 출력
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


    // ImGui 위치 고정
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

    ImGui::End();
}

void Imgui::Render() {
    ImGui::Render();                                     // 렌더링할 것들 기록 끝
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링
}
