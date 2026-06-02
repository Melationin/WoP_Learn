#pragma once
#include <glad/glad.h>
#include <imgui.h>
#include <vector>
#include <cmath>
#include <algorithm>
namespace USTC_CG {
class PDEWidget {
public:
    PDEWidget(int w, int h)
        : width(w), height(h)
    {
        ref.resize(w * h);
        sol.resize(w * h);
        err.resize(w * h);

        createTexture(refTex);
        createTexture(solTex);
        createTexture(errTex);
    }

    PDEWidget() = default;

    ~PDEWidget()
    {
        glDeleteTextures(1, &refTex);
        glDeleteTextures(1, &solTex);
        glDeleteTextures(1, &errTex);
    }

    // ====== 数据输入 ======
    void setReference(const std::vector<float>& data)
    {
        ref = data;
        updateTexture(ref, refTex);
    }

    void setSolution(const std::vector<float>& data)
    {
        sol = data;
        computeError();
        float maxv = *std::max_element(data.begin(), data.end());
        float minv = *std::min_element(data.begin(), data.end());
        updateTexture(sol, solTex, maxv, minv);
        updateTexture(err, errTex, (maxv - minv)/5, 0);
    }

    // ====== 绘制 ======
    void draw()
    {


        //io.DisplayFramebufferScale = ImVec2(2.0f, 2.0f);
        ImGui::SetNextWindowSize(ImVec2(width *2+ 200, height*2 + 200));
        ImGui::Begin("PDE Viewer");

        ImGui::Text("Error L2: %.6f", l2_error);
        ImGui::Text("Error Linf: %.6f", linf_error);

        ImGui::Separator();

        ImGui::RadioButton("Reference", &mode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Solution", &mode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Error", &mode, 2);

        GLuint tex = (mode == 0 ? refTex :
                      mode == 1 ? solTex : errTex);

        ImGui::Image((void*)(intptr_t)tex,
                     ImVec2((float)width*2, (float)height*2));

        ImGui::End();
    }

private:
    int width{}, height{};

    std::vector<float> ref, sol, err;

    GLuint refTex = 0, solTex = 0, errTex = 0;

    int mode = 1;

    float l2_error = 0.0f;
    float linf_error = 0.0f;

    // ====== OpenGL ======
    void createTexture(GLuint& tex) const
    {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // ====== 误差计算 ======
    void computeError()
    {
        l2_error = 0.0f;
        linf_error = 0.0f;

        for (int i = 0; i < width * height; i++)
        {
            err[i] = std::abs(sol[i] - ref[i]);

            l2_error += err[i] * err[i];
            linf_error = std::max(linf_error, err[i]);
        }

        l2_error = std::sqrt(l2_error / (width * height));
    }

    // ====== 映射到图像 ======
    void updateTexture(const std::vector<float>& data, GLuint tex) const
    {
        float maxv = *std::max_element(data.begin(), data.end());
        float minv = *std::min_element(data.begin(), data.end());

        std::vector<unsigned char> img(width * height * 3);

        for (int i = 0; i < width * height; i++)
        {
            float t = (data[i] - minv) / (maxv - minv + 1e-8f);

            float r, g, b;

            colormap_turbo(t, r, g, b);

            img[i * 3 + 0] = (unsigned char)(r * 255);
            img[i * 3 + 1] = (unsigned char)(g * 255);
            img[i * 3 + 2] = (unsigned char)(b * 255);
        }

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        0, 0, width, height,
                        GL_RGB, GL_UNSIGNED_BYTE,
                        img.data());
    }
    void updateTexture(const std::vector<float>& data,GLuint tex, float maxv, float minv) const
    {
        std::vector<unsigned char> img(width * height * 3);

        for (int i = 0; i < width * height; i++)
        {
            float t = (data[i] - minv) / (maxv - minv + 1e-8f);

            float r, g, b;

            colormap_turbo(t, r, g, b);

            img[i * 3 + 0] = (unsigned char)(r * 255);
            img[i * 3 + 1] = (unsigned char)(g * 255);
            img[i * 3 + 2] = (unsigned char)(b * 255);
        }

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        0, 0, width, height,
                        GL_RGB, GL_UNSIGNED_BYTE,
                        img.data());
    }

    static void colormap_turbo(float t, float& r, float& g, float& b)
    {
        r = g = b = std::clamp(t, 0.0f, 1.0f);
        //return;
        const float a[] = {0.1357f, 4.6154f, -42.6603f, 132.1311f, -152.9424f, 59.2864f};
        const float b_[] = {0.0914f, 2.1942f, 4.8429f, -14.1850f, 4.2773f, 2.8296f};
        const float c[] = {0.1067f, 12.6419f, -60.5820f, 110.3627f, -89.9031f, 27.3482f};

        r = std::clamp(a[0] + t*(a[1] + t*(a[2] + t*(a[3] + t*(a[4] + t*a[5])))), 0.0f, 1.0f);
        g = std::clamp(b_[0] + t*(b_[1] + t*(b_[2] + t*(b_[3] + t*(b_[4] + t*b_[5])))), 0.0f, 1.0f);
        b = std::clamp(c[0] + t*(c[1] + t*(c[2] + t*(c[3] + t*(c[4] + t*c[5])))), 0.0f, 1.0f);
    }
};
}