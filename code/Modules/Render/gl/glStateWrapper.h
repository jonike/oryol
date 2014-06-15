#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glStateWrapper
    @brief translate State changes into GL state changes
    
    The glStateWrapper offers a simplified interface to the GL state,
    and filters redundant state changes. There's only a limited combination
    of state parameter types in GL, which is handled by a handful of 
    ApplyState method overrides.
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"
#include "Render/gl/gl_decl.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {

class mesh;
class blendState;
class programBundle;
class depthStencilState;
    
class glStateWrapper {
public:
    /// constructor
    glStateWrapper();
    /// destructor
    ~glStateWrapper();
    
    /// setup the state wrapper, sets the initial state
    void Setup();
    /// discard the state wrapper
    void Discard();
    /// return true if the state wrapper has been setup
    bool IsValid() const;
    
    /// apply depth-stencil state
    void ApplyDepthStencilState(const depthStencilState* dss);
    /// apply blend state
    void ApplyBlendState(const blendState* bs);
    /// apply state
    void ApplyState(State::Code state, bool b0);
    /// apply state
    void ApplyState(State::Code state, State::Value v0);
    /// apply state
    void ApplyState(State::Code state, float32 f0);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3);
    /// apply state
    void ApplyState(State::Code state, int32 i0);
    /// apply state
    void ApplyState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3);
    
    /// invalidate bound mesh state
    void InvalidateMeshState();
    /// bind vertex buffer
    void BindVertexBuffer(GLuint vb);
    /// bind index buffer
    void BindIndexBuffer(GLuint ib);
    /// bind vertex array object
    void BindVertexArrayObject(GLuint vao);
    
    /// invalidate program state
    void InvalidateProgramState();
    /// invoke glUseProgram (if changed)
    void UseProgram(GLuint prog);
    /// bind currently selected program in program bundle
    void BindProgram(const programBundle* progBundle);

    /// invalidate texture state
    void InvalidateTextureState();
    /// bind a texture to a sampler index
    void BindTexture(int32 samplerIndex, GLenum target, GLuint tex);

    /// bind mesh, program may be needed for vertex attrib binding
    void BindMesh(const mesh* msh, const programBundle* progBundle);
    
private:
    
    /// state update callback function
    typedef void (glStateWrapper::*Callback)(const State::Vector& input);
    
    /// a state table entry
    struct Function {
    public:
        /// constructor
        Function() : cb(0), sig(State::Void) {};
        
        Callback cb;
        State::Signature sig;
    };
    
    /// setup the jump table
    void setupJumpTable();
    /// setup the initial depth-stencil-state
    void setupDepthStencilState();
    /// setup the initial blend-state
    void setupBlendState();
    /// FontFace state function
    void onFrontFace(const State::Vector& input);
    /// CullFaceEnabled state function
    void onCullFaceEnabled(const State::Vector& input);
    /// CullFace state function
    void onCullFace(const State::Vector& input);
    /// DepthOffsetEnabled state function
    void onDepthOffsetEnabled(const State::Vector& input);
    /// DepthOffset state function
    void onDepthOffset(const State::Vector& input);
    /// ScissorTestEnabled state function
    void onScissorTestEnabled(const State::Vector& input);
    /// ScissorRect state function
    void onScissorRect(const State::Vector& input);
    /// BlendColor state function
    void onBlendColor(const State::Vector& input);
    /// DitherEnabled state function
    void onDitherEnabled(const State::Vector& input);
    /// ClearColor state function
    void onClearColor(const State::Vector& input);
    /// ClearDepth state function
    void onClearDepth(const State::Vector& input);
    /// ClearStencil state function
    void onClearStencil(const State::Vector& input);
    /// ViewPort state function
    void onViewPort(const State::Vector& input);
    
    /// apply front/back side stencil state
    void applyStencilState(const depthStencilState* dds, Face::Code face, GLenum glFace);

    bool isValid;

    Function funcs[State::NumStateCodes];
    
    struct {
        CompareFunc::Code depthCompareFunc;
        bool depthWriteEnabled;
        bool stencilTestEnabled;
        struct {
            StencilOp::Code stencilFailOp;
            StencilOp::Code depthFailOp;
            StencilOp::Code depthStencilPassOp;
            CompareFunc::Code stencilCompareFunc;
            uint32 stencilReadMask;
            uint32 stencilWriteMask;
            int32 stencilRef;
        } stencilState[Face::NumSides];
    } curDepthStencilState;
    
    struct {
        bool blendingEnabled;
        BlendFactor::Code rgbSrcFactor;
        BlendFactor::Code rgbDstFactor;
        BlendOperation::Code rgbBlendOperation;
        BlendFactor::Code alphaSrcFactor;
        BlendFactor::Code alphaDstFactor;
        BlendOperation::Code alphaBlendOperation;
        ColorWriteMask::Code colorWriteMask;
    } curBlendState;
    
    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    
    GLenum curFrontFaceMode;
    bool curCullFaceEnabled;
    GLenum curCullFaceMode;
    
    bool curDepthOffsetEnabled;
    GLfloat curDepthOffsetFactor;
    GLfloat curDepthOffsetUnits;
    
    bool curScissorTestEnabled;
    GLint curScissorLeft;
    GLint curScissorBottom;
    GLsizei curScissorWidth;
    GLsizei curScissorHeight;
    
    GLclampf curBlendColorR;
    GLclampf curBlendColorG;
    GLclampf curBlendColorB;
    GLclampf curBlendColorA;
    
    bool curDitherEnabled;
    GLclampf curClearColorR;
    GLclampf curClearColorG;
    GLclampf curClearColorB;
    GLclampf curClearColorA;
    GLclampf curClearDepth;
    GLint curClearStencil;
    GLint curViewPortX;
    GLint curViewPortY;
    GLsizei curViewPortWidth;
    GLsizei curViewPortHeight;
    
    GLuint curVertexBuffer;
    GLuint curIndexBuffer;
    GLuint curVertexArrayObject;
    GLuint curProgram;
    
    static const int32 MaxTextureSamplers = 16;
    GLuint samplers2D[MaxTextureSamplers];
    GLuint samplersCube[MaxTextureSamplers];
};

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, bool b0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::B0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].b = b0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0_F1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    values.val[1].f = f1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1, float32 f2, float32 f3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0_F1_F2_F3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    values.val[1].f = f1;
    values.val[2].f = f2;
    values.val[3].f = f3;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::I0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].i = i0;
    (this->*funcs[c].cb)(values);
}
    
//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0, int32 i1, int32 i2, int32 i3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::I0_I1_I2_I3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].i = i0;
    values.val[1].i = i1;
    values.val[2].i = i2;
    values.val[3].i = i3;
    (this->*funcs[c].cb)(values);
}

} // namespace Render
} // namespace Oryol
