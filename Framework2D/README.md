# 配置说明

## 文件说明

项目目录包含以下子目录：
- [third_party/](./third_party/)：第三方依赖库
    - [imgui/](./third_party/imgui/)：跨平台C++图形界面库 [Dear ImGui](https://github.com/ocornut/imgui)
    - [ImGuiFileDialog/](./third_party/ImGuiFileDialog/)：ImGui 的[文件窗口实现](https://github.com/aiekick/ImGuiFileDialog)
    - OpenGL 内核相关库：[glfw/](./third_party/glfw/)、[glad/](./third_party/glad/)；OpenGL（Open Graphics Library）是一个跨编程语言、跨平台的编程图形程序接口（API），它告诉硬件应该如何实现图形的绘制；而 glfw, glad 提供了 OpenGL 的窗口创建、函数调用等功能，方便人们去使用 OpenGL 来进行绘图操作。**OpenGL 在这里作为 ImGui 的渲染内核使用，绑定了 OpenGL 作为内核之后，ImGui 就可以在屏幕上绘制图形界面，我们暂时不需要关心其中的细节**
    - [stb_image.h](./third_party/stb_image.h)：图像解码库，用以从文件中读取图像
- [include/](./include/)：项目的头文件目录
    - [common/](./include/common/)：ImGui 的主要功能封装，UI 视图等
- [src/](./src/)：项目的源文件目录
    - [common/](./src/common/)：UI 视图的具体实现
    - [demo/](./src/demo/)：演示程序，简单的图像显示功能
    - [assignments/](./src/assignments/)：相关的实现。



WoS相关代码在 [Framework2D/src/assignments/Ex_WosDemo](./src/assignments/Ex_WosDemo)

