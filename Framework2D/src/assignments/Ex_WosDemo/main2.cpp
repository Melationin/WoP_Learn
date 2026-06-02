#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "util/nvg_util.h"
#include
void errorcb(int error, const char* desc)
{
    printf("GLFW error %d: %s\n", error, desc);
}
int main() {


    if (!glfwInit()) {
        fprintf(stderr, "无法初始化 GLFW\n");
        return EXIT_FAILURE;
    }


    glfwSetErrorCallback([](int error, const char* desc) {
        fprintf(stderr, "GLFW Error (%d): %s\n", error, desc);
    });


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Window", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "创建窗口失败\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }


    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "无法初始化GLEW\n");
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window, [](auto window, int width, int height) {
        glViewport(0, 0, width, height);

   // 如果需要，同时更新投影矩阵（例如正交或透视相机）
   // 下面以正交投影为例，让物体比例不随窗口宽高比改变而拉伸
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // 保持左下角为原点，宽高跟随窗口，但内容会被拉伸
   // 若要保持逻辑坐标系不变，可以固定高度或宽度，根据宽高比调整
   // 例如：固定高度 10，宽度随窗口比例变化
   float aspect = (float)width / (float)height;
   float left = -5.0f * aspect;
   float right = 5.0f * aspect;
   glOrtho(left, right, -5.0f, 5.0f, -1.0f, 1.0f);
   glMatrixMode(GL_MODELVIEW);
    });

    NVG vg{NVG_ANTIALIAS | NVG_STENCIL_STROKES};
    if (vg.vg ==  NULL) {
        printf("Could not init nanovg.\n");
        return -1;
    }

    glfwSwapInterval(1);


    int t = 0;
    int f1 = vg.CreateFont("sans", "C:\\Windows\\Fonts\\NotoSansSC-VF.ttf");

    vg.BeginFrame(800, 600, 1);
    vg.FontFaceId(f1);
    vg.FontSize(18);
    vg.TextLineHeight(4);
    vg.FillColor({0,0,0,255});
    std::deque<std::tuple<double,double,double,double,int>> points;


    double now_x = 400, now_y = 0;
    long long n = 0;
    while (!glfwWindowShouldClose(window)) {
        double s = t*0.01;
        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        double mx, my, t, dt;
        glfwGetCursorPos(window, &mx, &my);
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);

        mx = mx/fw*800;
        my = my/fh*600;

        if (mx < 0) mx = 0;
        if (mx > 799) mx = 799;
        if (my < 0) my = 0;
        if (my > 599) my = 599;
        now_x = mx;
        now_y = my;

        while (1){
            double dist = std::min(now_x, 800 - now_x);


            dist = std::min(dist, now_y);
            dist = std::min(dist, 600 - now_y);
            if (dist < 1e-2) break;
            else
            {
                double d = std::rand()/(double)RAND_MAX*2*3.1415926;
                double r = dist;
                now_x += std::cos(d)*r;
                now_y += std::sin(d)*r;
                points.emplace_front(now_x - std::cos(d)*r, now_y- std::sin(d)*r, now_x , now_y, n);
                if (points.size() > 1000) points.pop_back();
                //std::cout<<"now.x = "<<now_x<<" now.y = "<<now_y<<" dist = "<<dist<<std::endl;
            }
        }
        n++;
        //return 0;

        int a = 0;
        long N = std::get<4>(points.front()) - std::get<4>(points.back());
        double argCount = points.size()*1.0/N;
        for (auto [x,y,x2,y2,n] : points)
        {
            double alpha = 1.0 - (double)a/points.size();
            vg.BeginPath();
            vg.MoveTo(x, y);
            vg.LineTo(x2, y2);
            double r = std::sqrt((x2-x)*(x2-x) + (y2-y)*(y2-y));
            //vg.Circle(x, y, r);
            vg.StrokeColor(NVG::RGBA(128,128,128, alpha*255));
            vg.StrokeWidth(1);
            vg.Stroke();

            a ++;
        }
        vg.Text(mx,my, std::to_string(argCount).c_str());

        vg.EndFrame();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
