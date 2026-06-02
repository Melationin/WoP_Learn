//
// Created by zhdds on 2026/5/28.
// 封装 NanoVG 常用 API
//

#ifndef WOSINGPU_NVG_UTIL_H
#define WOSINGPU_NVG_UTIL_H

#include <glad/glad.h>
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg.h"
#include "nanovg/src/nanovg_gl.h"

class NVG {

    bool owner;   // 是否拥有 vg 指针（用于析构）

public:
    NVGcontext* vg;
    // 构造：接管外部传入的 NVGcontext（不负责释放）
    explicit NVG(NVGcontext* ctx) : vg(ctx), owner(false) {}

    // 构造：自动创建 GL3 后端上下文（flags 为 nvgCreateGL3 的参数，如 NVG_ANTIALIAS 等）
    explicit NVG(int flags = NVG_ANTIALIAS | NVG_STENCIL_STROKES) : owner(true) {
        vg = nvgCreateGL3(flags);
    }

    // 析构：如果是自己创建的上下文，则释放
    ~NVG() {
        if (owner && vg) {
            nvgDeleteGL3(vg);
        }
    }

    // 禁止拷贝，允许移动（简化处理：禁止拷贝）
    NVG(const NVG&) = delete;
    NVG& operator=(const NVG&) = delete;

    // 获取原始 NVGcontext*，便于直接调用 C API
    NVGcontext* raw() const { return vg; }

    // ========== 帧控制 ==========
    void BeginFrame(float windowWidth, float windowHeight, float devicePixelRatio) {
        nvgBeginFrame(vg, windowWidth, windowHeight, devicePixelRatio);
    }
    void CancelFrame() { nvgCancelFrame(vg); }
    void EndFrame() { nvgEndFrame(vg); }

    // ========== 混合模式 ==========
    void GlobalCompositeOperation(int op) { nvgGlobalCompositeOperation(vg, op); }
    void GlobalCompositeBlendFunc(int sfactor, int dfactor) { nvgGlobalCompositeBlendFunc(vg, sfactor, dfactor); }
    void GlobalCompositeBlendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha) {
        nvgGlobalCompositeBlendFuncSeparate(vg, srcRGB, dstRGB, srcAlpha, dstAlpha);
    }

    // ========== 颜色工具（静态方法，直接返回 NVGcolor） ==========
    static NVGcolor RGB(unsigned char r, unsigned char g, unsigned char b) { return nvgRGB(r, g, b); }
    static NVGcolor RGBf(float r, float g, float b) { return nvgRGBf(r, g, b); }
    static NVGcolor RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { return nvgRGBA(r, g, b, a); }
    static NVGcolor RGBAf(float r, float g, float b, float a) { return nvgRGBAf(r, g, b, a); }
    static NVGcolor LerpRGBA(NVGcolor c0, NVGcolor c1, float u) { return nvgLerpRGBA(c0, c1, u); }
    static NVGcolor TransRGBA(NVGcolor c0, unsigned char a) { return nvgTransRGBA(c0, a); }
    static NVGcolor TransRGBAf(NVGcolor c0, float a) { return nvgTransRGBAf(c0, a); }
    static NVGcolor HSL(float h, float s, float l) { return nvgHSL(h, s, l); }
    static NVGcolor HSLA(float h, float s, float l, unsigned char a) { return nvgHSLA(h, s, l, a); }

    // ========== 状态管理 ==========
    void Save() { nvgSave(vg); }
    void Restore() { nvgRestore(vg); }
    void Reset() { nvgReset(vg); }
    void ShapeAntiAlias(int enabled) { nvgShapeAntiAlias(vg, enabled); }
    void StrokeColor(NVGcolor color) { nvgStrokeColor(vg, color); }
    void StrokePaint(NVGpaint paint) { nvgStrokePaint(vg, paint); }
    void FillColor(NVGcolor color) { nvgFillColor(vg, color); }
    void FillPaint(NVGpaint paint) { nvgFillPaint(vg, paint); }
    void MiterLimit(float limit) { nvgMiterLimit(vg, limit); }
    void StrokeWidth(float size) { nvgStrokeWidth(vg, size); }
    void LineCap(int cap) { nvgLineCap(vg, cap); }
    void LineJoin(int join) { nvgLineJoin(vg, join); }
    void GlobalAlpha(float alpha) { nvgGlobalAlpha(vg, alpha); }

    // ========== 变换 ==========
    void ResetTransform() { nvgResetTransform(vg); }
    void Transform(float a, float b, float c, float d, float e, float f) { nvgTransform(vg, a, b, c, d, e, f); }
    void Translate(float x, float y) { nvgTranslate(vg, x, y); }
    void Rotate(float angle) { nvgRotate(vg, angle); }
    void SkewX(float angle) { nvgSkewX(vg, angle); }
    void SkewY(float angle) { nvgSkewY(vg, angle); }
    void Scale(float x, float y) { nvgScale(vg, x, y); }
    void CurrentTransform(float* xform) { nvgCurrentTransform(vg, xform); }

    // 静态辅助变换函数（不依赖上下文）
    static void TransformIdentity(float* dst) { nvgTransformIdentity(dst); }
    static void TransformTranslate(float* dst, float tx, float ty) { nvgTransformTranslate(dst, tx, ty); }
    static void TransformScale(float* dst, float sx, float sy) { nvgTransformScale(dst, sx, sy); }
    static void TransformRotate(float* dst, float a) { nvgTransformRotate(dst, a); }
    static void TransformSkewX(float* dst, float a) { nvgTransformSkewX(dst, a); }
    static void TransformSkewY(float* dst, float a) { nvgTransformSkewY(dst, a); }
    static void TransformMultiply(float* dst, const float* src) { nvgTransformMultiply(dst, src); }
    static void TransformPremultiply(float* dst, const float* src) { nvgTransformPremultiply(dst, src); }
    static int TransformInverse(float* dst, const float* src) { return nvgTransformInverse(dst, src); }
    static void TransformPoint(float* dstx, float* dsty, const float* xform, float srcx, float srcy) {
        nvgTransformPoint(dstx, dsty, xform, srcx, srcy);
    }
    static float DegToRad(float deg) { return nvgDegToRad(deg); }
    static float RadToDeg(float rad) { return nvgRadToDeg(rad); }

    // ========== 图像 ==========
    int CreateImage(const char* filename, int imageFlags) { return nvgCreateImage(vg, filename, imageFlags); }
    int CreateImageMem(int imageFlags, unsigned char* data, int ndata) { return nvgCreateImageMem(vg, imageFlags, data, ndata); }
    int CreateImageRGBA(int w, int h, int imageFlags, const unsigned char* data) { return nvgCreateImageRGBA(vg, w, h, imageFlags, data); }
    void UpdateImage(int image, const unsigned char* data) { nvgUpdateImage(vg, image, data); }
    void ImageSize(int image, int* w, int* h) { nvgImageSize(vg, image, w, h); }
    void DeleteImage(int image) { nvgDeleteImage(vg, image); }

    // ========== 渐变与图案 ==========
    NVGpaint LinearGradient(float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol) {
        return nvgLinearGradient(vg, sx, sy, ex, ey, icol, ocol);
    }
    NVGpaint BoxGradient(float x, float y, float w, float h, float r, float f, NVGcolor icol, NVGcolor ocol) {
        return nvgBoxGradient(vg, x, y, w, h, r, f, icol, ocol);
    }
    NVGpaint RadialGradient(float cx, float cy, float inr, float outr, NVGcolor icol, NVGcolor ocol) {
        return nvgRadialGradient(vg, cx, cy, inr, outr, icol, ocol);
    }
    NVGpaint ImagePattern(float ox, float oy, float ex, float ey, float angle, int image, float alpha) {
        return nvgImagePattern(vg, ox, oy, ex, ey, angle, image, alpha);
    }

    // ========== 剪裁 ==========
    void Scissor(float x, float y, float w, float h) { nvgScissor(vg, x, y, w, h); }
    void IntersectScissor(float x, float y, float w, float h) { nvgIntersectScissor(vg, x, y, w, h); }
    void ResetScissor() { nvgResetScissor(vg); }

    // ========== 路径 ==========
    void BeginPath() { nvgBeginPath(vg); }
    void MoveTo(float x, float y) { nvgMoveTo(vg, x, y); }
    void LineTo(float x, float y) { nvgLineTo(vg, x, y); }
    void BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) {
        nvgBezierTo(vg, c1x, c1y, c2x, c2y, x, y);
    }
    void QuadTo(float cx, float cy, float x, float y) { nvgQuadTo(vg, cx, cy, x, y); }
    void ArcTo(float x1, float y1, float x2, float y2, float radius) { nvgArcTo(vg, x1, y1, x2, y2, radius); }
    void ClosePath() { nvgClosePath(vg); }
    void PathWinding(int dir) { nvgPathWinding(vg, dir); }
    void Arc(float cx, float cy, float r, float a0, float a1, int dir) { nvgArc(vg, cx, cy, r, a0, a1, dir); }
    void Rect(float x, float y, float w, float h) { nvgRect(vg, x, y, w, h); }
    void RoundedRect(float x, float y, float w, float h, float r) { nvgRoundedRect(vg, x, y, w, h, r); }
    void RoundedRectVarying(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft) {
        nvgRoundedRectVarying(vg, x, y, w, h, radTopLeft, radTopRight, radBottomRight, radBottomLeft);
    }
    void Ellipse(float cx, float cy, float rx, float ry) { nvgEllipse(vg, cx, cy, rx, ry); }
    void Circle(float cx, float cy, float r) { nvgCircle(vg, cx, cy, r); }
    void Fill() { nvgFill(vg); }
    void Stroke() { nvgStroke(vg); }

    // ========== 文本与字体 ==========
    int CreateFont(const char* name, const char* filename) { return nvgCreateFont(vg, name, filename); }
    int CreateFontAtIndex(const char* name, const char* filename, int fontIndex) {
        return nvgCreateFontAtIndex(vg, name, filename, fontIndex);
    }
    int CreateFontMem(const char* name, unsigned char* data, int ndata, int freeData) {
        return nvgCreateFontMem(vg, name, data, ndata, freeData);
    }
    int CreateFontMemAtIndex(const char* name, unsigned char* data, int ndata, int freeData, int fontIndex) {
        return nvgCreateFontMemAtIndex(vg, name, data, ndata, freeData, fontIndex);
    }
    int FindFont(const char* name) { return nvgFindFont(vg, name); }
    int AddFallbackFontId(int baseFont, int fallbackFont) { return nvgAddFallbackFontId(vg, baseFont, fallbackFont); }
    int AddFallbackFont(const char* baseFont, const char* fallbackFont) {
        return nvgAddFallbackFont(vg, baseFont, fallbackFont);
    }
    void ResetFallbackFontsId(int baseFont) { nvgResetFallbackFontsId(vg, baseFont); }
    void ResetFallbackFonts(const char* baseFont) { nvgResetFallbackFonts(vg, baseFont); }
    void FontSize(float size) { nvgFontSize(vg, size); }
    void FontBlur(float blur) { nvgFontBlur(vg, blur); }
    void TextLetterSpacing(float spacing) { nvgTextLetterSpacing(vg, spacing); }
    void TextLineHeight(float lineHeight) { nvgTextLineHeight(vg, lineHeight); }
    void TextAlign(int align) { nvgTextAlign(vg, align); }
    void FontFaceId(int font) { nvgFontFaceId(vg, font); }
    void FontFace(const char* font) { nvgFontFace(vg, font); }
    float Text(float x, float y, const char* string, const char* end = nullptr) {
        return nvgText(vg, x, y, string, end);
    }
    void TextBox(float x, float y, float breakRowWidth, const char* string, const char* end = nullptr) {
        nvgTextBox(vg, x, y, breakRowWidth, string, end);
    }
    float TextBounds(float x, float y, const char* string, const char* end, float* bounds) {
        return nvgTextBounds(vg, x, y, string, end, bounds);
    }
    void TextBoxBounds(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds) {
        nvgTextBoxBounds(vg, x, y, breakRowWidth, string, end, bounds);
    }
    int TextGlyphPositions(float x, float y, const char* string, const char* end, NVGglyphPosition* positions, int maxPositions) {
        return nvgTextGlyphPositions(vg, x, y, string, end, positions, maxPositions);
    }
    void TextMetrics(float* ascender, float* descender, float* lineh) {
        nvgTextMetrics(vg, ascender, descender, lineh);
    }
    int TextBreakLines(const char* string, const char* end, float breakRowWidth, NVGtextRow* rows, int maxRows) {
        return nvgTextBreakLines(vg, string, end, breakRowWidth, rows, maxRows);
    }


};

#endif //WOSINGPU_NVG_UTIL_H