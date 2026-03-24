//
// Created by zhdds on 2026/3/24.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_PDEWINDOWS_H
#define USTC_CG_2025_FRAMEWORK2D_PDEWINDOWS_H
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include  "Reference_widget.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace USTC_CG {
class PDEWindows {

    std::unique_ptr<PDEWidget> widget_{};
    GLFWwindow* window = nullptr;
public:

    PDEWindows(int w,int h){};
    ~PDEWindows()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void init(int w,int h)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        window  = glfwCreateWindow(1000, 800, "PDE Viewer", nullptr, nullptr);
        glfwMakeContextCurrent(window);

        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        // ===== 2. 初始化 ImGui =====
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();  // 清除默认字体

        ImFontConfig cfg;
        cfg.SizePixels = 20.0f;  // 调整成你想要的字号
        io.Fonts->AddFontDefault(&cfg);

        widget_ = std::make_unique<PDEWidget>(w,h);
    }

    void set_reference(const std::vector<float>& data) const
    {
        widget_->setReference(data);
    }
    void set_solution(const std::vector<float>& data) const
    {
        widget_->setSolution(data);
    }

    void run()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            // ImGui begin
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // ⭐ 核心：画你的 PDE Widget
            widget_->draw();

            // ImGui end
            ImGui::Render();

            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glViewport(0, 0, w, h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_PDEWINDOWS_H