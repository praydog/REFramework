#pragma once

#include <stdint.h>     // intptr_t

#if defined(_WIN32)
// dont bother adding library
#pragma comment(lib,"opengl32.lib")
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/GL.h>
#  define glGetProcAddress(name) (void *)wglGetProcAddress((LPCSTR)name)
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
#  define glGetProcAddress(name) NSGLGetProcAddress(name)
#elif defined(__sgi) || defined(__sun)
#  define glGetProcAddress(name) dlGetProcAddress(name)
#elif defined(__ANDROID__)
#  define glGetProcAddress(name) NULL /* TODO */
#else /* __linux */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLX_get_proc_address
#define GLX_get_proc_address 1

   typedef void(*__GLXextFuncPtr)(void);
   extern __GLXextFuncPtr glXGetProcAddress(const GLubyte*);

#endif /* GLX_get_proc_address */

#ifdef __cplusplus
}
#endif
#  define glGetProcAddress(name) (*glXGetProcAddressARB)(name)
#endif

#ifdef FMOD_API
#include "fmod_studio.hpp"
#include "fmod.hpp"
#endif

//    typedef long int ptrdiff_t;
typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef unsigned int GLenum;
typedef int GLsizei;
typedef unsigned int GLuint;
typedef int GLint;
typedef void GLvoid;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef float GLfloat;

#if!defined(APIENTRY)
#if defined(__MINGW32__) || defined(__CYGWIN__)
#define APIENTRY __stdcall
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif


/*************************************************************/

#ifndef GL_VERSION_1_1
    /* AttribMask */
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
    /* Boolean */
#define GL_FALSE                          0
#define GL_TRUE                           1
    /* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
    /* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207
    /* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
    /* BlendingFactorSrc */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
    /* DrawBufferMode */
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
    /* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505
    /* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901
    /* GetPName */
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_VIEWPORT                       0x0BA2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069
    /* GetTextureParameter */
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
    /* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102
    /* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_DOUBLE                         0x140A
    /* ErrorCode */
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
    /* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F
    /* MatrixMode (for gl3.h, FBO attachment type) */
#define GL_TEXTURE                        0x1702
    /* PixelCopyType */
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802
    /* PixelFormat */
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
    /* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02
    /* StencilOp */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
    /* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
    /* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
    /* TextureMinFilter */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
    /* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
    /* TextureTarget */
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064
    /* TextureWrapMode */
#define GL_REPEAT                         0x2901
    /* PixelInternalFormat */
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#endif

#ifndef GL_VERSION_1_2
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_TEXTURE_BINDING_3D             0x806A
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#endif

#ifndef GL_ARB_imaging
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_BLEND_COLOR                    0x8005
#define GL_FUNC_ADD                       0x8006
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#define GL_BLEND_EQUATION                 0x8009
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#endif

#ifndef GL_VERSION_1_3
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
#define GL_COMPRESSED_RGB                 0x84ED
#define GL_COMPRESSED_RGBA                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_CLAMP_TO_BORDER                0x812D
#endif

#ifndef GL_VERSION_1_4
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_MIRRORED_REPEAT                0x8370
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_INCR_WRAP                      0x8507
#define GL_DECR_WRAP                      0x8508
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#endif

#ifndef GL_VERSION_1_5
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_SAMPLES_PASSED                 0x8914
#define GL_SRC1_ALPHA                     0x8589
#endif

#ifndef GL_VERSION_2_0
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
#define GL_CURRENT_VERTEX_ATTRIB          0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645
#define GL_STENCIL_BACK_FUNC              0x8800
#define GL_STENCIL_BACK_FAIL              0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL   0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS   0x8803
#define GL_MAX_DRAW_BUFFERS               0x8824
#define GL_DRAW_BUFFER0                   0x8825
#define GL_DRAW_BUFFER1                   0x8826
#define GL_DRAW_BUFFER2                   0x8827
#define GL_DRAW_BUFFER3                   0x8828
#define GL_DRAW_BUFFER4                   0x8829
#define GL_DRAW_BUFFER5                   0x882A
#define GL_DRAW_BUFFER6                   0x882B
#define GL_DRAW_BUFFER7                   0x882C
#define GL_DRAW_BUFFER8                   0x882D
#define GL_DRAW_BUFFER9                   0x882E
#define GL_DRAW_BUFFER10                  0x882F
#define GL_DRAW_BUFFER11                  0x8830
#define GL_DRAW_BUFFER12                  0x8831
#define GL_DRAW_BUFFER13                  0x8832
#define GL_DRAW_BUFFER14                  0x8833
#define GL_DRAW_BUFFER15                  0x8834
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_MAX_VERTEX_ATTRIBS             0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
#define GL_MAX_VARYING_FLOATS             0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE                    0x8B4F
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#define GL_SAMPLER_1D_SHADOW              0x8B61
#define GL_SAMPLER_2D_SHADOW              0x8B62
#define GL_DELETE_STATUS                  0x8B80
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ATTACHED_SHADERS               0x8B85
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
#define GL_SHADER_SOURCE_LENGTH           0x8B88
#define GL_ACTIVE_ATTRIBUTES              0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN      0x8CA0
#define GL_LOWER_LEFT                     0x8CA1
#define GL_UPPER_LEFT                     0x8CA2
#define GL_STENCIL_BACK_REF               0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK        0x8CA4
#define GL_STENCIL_BACK_WRITEMASK         0x8CA5
#endif

#ifndef GL_VERSION_2_1
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_COMPRESSED_SRGB                0x8C48
#define GL_COMPRESSED_SRGB_ALPHA          0x8C49
#endif

#ifndef GL_VERSION_3_0
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_CLIP_DISTANCE0                 0x3000
#define GL_CLIP_DISTANCE1                 0x3001
#define GL_CLIP_DISTANCE2                 0x3002
#define GL_CLIP_DISTANCE3                 0x3003
#define GL_CLIP_DISTANCE4                 0x3004
#define GL_CLIP_DISTANCE5                 0x3005
#define GL_CLIP_DISTANCE6                 0x3006
#define GL_CLIP_DISTANCE7                 0x3007
#define GL_MAX_CLIP_DISTANCES             0x0D32
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_NUM_EXTENSIONS                 0x821D
#define GL_CONTEXT_FLAGS                  0x821E
#define GL_COMPRESSED_RED                 0x8225
#define GL_COMPRESSED_RG                  0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x0001
#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER    0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS       0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET       0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET       0x8905
#define GL_CLAMP_READ_COLOR               0x891C
#define GL_FIXED_ONLY                     0x891D
#define GL_MAX_VARYING_COMPONENTS         0x8B4B
#define GL_TEXTURE_1D_ARRAY               0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY         0x8C19
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY         0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY       0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY       0x8C1D
#define GL_R11F_G11F_B10F                 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV   0x8C3B
#define GL_RGB9_E5                        0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV       0x8C3E
#define GL_TEXTURE_SHARED_SIZE            0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS    0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED           0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD             0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS            0x8C8C
#define GL_SEPARATE_ATTRIBS               0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER      0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI                       0x8D70
#define GL_RGB32UI                        0x8D71
#define GL_RGBA16UI                       0x8D76
#define GL_RGB16UI                        0x8D77
#define GL_RGBA8UI                        0x8D7C
#define GL_RGB8UI                         0x8D7D
#define GL_RGBA32I                        0x8D82
#define GL_RGB32I                         0x8D83
#define GL_RGBA16I                        0x8D88
#define GL_RGB16I                         0x8D89
#define GL_RGBA8I                         0x8D8E
#define GL_RGB8I                          0x8D8F
#define GL_RED_INTEGER                    0x8D94
#define GL_GREEN_INTEGER                  0x8D95
#define GL_BLUE_INTEGER                   0x8D96
#define GL_RGB_INTEGER                    0x8D98
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGR_INTEGER                    0x8D9A
#define GL_BGRA_INTEGER                   0x8D9B
#define GL_SAMPLER_1D_ARRAY               0x8DC0
#define GL_SAMPLER_2D_ARRAY               0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW        0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW        0x8DC4
#define GL_SAMPLER_CUBE_SHADOW            0x8DC5
#define GL_UNSIGNED_INT_VEC2              0x8DC6
#define GL_UNSIGNED_INT_VEC3              0x8DC7
#define GL_UNSIGNED_INT_VEC4              0x8DC8
#define GL_INT_SAMPLER_1D                 0x8DC9
#define GL_INT_SAMPLER_2D                 0x8DCA
#define GL_INT_SAMPLER_3D                 0x8DCB
#define GL_INT_SAMPLER_CUBE               0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY           0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY           0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D        0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D        0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D        0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE      0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY  0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY  0x8DD7
#define GL_QUERY_WAIT                     0x8E13
#define GL_QUERY_NO_WAIT                  0x8E14
#define GL_QUERY_BY_REGION_WAIT           0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT        0x8E16
#define GL_BUFFER_ACCESS_FLAGS            0x911F
#define GL_BUFFER_MAP_LENGTH              0x9120
#define GL_BUFFER_MAP_OFFSET              0x9121
    /* Reuse tokens from ARB_depth_buffer_float */
    /* reuse GL_DEPTH_COMPONENT32F */
    /* reuse GL_DEPTH32F_STENCIL8 */
    /* reuse GL_FLOAT_32_UNSIGNED_INT_24_8_REV */
    /* Reuse tokens from ARB_framebuffer_object */
    /* reuse GL_INVALID_FRAMEBUFFER_OPERATION */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE */
    /* reuse GL_FRAMEBUFFER_DEFAULT */
    /* reuse GL_FRAMEBUFFER_UNDEFINED */
    /* reuse GL_DEPTH_STENCIL_ATTACHMENT */
    /* reuse GL_INDEX */
    /* reuse GL_MAX_RENDERBUFFER_SIZE */
    /* reuse GL_DEPTH_STENCIL */
    /* reuse GL_UNSIGNED_INT_24_8 */
    /* reuse GL_DEPTH24_STENCIL8 */
    /* reuse GL_TEXTURE_STENCIL_SIZE */
    /* reuse GL_TEXTURE_RED_TYPE */
    /* reuse GL_TEXTURE_GREEN_TYPE */
    /* reuse GL_TEXTURE_BLUE_TYPE */
    /* reuse GL_TEXTURE_ALPHA_TYPE */
    /* reuse GL_TEXTURE_DEPTH_TYPE */
    /* reuse GL_UNSIGNED_NORMALIZED */
    /* reuse GL_FRAMEBUFFER_BINDING */
    /* reuse GL_DRAW_FRAMEBUFFER_BINDING */
    /* reuse GL_RENDERBUFFER_BINDING */
    /* reuse GL_READ_FRAMEBUFFER */
    /* reuse GL_DRAW_FRAMEBUFFER */
    /* reuse GL_READ_FRAMEBUFFER_BINDING */
    /* reuse GL_RENDERBUFFER_SAMPLES */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER */
    /* reuse GL_FRAMEBUFFER_COMPLETE */
    /* reuse GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT */
    /* reuse GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT */
    /* reuse GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER */
    /* reuse GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER */
    /* reuse GL_FRAMEBUFFER_UNSUPPORTED */
    /* reuse GL_MAX_COLOR_ATTACHMENTS */
    /* reuse GL_COLOR_ATTACHMENT0 */
    /* reuse GL_COLOR_ATTACHMENT1 */
    /* reuse GL_COLOR_ATTACHMENT2 */
    /* reuse GL_COLOR_ATTACHMENT3 */
    /* reuse GL_COLOR_ATTACHMENT4 */
    /* reuse GL_COLOR_ATTACHMENT5 */
    /* reuse GL_COLOR_ATTACHMENT6 */
    /* reuse GL_COLOR_ATTACHMENT7 */
    /* reuse GL_COLOR_ATTACHMENT8 */
    /* reuse GL_COLOR_ATTACHMENT9 */
    /* reuse GL_COLOR_ATTACHMENT10 */
    /* reuse GL_COLOR_ATTACHMENT11 */
    /* reuse GL_COLOR_ATTACHMENT12 */
    /* reuse GL_COLOR_ATTACHMENT13 */
    /* reuse GL_COLOR_ATTACHMENT14 */
    /* reuse GL_COLOR_ATTACHMENT15 */
    /* reuse GL_DEPTH_ATTACHMENT */
    /* reuse GL_STENCIL_ATTACHMENT */
    /* reuse GL_FRAMEBUFFER */
    /* reuse GL_RENDERBUFFER */
    /* reuse GL_RENDERBUFFER_WIDTH */
    /* reuse GL_RENDERBUFFER_HEIGHT */
    /* reuse GL_RENDERBUFFER_INTERNAL_FORMAT */
    /* reuse GL_STENCIL_INDEX1 */
    /* reuse GL_STENCIL_INDEX4 */
    /* reuse GL_STENCIL_INDEX8 */
    /* reuse GL_STENCIL_INDEX16 */
    /* reuse GL_RENDERBUFFER_RED_SIZE */
    /* reuse GL_RENDERBUFFER_GREEN_SIZE */
    /* reuse GL_RENDERBUFFER_BLUE_SIZE */
    /* reuse GL_RENDERBUFFER_ALPHA_SIZE */
    /* reuse GL_RENDERBUFFER_DEPTH_SIZE */
    /* reuse GL_RENDERBUFFER_STENCIL_SIZE */
    /* reuse GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE */
    /* reuse GL_MAX_SAMPLES */
    /* Reuse tokens from ARB_framebuffer_sRGB */
    /* reuse GL_FRAMEBUFFER_SRGB */
    /* Reuse tokens from ARB_half_float_vertex */
    /* reuse GL_HALF_FLOAT */
    /* Reuse tokens from ARB_map_buffer_range */
    /* reuse GL_MAP_READ_BIT */
    /* reuse GL_MAP_WRITE_BIT */
    /* reuse GL_MAP_INVALIDATE_RANGE_BIT */
    /* reuse GL_MAP_INVALIDATE_BUFFER_BIT */
    /* reuse GL_MAP_FLUSH_EXPLICIT_BIT */
    /* reuse GL_MAP_UNSYNCHRONIZED_BIT */
    /* Reuse tokens from ARB_texture_compression_rgtc */
    /* reuse GL_COMPRESSED_RED_RGTC1 */
    /* reuse GL_COMPRESSED_SIGNED_RED_RGTC1 */
    /* reuse GL_COMPRESSED_RG_RGTC2 */
    /* reuse GL_COMPRESSED_SIGNED_RG_RGTC2 */
    /* Reuse tokens from ARB_texture_rg */
    /* reuse GL_RG */
    /* reuse GL_RG_INTEGER */
    /* reuse GL_R8 */
    /* reuse GL_R16 */
    /* reuse GL_RG8 */
    /* reuse GL_RG16 */
    /* reuse GL_R16F */
    /* reuse GL_R32F */
    /* reuse GL_RG16F */
    /* reuse GL_RG32F */
    /* reuse GL_R8I */
    /* reuse GL_R8UI */
    /* reuse GL_R16I */
    /* reuse GL_R16UI */
    /* reuse GL_R32I */
    /* reuse GL_R32UI */
    /* reuse GL_RG8I */
    /* reuse GL_RG8UI */
    /* reuse GL_RG16I */
    /* reuse GL_RG16UI */
    /* reuse GL_RG32I */
    /* reuse GL_RG32UI */
    /* Reuse tokens from ARB_vertex_array_object */
    /* reuse GL_VERTEX_ARRAY_BINDING */
#endif

#ifndef GL_VERSION_3_1
#define GL_SAMPLER_2D_RECT                0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW         0x8B64
#define GL_SAMPLER_BUFFER                 0x8DC2
#define GL_INT_SAMPLER_2D_RECT            0x8DCD
#define GL_INT_SAMPLER_BUFFER             0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT   0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER    0x8DD8
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE        0x8C2B
#define GL_TEXTURE_BINDING_BUFFER         0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE              0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE      0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE        0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE     0x84F8
#define GL_RED_SNORM                      0x8F90
#define GL_RG_SNORM                       0x8F91
#define GL_RGB_SNORM                      0x8F92
#define GL_RGBA_SNORM                     0x8F93
#define GL_R8_SNORM                       0x8F94
#define GL_RG8_SNORM                      0x8F95
#define GL_RGB8_SNORM                     0x8F96
#define GL_RGBA8_SNORM                    0x8F97
#define GL_R16_SNORM                      0x8F98
#define GL_RG16_SNORM                     0x8F99
#define GL_RGB16_SNORM                    0x8F9A
#define GL_RGBA16_SNORM                   0x8F9B
#define GL_SIGNED_NORMALIZED              0x8F9C
#define GL_PRIMITIVE_RESTART              0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX        0x8F9E
    /* Reuse tokens from ARB_copy_buffer */
    /* reuse GL_COPY_READ_BUFFER */
    /* reuse GL_COPY_WRITE_BUFFER */
    /* Reuse tokens from ARB_draw_instanced (none) */
    /* Reuse tokens from ARB_uniform_buffer_object */
    /* reuse GL_UNIFORM_BUFFER */
    /* reuse GL_UNIFORM_BUFFER_BINDING */
    /* reuse GL_UNIFORM_BUFFER_START */
    /* reuse GL_UNIFORM_BUFFER_SIZE */
    /* reuse GL_MAX_VERTEX_UNIFORM_BLOCKS */
    /* reuse GL_MAX_FRAGMENT_UNIFORM_BLOCKS */
    /* reuse GL_MAX_COMBINED_UNIFORM_BLOCKS */
    /* reuse GL_MAX_UNIFORM_BUFFER_BINDINGS */
    /* reuse GL_MAX_UNIFORM_BLOCK_SIZE */
    /* reuse GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS */
    /* reuse GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT */
    /* reuse GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH */
    /* reuse GL_ACTIVE_UNIFORM_BLOCKS */
    /* reuse GL_UNIFORM_TYPE */
    /* reuse GL_UNIFORM_SIZE */
    /* reuse GL_UNIFORM_NAME_LENGTH */
    /* reuse GL_UNIFORM_BLOCK_INDEX */
    /* reuse GL_UNIFORM_OFFSET */
    /* reuse GL_UNIFORM_ARRAY_STRIDE */
    /* reuse GL_UNIFORM_MATRIX_STRIDE */
    /* reuse GL_UNIFORM_IS_ROW_MAJOR */
    /* reuse GL_UNIFORM_BLOCK_BINDING */
    /* reuse GL_UNIFORM_BLOCK_DATA_SIZE */
    /* reuse GL_UNIFORM_BLOCK_NAME_LENGTH */
    /* reuse GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS */
    /* reuse GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES */
    /* reuse GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER */
    /* reuse GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER */
    /* reuse GL_INVALID_INDEX */
#endif

#ifndef GL_VERSION_3_2
#define GL_CONTEXT_CORE_PROFILE_BIT       0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY                0x000A
#define GL_LINE_STRIP_ADJACENCY           0x000B
#define GL_TRIANGLES_ADJACENCY            0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY       0x000D
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_GEOMETRY_VERTICES_OUT          0x8916
#define GL_GEOMETRY_INPUT_TYPE            0x8917
#define GL_GEOMETRY_OUTPUT_TYPE           0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES   0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS   0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS  0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS  0x9125
#define GL_CONTEXT_PROFILE_MASK           0x9126
    /* reuse GL_MAX_VARYING_COMPONENTS */
    /* reuse GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER */
    /* Reuse tokens from ARB_depth_clamp */
    /* reuse GL_DEPTH_CLAMP */
    /* Reuse tokens from ARB_draw_elements_base_vertex (none) */
    /* Reuse tokens from ARB_fragment_coord_conventions (none) */
    /* Reuse tokens from ARB_provoking_vertex */
    /* reuse GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION */
    /* reuse GL_FIRST_VERTEX_CONVENTION */
    /* reuse GL_LAST_VERTEX_CONVENTION */
    /* reuse GL_PROVOKING_VERTEX */
    /* Reuse tokens from ARB_seamless_cube_map */
    /* reuse GL_TEXTURE_CUBE_MAP_SEAMLESS */
    /* Reuse tokens from ARB_sync */
    /* reuse GL_MAX_SERVER_WAIT_TIMEOUT */
    /* reuse GL_OBJECT_TYPE */
    /* reuse GL_SYNC_CONDITION */
    /* reuse GL_SYNC_STATUS */
    /* reuse GL_SYNC_FLAGS */
    /* reuse GL_SYNC_FENCE */
    /* reuse GL_SYNC_GPU_COMMANDS_COMPLETE */
    /* reuse GL_UNSIGNALED */
    /* reuse GL_SIGNALED */
    /* reuse GL_ALREADY_SIGNALED */
    /* reuse GL_TIMEOUT_EXPIRED */
    /* reuse GL_CONDITION_SATISFIED */
    /* reuse GL_WAIT_FAILED */
    /* reuse GL_TIMEOUT_IGNORED */
    /* reuse GL_SYNC_FLUSH_COMMANDS_BIT */
    /* reuse GL_TIMEOUT_IGNORED */
    /* Reuse tokens from ARB_texture_multisample */
    /* reuse GL_SAMPLE_POSITION */
    /* reuse GL_SAMPLE_MASK */
    /* reuse GL_SAMPLE_MASK_VALUE */
    /* reuse GL_MAX_SAMPLE_MASK_WORDS */
    /* reuse GL_TEXTURE_2D_MULTISAMPLE */
    /* reuse GL_PROXY_TEXTURE_2D_MULTISAMPLE */
    /* reuse GL_TEXTURE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_TEXTURE_BINDING_2D_MULTISAMPLE */
    /* reuse GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_TEXTURE_SAMPLES */
    /* reuse GL_TEXTURE_FIXED_SAMPLE_LOCATIONS */
    /* reuse GL_SAMPLER_2D_MULTISAMPLE */
    /* reuse GL_INT_SAMPLER_2D_MULTISAMPLE */
    /* reuse GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE */
    /* reuse GL_SAMPLER_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_MAX_COLOR_TEXTURE_SAMPLES */
    /* reuse GL_MAX_DEPTH_TEXTURE_SAMPLES */
    /* reuse GL_MAX_INTEGER_SAMPLES */
    /* Don't need to reuse tokens from ARB_vertex_array_bgra since they're already in 1.2 core */
#endif

#ifndef GL_VERSION_3_3
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
    /* Reuse tokens from ARB_blend_func_extended */
    /* reuse GL_SRC1_COLOR */
    /* reuse GL_ONE_MINUS_SRC1_COLOR */
    /* reuse GL_ONE_MINUS_SRC1_ALPHA */
    /* reuse GL_MAX_DUAL_SOURCE_DRAW_BUFFERS */
    /* Reuse tokens from ARB_explicit_attrib_location (none) */
    /* Reuse tokens from ARB_occlusion_query2 */
    /* reuse GL_ANY_SAMPLES_PASSED */
    /* Reuse tokens from ARB_sampler_objects */
    /* reuse GL_SAMPLER_BINDING */
    /* Reuse tokens from ARB_shader_bit_encoding (none) */
    /* Reuse tokens from ARB_texture_rgb10_a2ui */
    /* reuse GL_RGB10_A2UI */
    /* Reuse tokens from ARB_texture_swizzle */
    /* reuse GL_TEXTURE_SWIZZLE_R */
    /* reuse GL_TEXTURE_SWIZZLE_G */
    /* reuse GL_TEXTURE_SWIZZLE_B */
    /* reuse GL_TEXTURE_SWIZZLE_A */
    /* reuse GL_TEXTURE_SWIZZLE_RGBA */
    /* Reuse tokens from ARB_timer_query */
    /* reuse GL_TIME_ELAPSED */
    /* reuse GL_TIMESTAMP */
    /* Reuse tokens from ARB_vertex_type_2_10_10_10_rev */
    /* reuse GL_INT_2_10_10_10_REV */
#endif

#ifndef GL_VERSION_4_0
#define GL_SAMPLE_SHADING                 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE       0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY   0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY         0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW  0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY     0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
    /* Reuse tokens from ARB_texture_query_lod (none) */
    /* Reuse tokens from ARB_draw_buffers_blend (none) */
    /* Reuse tokens from ARB_draw_indirect */
    /* reuse GL_DRAW_INDIRECT_BUFFER */
    /* reuse GL_DRAW_INDIRECT_BUFFER_BINDING */
    /* Reuse tokens from ARB_gpu_shader5 */
    /* reuse GL_GEOMETRY_SHADER_INVOCATIONS */
    /* reuse GL_MAX_GEOMETRY_SHADER_INVOCATIONS */
    /* reuse GL_MIN_FRAGMENT_INTERPOLATION_OFFSET */
    /* reuse GL_MAX_FRAGMENT_INTERPOLATION_OFFSET */
    /* reuse GL_FRAGMENT_INTERPOLATION_OFFSET_BITS */
    /* reuse GL_MAX_VERTEX_STREAMS */
    /* Reuse tokens from ARB_gpu_shader_fp64 */
    /* reuse GL_DOUBLE_VEC2 */
    /* reuse GL_DOUBLE_VEC3 */
    /* reuse GL_DOUBLE_VEC4 */
    /* reuse GL_DOUBLE_MAT2 */
    /* reuse GL_DOUBLE_MAT3 */
    /* reuse GL_DOUBLE_MAT4 */
    /* reuse GL_DOUBLE_MAT2x3 */
    /* reuse GL_DOUBLE_MAT2x4 */
    /* reuse GL_DOUBLE_MAT3x2 */
    /* reuse GL_DOUBLE_MAT3x4 */
    /* reuse GL_DOUBLE_MAT4x2 */
    /* reuse GL_DOUBLE_MAT4x3 */
    /* Reuse tokens from ARB_shader_subroutine */
    /* reuse GL_ACTIVE_SUBROUTINES */
    /* reuse GL_ACTIVE_SUBROUTINE_UNIFORMS */
    /* reuse GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS */
    /* reuse GL_ACTIVE_SUBROUTINE_MAX_LENGTH */
    /* reuse GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH */
    /* reuse GL_MAX_SUBROUTINES */
    /* reuse GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS */
    /* reuse GL_NUM_COMPATIBLE_SUBROUTINES */
    /* reuse GL_COMPATIBLE_SUBROUTINES */
    /* Reuse tokens from ARB_tessellation_shader */
    /* reuse GL_PATCHES */
    /* reuse GL_PATCH_VERTICES */
    /* reuse GL_PATCH_DEFAULT_INNER_LEVEL */
    /* reuse GL_PATCH_DEFAULT_OUTER_LEVEL */
    /* reuse GL_TESS_CONTROL_OUTPUT_VERTICES */
    /* reuse GL_TESS_GEN_MODE */
    /* reuse GL_TESS_GEN_SPACING */
    /* reuse GL_TESS_GEN_VERTEX_ORDER */
    /* reuse GL_TESS_GEN_POINT_MODE */
    /* reuse GL_ISOLINES */
    /* reuse GL_FRACTIONAL_ODD */
    /* reuse GL_FRACTIONAL_EVEN */
    /* reuse GL_MAX_PATCH_VERTICES */
    /* reuse GL_MAX_TESS_GEN_LEVEL */
    /* reuse GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS */
    /* reuse GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS */
    /* reuse GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS */
    /* reuse GL_MAX_TESS_PATCH_COMPONENTS */
    /* reuse GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS */
    /* reuse GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS */
    /* reuse GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS */
    /* reuse GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS */
    /* reuse GL_MAX_TESS_CONTROL_INPUT_COMPONENTS */
    /* reuse GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS */
    /* reuse GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS */
    /* reuse GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER */
    /* reuse GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER */
    /* reuse GL_TESS_EVALUATION_SHADER */
    /* reuse GL_TESS_CONTROL_SHADER */
    /* Reuse tokens from ARB_texture_buffer_object_rgb32 (none) */
    /* Reuse tokens from ARB_transform_feedback2 */
    /* reuse GL_TRANSFORM_FEEDBACK */
    /* reuse GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED */
    /* reuse GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE */
    /* reuse GL_TRANSFORM_FEEDBACK_BINDING */
    /* Reuse tokens from ARB_transform_feedback3 */
    /* reuse GL_MAX_TRANSFORM_FEEDBACK_BUFFERS */
    /* reuse GL_MAX_VERTEX_STREAMS */
#endif

#ifndef GL_VERSION_4_1
    /* Reuse tokens from ARB_ES2_compatibility */
    /* reuse GL_FIXED */
    /* reuse GL_IMPLEMENTATION_COLOR_READ_TYPE */
    /* reuse GL_IMPLEMENTATION_COLOR_READ_FORMAT */
    /* reuse GL_LOW_FLOAT */
    /* reuse GL_MEDIUM_FLOAT */
    /* reuse GL_HIGH_FLOAT */
    /* reuse GL_LOW_INT */
    /* reuse GL_MEDIUM_INT */
    /* reuse GL_HIGH_INT */
    /* reuse GL_SHADER_COMPILER */
    /* reuse GL_SHADER_BINARY_FORMATS */
    /* reuse GL_NUM_SHADER_BINARY_FORMATS */
    /* reuse GL_MAX_VERTEX_UNIFORM_VECTORS */
    /* reuse GL_MAX_VARYING_VECTORS */
    /* reuse GL_MAX_FRAGMENT_UNIFORM_VECTORS */
    /* reuse GL_RGB565 */
    /* Reuse tokens from ARB_get_program_binary */
    /* reuse GL_PROGRAM_BINARY_RETRIEVABLE_HINT */
    /* reuse GL_PROGRAM_BINARY_LENGTH */
    /* reuse GL_NUM_PROGRAM_BINARY_FORMATS */
    /* reuse GL_PROGRAM_BINARY_FORMATS */
    /* Reuse tokens from ARB_separate_shader_objects */
    /* reuse GL_VERTEX_SHADER_BIT */
    /* reuse GL_FRAGMENT_SHADER_BIT */
    /* reuse GL_GEOMETRY_SHADER_BIT */
    /* reuse GL_TESS_CONTROL_SHADER_BIT */
    /* reuse GL_TESS_EVALUATION_SHADER_BIT */
    /* reuse GL_ALL_SHADER_BITS */
    /* reuse GL_PROGRAM_SEPARABLE */
    /* reuse GL_ACTIVE_PROGRAM */
    /* reuse GL_PROGRAM_PIPELINE_BINDING */
    /* Reuse tokens from ARB_shader_precision (none) */
    /* Reuse tokens from ARB_vertex_attrib_64bit - all are in GL 3.0 and 4.0 already */
    /* Reuse tokens from ARB_viewport_array - some are in GL 1.1 and ARB_provoking_vertex already */
    /* reuse GL_MAX_VIEWPORTS */
    /* reuse GL_VIEWPORT_SUBPIXEL_BITS */
    /* reuse GL_VIEWPORT_BOUNDS_RANGE */
    /* reuse GL_LAYER_PROVOKING_VERTEX */
    /* reuse GL_VIEWPORT_INDEX_PROVOKING_VERTEX */
    /* reuse GL_UNDEFINED_VERTEX */
#endif

#ifndef GL_VERSION_4_2
    /* Reuse tokens from ARB_base_instance (none) */
    /* Reuse tokens from ARB_shading_language_420pack (none) */
    /* Reuse tokens from ARB_transform_feedback_instanced (none) */
    /* Reuse tokens from ARB_compressed_texture_pixel_storage */
    /* reuse GL_UNPACK_COMPRESSED_BLOCK_WIDTH */
    /* reuse GL_UNPACK_COMPRESSED_BLOCK_HEIGHT */
    /* reuse GL_UNPACK_COMPRESSED_BLOCK_DEPTH */
    /* reuse GL_UNPACK_COMPRESSED_BLOCK_SIZE */
    /* reuse GL_PACK_COMPRESSED_BLOCK_WIDTH */
    /* reuse GL_PACK_COMPRESSED_BLOCK_HEIGHT */
    /* reuse GL_PACK_COMPRESSED_BLOCK_DEPTH */
    /* reuse GL_PACK_COMPRESSED_BLOCK_SIZE */
    /* Reuse tokens from ARB_conservative_depth (none) */
    /* Reuse tokens from ARB_internalformat_query */
    /* reuse GL_NUM_SAMPLE_COUNTS */
    /* Reuse tokens from ARB_map_buffer_alignment */
    /* reuse GL_MIN_MAP_BUFFER_ALIGNMENT */
    /* Reuse tokens from ARB_shader_atomic_counters */
    /* reuse GL_ATOMIC_COUNTER_BUFFER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_BINDING */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_START */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_SIZE */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER */
    /* reuse GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_VERTEX_ATOMIC_COUNTERS */
    /* reuse GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS */
    /* reuse GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS */
    /* reuse GL_MAX_GEOMETRY_ATOMIC_COUNTERS */
    /* reuse GL_MAX_FRAGMENT_ATOMIC_COUNTERS */
    /* reuse GL_MAX_COMBINED_ATOMIC_COUNTERS */
    /* reuse GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE */
    /* reuse GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS */
    /* reuse GL_ACTIVE_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX */
    /* reuse GL_UNSIGNED_INT_ATOMIC_COUNTER */
    /* Reuse tokens from ARB_shader_image_load_store */
    /* reuse GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT */
    /* reuse GL_ELEMENT_ARRAY_BARRIER_BIT */
    /* reuse GL_UNIFORM_BARRIER_BIT */
    /* reuse GL_TEXTURE_FETCH_BARRIER_BIT */
    /* reuse GL_SHADER_IMAGE_ACCESS_BARRIER_BIT */
    /* reuse GL_COMMAND_BARRIER_BIT */
    /* reuse GL_PIXEL_BUFFER_BARRIER_BIT */
    /* reuse GL_TEXTURE_UPDATE_BARRIER_BIT */
    /* reuse GL_BUFFER_UPDATE_BARRIER_BIT */
    /* reuse GL_FRAMEBUFFER_BARRIER_BIT */
    /* reuse GL_TRANSFORM_FEEDBACK_BARRIER_BIT */
    /* reuse GL_ATOMIC_COUNTER_BARRIER_BIT */
    /* reuse GL_ALL_BARRIER_BITS */
    /* reuse GL_MAX_IMAGE_UNITS */
    /* reuse GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS */
    /* reuse GL_IMAGE_BINDING_NAME */
    /* reuse GL_IMAGE_BINDING_LEVEL */
    /* reuse GL_IMAGE_BINDING_LAYERED */
    /* reuse GL_IMAGE_BINDING_LAYER */
    /* reuse GL_IMAGE_BINDING_ACCESS */
    /* reuse GL_IMAGE_1D */
    /* reuse GL_IMAGE_2D */
    /* reuse GL_IMAGE_3D */
    /* reuse GL_IMAGE_2D_RECT */
    /* reuse GL_IMAGE_CUBE */
    /* reuse GL_IMAGE_BUFFER */
    /* reuse GL_IMAGE_1D_ARRAY */
    /* reuse GL_IMAGE_2D_ARRAY */
    /* reuse GL_IMAGE_CUBE_MAP_ARRAY */
    /* reuse GL_IMAGE_2D_MULTISAMPLE */
    /* reuse GL_IMAGE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_INT_IMAGE_1D */
    /* reuse GL_INT_IMAGE_2D */
    /* reuse GL_INT_IMAGE_3D */
    /* reuse GL_INT_IMAGE_2D_RECT */
    /* reuse GL_INT_IMAGE_CUBE */
    /* reuse GL_INT_IMAGE_BUFFER */
    /* reuse GL_INT_IMAGE_1D_ARRAY */
    /* reuse GL_INT_IMAGE_2D_ARRAY */
    /* reuse GL_INT_IMAGE_CUBE_MAP_ARRAY */
    /* reuse GL_INT_IMAGE_2D_MULTISAMPLE */
    /* reuse GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_UNSIGNED_INT_IMAGE_1D */
    /* reuse GL_UNSIGNED_INT_IMAGE_2D */
    /* reuse GL_UNSIGNED_INT_IMAGE_3D */
    /* reuse GL_UNSIGNED_INT_IMAGE_2D_RECT */
    /* reuse GL_UNSIGNED_INT_IMAGE_CUBE */
    /* reuse GL_UNSIGNED_INT_IMAGE_BUFFER */
    /* reuse GL_UNSIGNED_INT_IMAGE_1D_ARRAY */
    /* reuse GL_UNSIGNED_INT_IMAGE_2D_ARRAY */
    /* reuse GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY */
    /* reuse GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE */
    /* reuse GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_MAX_IMAGE_SAMPLES */
    /* reuse GL_IMAGE_BINDING_FORMAT */
    /* reuse GL_IMAGE_FORMAT_COMPATIBILITY_TYPE */
    /* reuse GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE */
    /* reuse GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS */
    /* reuse GL_MAX_VERTEX_IMAGE_UNIFORMS */
    /* reuse GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS */
    /* reuse GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS */
    /* reuse GL_MAX_GEOMETRY_IMAGE_UNIFORMS */
    /* reuse GL_MAX_FRAGMENT_IMAGE_UNIFORMS */
    /* reuse GL_MAX_COMBINED_IMAGE_UNIFORMS */
    /* Reuse tokens from ARB_shading_language_packing (none) */
    /* Reuse tokens from ARB_texture_storage */
    /* reuse GL_TEXTURE_IMMUTABLE_FORMAT */
#endif

#ifndef GL_VERSION_4_3
#define GL_NUM_SHADING_LANGUAGE_VERSIONS  0x82E9
#define GL_VERTEX_ATTRIB_ARRAY_LONG       0x874E
    /* Reuse tokens from ARB_arrays_of_arrays (none, GLSL only) */
    /* Reuse tokens from ARB_fragment_layer_viewport (none, GLSL only) */
    /* Reuse tokens from ARB_shader_image_size (none, GLSL only) */
    /* Reuse tokens from ARB_ES3_compatibility */
    /* reuse GL_COMPRESSED_RGB8_ETC2 */
    /* reuse GL_COMPRESSED_SRGB8_ETC2 */
    /* reuse GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 */
    /* reuse GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 */
    /* reuse GL_COMPRESSED_RGBA8_ETC2_EAC */
    /* reuse GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC */
    /* reuse GL_COMPRESSED_R11_EAC */
    /* reuse GL_COMPRESSED_SIGNED_R11_EAC */
    /* reuse GL_COMPRESSED_RG11_EAC */
    /* reuse GL_COMPRESSED_SIGNED_RG11_EAC */
    /* reuse GL_PRIMITIVE_RESTART_FIXED_INDEX */
    /* reuse GL_ANY_SAMPLES_PASSED_CONSERVATIVE */
    /* reuse GL_MAX_ELEMENT_INDEX */
    /* Reuse tokens from ARB_clear_buffer_object (none) */
    /* Reuse tokens from ARB_compute_shader */
    /* reuse GL_COMPUTE_SHADER */
    /* reuse GL_MAX_COMPUTE_UNIFORM_BLOCKS */
    /* reuse GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS */
    /* reuse GL_MAX_COMPUTE_IMAGE_UNIFORMS */
    /* reuse GL_MAX_COMPUTE_SHARED_MEMORY_SIZE */
    /* reuse GL_MAX_COMPUTE_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS */
    /* reuse GL_MAX_COMPUTE_ATOMIC_COUNTERS */
    /* reuse GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS */
    /* reuse GL_MAX_COMPUTE_LOCAL_INVOCATIONS */
    /* reuse GL_MAX_COMPUTE_WORK_GROUP_COUNT */
    /* reuse GL_MAX_COMPUTE_WORK_GROUP_SIZE */
    /* reuse GL_COMPUTE_LOCAL_WORK_SIZE */
    /* reuse GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER */
    /* reuse GL_DISPATCH_INDIRECT_BUFFER */
    /* reuse GL_DISPATCH_INDIRECT_BUFFER_BINDING */
    /* Reuse tokens from ARB_copy_image (none) */
    /* Reuse tokens from KHR_debug */
    /* reuse GL_DEBUG_OUTPUT_SYNCHRONOUS */
    /* reuse GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH */
    /* reuse GL_DEBUG_CALLBACK_FUNCTION */
    /* reuse GL_DEBUG_CALLBACK_USER_PARAM */
    /* reuse GL_DEBUG_SOURCE_API */
    /* reuse GL_DEBUG_SOURCE_WINDOW_SYSTEM */
    /* reuse GL_DEBUG_SOURCE_SHADER_COMPILER */
    /* reuse GL_DEBUG_SOURCE_THIRD_PARTY */
    /* reuse GL_DEBUG_SOURCE_APPLICATION */
    /* reuse GL_DEBUG_SOURCE_OTHER */
    /* reuse GL_DEBUG_TYPE_ERROR */
    /* reuse GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR */
    /* reuse GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR */
    /* reuse GL_DEBUG_TYPE_PORTABILITY */
    /* reuse GL_DEBUG_TYPE_PERFORMANCE */
    /* reuse GL_DEBUG_TYPE_OTHER */
    /* reuse GL_MAX_DEBUG_MESSAGE_LENGTH */
    /* reuse GL_MAX_DEBUG_LOGGED_MESSAGES */
    /* reuse GL_DEBUG_LOGGED_MESSAGES */
    /* reuse GL_DEBUG_SEVERITY_HIGH */
    /* reuse GL_DEBUG_SEVERITY_MEDIUM */
    /* reuse GL_DEBUG_SEVERITY_LOW */
    /* reuse GL_DEBUG_TYPE_MARKER */
    /* reuse GL_DEBUG_TYPE_PUSH_GROUP */
    /* reuse GL_DEBUG_TYPE_POP_GROUP */
    /* reuse GL_DEBUG_SEVERITY_NOTIFICATION */
    /* reuse GL_MAX_DEBUG_GROUP_STACK_DEPTH */
    /* reuse GL_DEBUG_GROUP_STACK_DEPTH */
    /* reuse GL_BUFFER */
    /* reuse GL_SHADER */
    /* reuse GL_PROGRAM */
    /* reuse GL_QUERY */
    /* reuse GL_PROGRAM_PIPELINE */
    /* reuse GL_SAMPLER */
    /* reuse GL_DISPLAY_LIST */
    /* reuse GL_MAX_LABEL_LENGTH */
    /* reuse GL_DEBUG_OUTPUT */
    /* reuse GL_CONTEXT_FLAG_DEBUG_BIT */
    /* reuse GL_STACK_UNDERFLOW */
    /* reuse GL_STACK_OVERFLOW */
    /* Reuse tokens from ARB_explicit_uniform_location */
    /* reuse GL_MAX_UNIFORM_LOCATIONS */
    /* Reuse tokens from ARB_framebuffer_no_attachments */
    /* reuse GL_FRAMEBUFFER_DEFAULT_WIDTH */
    /* reuse GL_FRAMEBUFFER_DEFAULT_HEIGHT */
    /* reuse GL_FRAMEBUFFER_DEFAULT_LAYERS */
    /* reuse GL_FRAMEBUFFER_DEFAULT_SAMPLES */
    /* reuse GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS */
    /* reuse GL_MAX_FRAMEBUFFER_WIDTH */
    /* reuse GL_MAX_FRAMEBUFFER_HEIGHT */
    /* reuse GL_MAX_FRAMEBUFFER_LAYERS */
    /* reuse GL_MAX_FRAMEBUFFER_SAMPLES */
    /* Reuse tokens from ARB_internalformat_query2 */
    /* reuse GL_INTERNALFORMAT_SUPPORTED */
    /* reuse GL_INTERNALFORMAT_PREFERRED */
    /* reuse GL_INTERNALFORMAT_RED_SIZE */
    /* reuse GL_INTERNALFORMAT_GREEN_SIZE */
    /* reuse GL_INTERNALFORMAT_BLUE_SIZE */
    /* reuse GL_INTERNALFORMAT_ALPHA_SIZE */
    /* reuse GL_INTERNALFORMAT_DEPTH_SIZE */
    /* reuse GL_INTERNALFORMAT_STENCIL_SIZE */
    /* reuse GL_INTERNALFORMAT_SHARED_SIZE */
    /* reuse GL_INTERNALFORMAT_RED_TYPE */
    /* reuse GL_INTERNALFORMAT_GREEN_TYPE */
    /* reuse GL_INTERNALFORMAT_BLUE_TYPE */
    /* reuse GL_INTERNALFORMAT_ALPHA_TYPE */
    /* reuse GL_INTERNALFORMAT_DEPTH_TYPE */
    /* reuse GL_INTERNALFORMAT_STENCIL_TYPE */
    /* reuse GL_MAX_WIDTH */
    /* reuse GL_MAX_HEIGHT */
    /* reuse GL_MAX_DEPTH */
    /* reuse GL_MAX_LAYERS */
    /* reuse GL_MAX_COMBINED_DIMENSIONS */
    /* reuse GL_COLOR_COMPONENTS */
    /* reuse GL_DEPTH_COMPONENTS */
    /* reuse GL_STENCIL_COMPONENTS */
    /* reuse GL_COLOR_RENDERABLE */
    /* reuse GL_DEPTH_RENDERABLE */
    /* reuse GL_STENCIL_RENDERABLE */
    /* reuse GL_FRAMEBUFFER_RENDERABLE */
    /* reuse GL_FRAMEBUFFER_RENDERABLE_LAYERED */
    /* reuse GL_FRAMEBUFFER_BLEND */
    /* reuse GL_READ_PIXELS */
    /* reuse GL_READ_PIXELS_FORMAT */
    /* reuse GL_READ_PIXELS_TYPE */
    /* reuse GL_TEXTURE_IMAGE_FORMAT */
    /* reuse GL_TEXTURE_IMAGE_TYPE */
    /* reuse GL_GET_TEXTURE_IMAGE_FORMAT */
    /* reuse GL_GET_TEXTURE_IMAGE_TYPE */
    /* reuse GL_MIPMAP */
    /* reuse GL_MANUAL_GENERATE_MIPMAP */
    /* reuse GL_AUTO_GENERATE_MIPMAP */
    /* reuse GL_COLOR_ENCODING */
    /* reuse GL_SRGB_READ */
    /* reuse GL_SRGB_WRITE */
    /* reuse GL_FILTER */
    /* reuse GL_VERTEX_TEXTURE */
    /* reuse GL_TESS_CONTROL_TEXTURE */
    /* reuse GL_TESS_EVALUATION_TEXTURE */
    /* reuse GL_GEOMETRY_TEXTURE */
    /* reuse GL_FRAGMENT_TEXTURE */
    /* reuse GL_COMPUTE_TEXTURE */
    /* reuse GL_TEXTURE_SHADOW */
    /* reuse GL_TEXTURE_GATHER */
    /* reuse GL_TEXTURE_GATHER_SHADOW */
    /* reuse GL_SHADER_IMAGE_LOAD */
    /* reuse GL_SHADER_IMAGE_STORE */
    /* reuse GL_SHADER_IMAGE_ATOMIC */
    /* reuse GL_IMAGE_TEXEL_SIZE */
    /* reuse GL_IMAGE_COMPATIBILITY_CLASS */
    /* reuse GL_IMAGE_PIXEL_FORMAT */
    /* reuse GL_IMAGE_PIXEL_TYPE */
    /* reuse GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST */
    /* reuse GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST */
    /* reuse GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE */
    /* reuse GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE */
    /* reuse GL_TEXTURE_COMPRESSED_BLOCK_WIDTH */
    /* reuse GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT */
    /* reuse GL_TEXTURE_COMPRESSED_BLOCK_SIZE */
    /* reuse GL_CLEAR_BUFFER */
    /* reuse GL_TEXTURE_VIEW */
    /* reuse GL_VIEW_COMPATIBILITY_CLASS */
    /* reuse GL_FULL_SUPPORT */
    /* reuse GL_CAVEAT_SUPPORT */
    /* reuse GL_IMAGE_CLASS_4_X_32 */
    /* reuse GL_IMAGE_CLASS_2_X_32 */
    /* reuse GL_IMAGE_CLASS_1_X_32 */
    /* reuse GL_IMAGE_CLASS_4_X_16 */
    /* reuse GL_IMAGE_CLASS_2_X_16 */
    /* reuse GL_IMAGE_CLASS_1_X_16 */
    /* reuse GL_IMAGE_CLASS_4_X_8 */
    /* reuse GL_IMAGE_CLASS_2_X_8 */
    /* reuse GL_IMAGE_CLASS_1_X_8 */
    /* reuse GL_IMAGE_CLASS_11_11_10 */
    /* reuse GL_IMAGE_CLASS_10_10_10_2 */
    /* reuse GL_VIEW_CLASS_128_BITS */
    /* reuse GL_VIEW_CLASS_96_BITS */
    /* reuse GL_VIEW_CLASS_64_BITS */
    /* reuse GL_VIEW_CLASS_48_BITS */
    /* reuse GL_VIEW_CLASS_32_BITS */
    /* reuse GL_VIEW_CLASS_24_BITS */
    /* reuse GL_VIEW_CLASS_16_BITS */
    /* reuse GL_VIEW_CLASS_8_BITS */
    /* reuse GL_VIEW_CLASS_S3TC_DXT1_RGB */
    /* reuse GL_VIEW_CLASS_S3TC_DXT1_RGBA */
    /* reuse GL_VIEW_CLASS_S3TC_DXT3_RGBA */
    /* reuse GL_VIEW_CLASS_S3TC_DXT5_RGBA */
    /* reuse GL_VIEW_CLASS_RGTC1_RED */
    /* reuse GL_VIEW_CLASS_RGTC2_RG */
    /* reuse GL_VIEW_CLASS_BPTC_UNORM */
    /* reuse GL_VIEW_CLASS_BPTC_FLOAT */
    /* Reuse tokens from ARB_invalidate_subdata (none) */
    /* Reuse tokens from ARB_multi_draw_indirect (none) */
    /* Reuse tokens from ARB_program_interface_query */
    /* reuse GL_UNIFORM */
    /* reuse GL_UNIFORM_BLOCK */
    /* reuse GL_PROGRAM_INPUT */
    /* reuse GL_PROGRAM_OUTPUT */
    /* reuse GL_BUFFER_VARIABLE */
    /* reuse GL_SHADER_STORAGE_BLOCK */
    /* reuse GL_VERTEX_SUBROUTINE */
    /* reuse GL_TESS_CONTROL_SUBROUTINE */
    /* reuse GL_TESS_EVALUATION_SUBROUTINE */
    /* reuse GL_GEOMETRY_SUBROUTINE */
    /* reuse GL_FRAGMENT_SUBROUTINE */
    /* reuse GL_COMPUTE_SUBROUTINE */
    /* reuse GL_VERTEX_SUBROUTINE_UNIFORM */
    /* reuse GL_TESS_CONTROL_SUBROUTINE_UNIFORM */
    /* reuse GL_TESS_EVALUATION_SUBROUTINE_UNIFORM */
    /* reuse GL_GEOMETRY_SUBROUTINE_UNIFORM */
    /* reuse GL_FRAGMENT_SUBROUTINE_UNIFORM */
    /* reuse GL_COMPUTE_SUBROUTINE_UNIFORM */
    /* reuse GL_TRANSFORM_FEEDBACK_VARYING */
    /* reuse GL_ACTIVE_RESOURCES */
    /* reuse GL_MAX_NAME_LENGTH */
    /* reuse GL_MAX_NUM_ACTIVE_VARIABLES */
    /* reuse GL_MAX_NUM_COMPATIBLE_SUBROUTINES */
    /* reuse GL_NAME_LENGTH */
    /* reuse GL_TYPE */
    /* reuse GL_ARRAY_SIZE */
    /* reuse GL_OFFSET */
    /* reuse GL_BLOCK_INDEX */
    /* reuse GL_ARRAY_STRIDE */
    /* reuse GL_MATRIX_STRIDE */
    /* reuse GL_IS_ROW_MAJOR */
    /* reuse GL_ATOMIC_COUNTER_BUFFER_INDEX */
    /* reuse GL_BUFFER_BINDING */
    /* reuse GL_BUFFER_DATA_SIZE */
    /* reuse GL_NUM_ACTIVE_VARIABLES */
    /* reuse GL_ACTIVE_VARIABLES */
    /* reuse GL_REFERENCED_BY_VERTEX_SHADER */
    /* reuse GL_REFERENCED_BY_TESS_CONTROL_SHADER */
    /* reuse GL_REFERENCED_BY_TESS_EVALUATION_SHADER */
    /* reuse GL_REFERENCED_BY_GEOMETRY_SHADER */
    /* reuse GL_REFERENCED_BY_FRAGMENT_SHADER */
    /* reuse GL_REFERENCED_BY_COMPUTE_SHADER */
    /* reuse GL_TOP_LEVEL_ARRAY_SIZE */
    /* reuse GL_TOP_LEVEL_ARRAY_STRIDE */
    /* reuse GL_LOCATION */
    /* reuse GL_LOCATION_INDEX */
    /* reuse GL_IS_PER_PATCH */
    /* Reuse tokens from ARB_robust_buffer_access_behavior (none) */
    /* Reuse tokens from ARB_shader_storage_buffer_object */
    /* reuse GL_SHADER_STORAGE_BUFFER */
    /* reuse GL_SHADER_STORAGE_BUFFER_BINDING */
    /* reuse GL_SHADER_STORAGE_BUFFER_START */
    /* reuse GL_SHADER_STORAGE_BUFFER_SIZE */
    /* reuse GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS */
    /* reuse GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS */
    /* reuse GL_MAX_SHADER_STORAGE_BLOCK_SIZE */
    /* reuse GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT */
    /* reuse GL_SHADER_STORAGE_BARRIER_BIT */
    /* reuse GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES */
    /* Reuse tokens from ARB_stencil_texturing */
    /* reuse GL_DEPTH_STENCIL_TEXTURE_MODE */
    /* Reuse tokens from ARB_texture_buffer_range */
    /* reuse GL_TEXTURE_BUFFER_OFFSET */
    /* reuse GL_TEXTURE_BUFFER_SIZE */
    /* reuse GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT */
    /* Reuse tokens from ARB_texture_query_levels (none) */
    /* Reuse tokens from ARB_texture_storage_multisample (none) */
    /* Reuse tokens from ARB_texture_view */
    /* reuse GL_TEXTURE_VIEW_MIN_LEVEL */
    /* reuse GL_TEXTURE_VIEW_NUM_LEVELS */
    /* reuse GL_TEXTURE_VIEW_MIN_LAYER */
    /* reuse GL_TEXTURE_VIEW_NUM_LAYERS */
    /* reuse GL_TEXTURE_IMMUTABLE_LEVELS */
    /* Reuse tokens from ARB_vertex_attrib_binding */
    /* reuse GL_VERTEX_ATTRIB_BINDING */
    /* reuse GL_VERTEX_ATTRIB_RELATIVE_OFFSET */
    /* reuse GL_VERTEX_BINDING_DIVISOR */
    /* reuse GL_VERTEX_BINDING_OFFSET */
    /* reuse GL_VERTEX_BINDING_STRIDE */
    /* reuse GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET */
    /* reuse GL_MAX_VERTEX_ATTRIB_BINDINGS */
#endif

#ifndef GL_ARB_depth_buffer_float
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_DEPTH32F_STENCIL8              0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#endif

#ifndef GL_ARB_framebuffer_object
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAX_RENDERBUFFER_SIZE          0x84E8
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING       GL_FRAMEBUFFER_BINDING
#define GL_RENDERBUFFER_BINDING           0x8CA7
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING       0x8CAA
#define GL_RENDERBUFFER_SAMPLES           0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49
#define GL_RENDERBUFFER_RED_SIZE          0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE        0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE         0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE        0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE        0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE      0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES                    0x8D57
#endif

#ifndef GL_ARB_framebuffer_sRGB
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#endif

#ifndef GL_ARB_half_float_vertex
#define GL_HALF_FLOAT                     0x140B
#endif

#ifndef GL_ARB_map_buffer_range
#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#endif

#ifndef GL_ARB_texture_compression_rgtc
#define GL_COMPRESSED_RED_RGTC1           0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1    0x8DBC
#define GL_COMPRESSED_RG_RGTC2            0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2     0x8DBE
#endif

#ifndef GL_ARB_texture_rg
#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C
#endif

#ifndef GL_ARB_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#endif

#ifndef GL_ARB_uniform_buffer_object
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_UNIFORM_BUFFER_BINDING         0x8A28
#define GL_UNIFORM_BUFFER_START           0x8A29
#define GL_UNIFORM_BUFFER_SIZE            0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS      0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS    0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS    0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS    0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS    0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE         0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS          0x8A36
#define GL_UNIFORM_TYPE                   0x8A37
#define GL_UNIFORM_SIZE                   0x8A38
#define GL_UNIFORM_NAME_LENGTH            0x8A39
#define GL_UNIFORM_BLOCK_INDEX            0x8A3A
#define GL_UNIFORM_OFFSET                 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE           0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE          0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR           0x8A3E
#define GL_UNIFORM_BLOCK_BINDING          0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE        0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH      0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS  0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX                  0xFFFFFFFFu
#endif

#ifndef GL_ARB_copy_buffer
#define GL_COPY_READ_BUFFER_BINDING       0x8F36
#define GL_COPY_READ_BUFFER               GL_COPY_READ_BUFFER_BINDING
#define GL_COPY_WRITE_BUFFER_BINDING      0x8F37
#define GL_COPY_WRITE_BUFFER              GL_COPY_WRITE_BUFFER_BINDING
#endif

#ifndef GL_ARB_depth_clamp
#define GL_DEPTH_CLAMP                    0x864F
#endif

#ifndef GL_ARB_draw_elements_base_vertex
#endif

#ifndef GL_ARB_fragment_coord_conventions
#endif

#ifndef GL_ARB_provoking_vertex
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_LAST_VERTEX_CONVENTION         0x8E4E
#define GL_PROVOKING_VERTEX               0x8E4F
#endif

#ifndef GL_ARB_seamless_cube_map
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#endif

#ifndef GL_ARB_sync
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#endif

#ifndef GL_ARB_texture_multisample
#define GL_SAMPLE_POSITION                0x8E50
#define GL_SAMPLE_MASK                    0x8E51
#define GL_SAMPLE_MASK_VALUE              0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS          0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE   0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY   0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES                0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE         0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE     0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY   0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_MAX_INTEGER_SAMPLES            0x9110
#endif

#ifndef GL_ARB_vertex_array_bgra
    /* reuse GL_BGRA */
#endif

#ifndef GL_ARB_draw_buffers_blend
#endif

#ifndef GL_ARB_sample_shading
#define GL_SAMPLE_SHADING_ARB             0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE_ARB   0x8C37
#endif

#ifndef GL_ARB_texture_cube_map_array
#define GL_TEXTURE_CUBE_MAP_ARRAY_ARB     0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_ARB 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB 0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY_ARB     0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB 0x900F
#endif

#ifndef GL_ARB_texture_gather
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET_ARB 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET_ARB 0x8E5F
#define GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS_ARB 0x8F9F
#endif

#ifndef GL_ARB_texture_query_lod
#endif

#ifndef GL_ARB_shading_language_include
#define GL_SHADER_INCLUDE_ARB             0x8DAE
#define GL_NAMED_STRING_LENGTH_ARB        0x8DE9
#define GL_NAMED_STRING_TYPE_ARB          0x8DEA
#endif

#ifndef GL_ARB_texture_compression_bptc
#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
#endif

#ifndef GL_ARB_blend_func_extended
#define GL_SRC1_COLOR                     0x88F9
    /* reuse GL_SRC1_ALPHA */
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#endif

#ifndef GL_ARB_explicit_attrib_location
#endif

#ifndef GL_ARB_occlusion_query2
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#endif

#ifndef GL_ARB_sampler_objects
#define GL_SAMPLER_BINDING                0x8919
#endif

#ifndef GL_ARB_shader_bit_encoding
#endif

#ifndef GL_ARB_texture_rgb10_a2ui
#define GL_RGB10_A2UI                     0x906F
#endif

#ifndef GL_ARB_texture_swizzle
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#endif

#ifndef GL_ARB_timer_query
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#endif

#ifndef GL_ARB_vertex_type_2_10_10_10_rev
    /* reuse GL_UNSIGNED_INT_2_10_10_10_REV */
#define GL_INT_2_10_10_10_REV             0x8D9F
#endif

#ifndef GL_ARB_draw_indirect
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#endif

#ifndef GL_ARB_gpu_shader5
#define GL_GEOMETRY_SHADER_INVOCATIONS    0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
    /* reuse GL_MAX_VERTEX_STREAMS */
#endif

#ifndef GL_ARB_gpu_shader_fp64
    /* reuse GL_DOUBLE */
#define GL_DOUBLE_VEC2                    0x8FFC
#define GL_DOUBLE_VEC3                    0x8FFD
#define GL_DOUBLE_VEC4                    0x8FFE
#define GL_DOUBLE_MAT2                    0x8F46
#define GL_DOUBLE_MAT3                    0x8F47
#define GL_DOUBLE_MAT4                    0x8F48
#define GL_DOUBLE_MAT2x3                  0x8F49
#define GL_DOUBLE_MAT2x4                  0x8F4A
#define GL_DOUBLE_MAT3x2                  0x8F4B
#define GL_DOUBLE_MAT3x4                  0x8F4C
#define GL_DOUBLE_MAT4x2                  0x8F4D
#define GL_DOUBLE_MAT4x3                  0x8F4E
#endif

#ifndef GL_ARB_shader_subroutine
#define GL_ACTIVE_SUBROUTINES             0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS     0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH   0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES                0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES     0x8E4A
#define GL_COMPATIBLE_SUBROUTINES         0x8E4B
    /* reuse GL_UNIFORM_SIZE */
    /* reuse GL_UNIFORM_NAME_LENGTH */
#endif

#ifndef GL_ARB_tessellation_shader
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL      0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL      0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES   0x8E75
#define GL_TESS_GEN_MODE                  0x8E76
#define GL_TESS_GEN_SPACING               0x8E77
#define GL_TESS_GEN_VERTEX_ORDER          0x8E78
#define GL_TESS_GEN_POINT_MODE            0x8E79
    /* reuse GL_TRIANGLES */
    /* reuse GL_QUADS */
#define GL_ISOLINES                       0x8E7A
    /* reuse GL_EQUAL */
#define GL_FRACTIONAL_ODD                 0x8E7B
#define GL_FRACTIONAL_EVEN                0x8E7C
    /* reuse GL_CCW */
    /* reuse GL_CW */
#define GL_MAX_PATCH_VERTICES             0x8E7D
#define GL_MAX_TESS_GEN_LEVEL             0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS      0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#endif

#ifndef GL_ARB_texture_buffer_object_rgb32
    /* reuse GL_RGB32F */
    /* reuse GL_RGB32UI */
    /* reuse GL_RGB32I */
#endif

#ifndef GL_ARB_transform_feedback2
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TRANSFORM_FEEDBACK_PAUSED      0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED GL_TRANSFORM_FEEDBACK_PAUSED
#define GL_TRANSFORM_FEEDBACK_ACTIVE      0x8E24
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE GL_TRANSFORM_FEEDBACK_ACTIVE
#define GL_TRANSFORM_FEEDBACK_BINDING     0x8E25
#endif

#ifndef GL_ARB_transform_feedback3
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#define GL_MAX_VERTEX_STREAMS             0x8E71
#endif

#ifndef GL_ARB_ES2_compatibility
#define GL_FIXED                          0x140C
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_LOW_FLOAT                      0x8DF0
#define GL_MEDIUM_FLOAT                   0x8DF1
#define GL_HIGH_FLOAT                     0x8DF2
#define GL_LOW_INT                        0x8DF3
#define GL_MEDIUM_INT                     0x8DF4
#define GL_HIGH_INT                       0x8DF5
#define GL_SHADER_COMPILER                0x8DFA
#define GL_SHADER_BINARY_FORMATS          0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS      0x8DF9
#define GL_MAX_VERTEX_UNIFORM_VECTORS     0x8DFB
#define GL_MAX_VARYING_VECTORS            0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS   0x8DFD
#define GL_RGB565                         0x8D62
#endif

#ifndef GL_ARB_get_program_binary
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH          0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS     0x87FE
#define GL_PROGRAM_BINARY_FORMATS         0x87FF
#endif

#ifndef GL_ARB_separate_shader_objects
#define GL_VERTEX_SHADER_BIT              0x00000001
#define GL_FRAGMENT_SHADER_BIT            0x00000002
#define GL_GEOMETRY_SHADER_BIT            0x00000004
#define GL_TESS_CONTROL_SHADER_BIT        0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT     0x00000010
#define GL_ALL_SHADER_BITS                0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE              0x8258
#define GL_ACTIVE_PROGRAM                 0x8259
#define GL_PROGRAM_PIPELINE_BINDING       0x825A
#endif

#ifndef GL_ARB_shader_precision
#endif

#ifndef GL_ARB_vertex_attrib_64bit
    /* reuse GL_RGB32I */
    /* reuse GL_DOUBLE_VEC2 */
    /* reuse GL_DOUBLE_VEC3 */
    /* reuse GL_DOUBLE_VEC4 */
    /* reuse GL_DOUBLE_MAT2 */
    /* reuse GL_DOUBLE_MAT3 */
    /* reuse GL_DOUBLE_MAT4 */
    /* reuse GL_DOUBLE_MAT2x3 */
    /* reuse GL_DOUBLE_MAT2x4 */
    /* reuse GL_DOUBLE_MAT3x2 */
    /* reuse GL_DOUBLE_MAT3x4 */
    /* reuse GL_DOUBLE_MAT4x2 */
    /* reuse GL_DOUBLE_MAT4x3 */
#endif

#ifndef GL_ARB_viewport_array
    /* reuse GL_SCISSOR_BOX */
    /* reuse GL_VIEWPORT */
    /* reuse GL_DEPTH_RANGE */
    /* reuse GL_SCISSOR_TEST */
#define GL_MAX_VIEWPORTS                  0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS         0x825C
#define GL_VIEWPORT_BOUNDS_RANGE          0x825D
#define GL_LAYER_PROVOKING_VERTEX         0x825E
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_UNDEFINED_VERTEX               0x8260
    /* reuse GL_FIRST_VERTEX_CONVENTION */
    /* reuse GL_LAST_VERTEX_CONVENTION */
    /* reuse GL_PROVOKING_VERTEX */
#endif

#ifndef GL_ARB_cl_event
#define GL_SYNC_CL_EVENT_ARB              0x8240
#define GL_SYNC_CL_EVENT_COMPLETE_ARB     0x8241
#endif

#ifndef GL_ARB_debug_output
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB   0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_ARB    0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB  0x8245
#define GL_DEBUG_SOURCE_API_ARB           0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB   0x8249
#define GL_DEBUG_SOURCE_APPLICATION_ARB   0x824A
#define GL_DEBUG_SOURCE_OTHER_ARB         0x824B
#define GL_DEBUG_TYPE_ERROR_ARB           0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_ARB     0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_ARB     0x8250
#define GL_DEBUG_TYPE_OTHER_ARB           0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB   0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB  0x9144
#define GL_DEBUG_LOGGED_MESSAGES_ARB      0x9145
#define GL_DEBUG_SEVERITY_HIGH_ARB        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB      0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB         0x9148
#endif

#ifndef GL_ARB_robustness
    /* reuse GL_NO_ERROR */
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GL_LOSE_CONTEXT_ON_RESET_ARB      0x8252
#define GL_GUILTY_CONTEXT_RESET_ARB       0x8253
#define GL_INNOCENT_CONTEXT_RESET_ARB     0x8254
#define GL_UNKNOWN_CONTEXT_RESET_ARB      0x8255
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_NO_RESET_NOTIFICATION_ARB      0x8261
#endif

#ifndef GL_ARB_shader_stencil_export
#endif

#ifndef GL_ARB_base_instance
#endif

#ifndef GL_ARB_shading_language_420pack
#endif

#ifndef GL_ARB_transform_feedback_instanced
#endif

#ifndef GL_ARB_compressed_texture_pixel_storage
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH  0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH  0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE   0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH    0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT   0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH    0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE     0x912E
#endif

#ifndef GL_ARB_conservative_depth
#endif

#ifndef GL_ARB_internalformat_query
#define GL_NUM_SAMPLE_COUNTS              0x9380
#endif

#ifndef GL_ARB_map_buffer_alignment
#define GL_MIN_MAP_BUFFER_ALIGNMENT       0x90BC
#endif

#ifndef GL_ARB_shader_atomic_counters
#define GL_ATOMIC_COUNTER_BUFFER          0x92C0
#define GL_ATOMIC_COUNTER_BUFFER_BINDING  0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START    0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE     0x92C3
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS     0x92D2
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS   0x92D5
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS   0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS   0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS  0x92D9
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_UNSIGNED_INT_ATOMIC_COUNTER    0x92DB
#endif

#ifndef GL_ARB_shader_image_load_store
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT      0x00000002
#define GL_UNIFORM_BARRIER_BIT            0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT      0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT            0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT       0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT     0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT      0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT        0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT     0x00001000
#define GL_ALL_BARRIER_BITS               0xFFFFFFFF
#define GL_MAX_IMAGE_UNITS                0x8F38
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_IMAGE_BINDING_NAME             0x8F3A
#define GL_IMAGE_BINDING_LEVEL            0x8F3B
#define GL_IMAGE_BINDING_LAYERED          0x8F3C
#define GL_IMAGE_BINDING_LAYER            0x8F3D
#define GL_IMAGE_BINDING_ACCESS           0x8F3E
#define GL_IMAGE_1D                       0x904C
#define GL_IMAGE_2D                       0x904D
#define GL_IMAGE_3D                       0x904E
#define GL_IMAGE_2D_RECT                  0x904F
#define GL_IMAGE_CUBE                     0x9050
#define GL_IMAGE_BUFFER                   0x9051
#define GL_IMAGE_1D_ARRAY                 0x9052
#define GL_IMAGE_2D_ARRAY                 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY           0x9054
#define GL_IMAGE_2D_MULTISAMPLE           0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY     0x9056
#define GL_INT_IMAGE_1D                   0x9057
#define GL_INT_IMAGE_2D                   0x9058
#define GL_INT_IMAGE_3D                   0x9059
#define GL_INT_IMAGE_2D_RECT              0x905A
#define GL_INT_IMAGE_CUBE                 0x905B
#define GL_INT_IMAGE_BUFFER               0x905C
#define GL_INT_IMAGE_1D_ARRAY             0x905D
#define GL_INT_IMAGE_2D_ARRAY             0x905E
#define GL_INT_IMAGE_CUBE_MAP_ARRAY       0x905F
#define GL_INT_IMAGE_2D_MULTISAMPLE       0x9060
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_UNSIGNED_INT_IMAGE_1D          0x9062
#define GL_UNSIGNED_INT_IMAGE_2D          0x9063
#define GL_UNSIGNED_INT_IMAGE_3D          0x9064
#define GL_UNSIGNED_INT_IMAGE_2D_RECT     0x9065
#define GL_UNSIGNED_INT_IMAGE_CUBE        0x9066
#define GL_UNSIGNED_INT_IMAGE_BUFFER      0x9067
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY    0x9068
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY    0x9069
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_IMAGE_SAMPLES              0x906D
#define GL_IMAGE_BINDING_FORMAT           0x906E
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_MAX_VERTEX_IMAGE_UNIFORMS      0x90CA
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS    0x90CD
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS    0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS    0x90CF
#endif

#ifndef GL_ARB_shading_language_packing
#endif

#ifndef GL_ARB_texture_storage
#define GL_TEXTURE_IMMUTABLE_FORMAT       0x912F
#endif

#ifndef GL_KHR_texture_compression_astc_ldr
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#endif

#ifndef GL_KHR_debug
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_DISPLAY_LIST                   0x82E7
    /* DISPLAY_LIST used in compatibility profile only */
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
    /* reuse GL_STACK_UNDERFLOW */
    /* reuse GL_STACK_OVERFLOW */
#endif

#ifndef GL_ARB_arrays_of_arrays
#endif

#ifndef GL_ARB_clear_buffer_object
#endif

#ifndef GL_ARB_compute_shader
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS     0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS     0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS    0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_LOCAL_INVOCATIONS  0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_COMPUTE_LOCAL_WORK_SIZE        0x8267
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_DISPATCH_INDIRECT_BUFFER       0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT             0x00000020
#endif

#ifndef GL_ARB_copy_image
#endif

#ifndef GL_ARB_texture_view
#define GL_TEXTURE_VIEW_MIN_LEVEL         0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS        0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER         0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS        0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS       0x82DF
#endif

#ifndef GL_ARB_vertex_attrib_binding
#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS     0x82DA
#endif

#ifndef GL_ARB_robustness_isolation
#endif

#ifndef GL_ARB_ES3_compatibility
#define GL_COMPRESSED_RGB8_ETC2           0x9274
#define GL_COMPRESSED_SRGB8_ETC2          0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_R11_EAC             0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC      0x9271
#define GL_COMPRESSED_RG11_EAC            0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC     0x9273
#define GL_PRIMITIVE_RESTART_FIXED_INDEX  0x8D69
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_MAX_ELEMENT_INDEX              0x8D6B
#endif

#ifndef GL_ARB_explicit_uniform_location
#define GL_MAX_UNIFORM_LOCATIONS          0x826E
#endif

#ifndef GL_ARB_fragment_layer_viewport
#endif

#ifndef GL_ARB_framebuffer_no_attachments
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#define GL_MAX_FRAMEBUFFER_LAYERS         0x9317
#define GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
#endif

#ifndef GL_ARB_internalformat_query2
    /* reuse GL_IMAGE_FORMAT_COMPATIBILITY_TYPE */
    /* reuse GL_NUM_SAMPLE_COUNTS */
    /* reuse GL_RENDERBUFFER */
    /* reuse GL_SAMPLES */
    /* reuse GL_TEXTURE_1D */
    /* reuse GL_TEXTURE_1D_ARRAY */
    /* reuse GL_TEXTURE_2D */
    /* reuse GL_TEXTURE_2D_ARRAY */
    /* reuse GL_TEXTURE_3D */
    /* reuse GL_TEXTURE_CUBE_MAP */
    /* reuse GL_TEXTURE_CUBE_MAP_ARRAY */
    /* reuse GL_TEXTURE_RECTANGLE */
    /* reuse GL_TEXTURE_BUFFER */
    /* reuse GL_TEXTURE_2D_MULTISAMPLE */
    /* reuse GL_TEXTURE_2D_MULTISAMPLE_ARRAY */
    /* reuse GL_TEXTURE_COMPRESSED */
#define GL_INTERNALFORMAT_SUPPORTED       0x826F
#define GL_INTERNALFORMAT_PREFERRED       0x8270
#define GL_INTERNALFORMAT_RED_SIZE        0x8271
#define GL_INTERNALFORMAT_GREEN_SIZE      0x8272
#define GL_INTERNALFORMAT_BLUE_SIZE       0x8273
#define GL_INTERNALFORMAT_ALPHA_SIZE      0x8274
#define GL_INTERNALFORMAT_DEPTH_SIZE      0x8275
#define GL_INTERNALFORMAT_STENCIL_SIZE    0x8276
#define GL_INTERNALFORMAT_SHARED_SIZE     0x8277
#define GL_INTERNALFORMAT_RED_TYPE        0x8278
#define GL_INTERNALFORMAT_GREEN_TYPE      0x8279
#define GL_INTERNALFORMAT_BLUE_TYPE       0x827A
#define GL_INTERNALFORMAT_ALPHA_TYPE      0x827B
#define GL_INTERNALFORMAT_DEPTH_TYPE      0x827C
#define GL_INTERNALFORMAT_STENCIL_TYPE    0x827D
#define GL_MAX_WIDTH                      0x827E
#define GL_MAX_HEIGHT                     0x827F
#define GL_MAX_DEPTH                      0x8280
#define GL_MAX_LAYERS                     0x8281
#define GL_MAX_COMBINED_DIMENSIONS        0x8282
#define GL_COLOR_COMPONENTS               0x8283
#define GL_DEPTH_COMPONENTS               0x8284
#define GL_STENCIL_COMPONENTS             0x8285
#define GL_COLOR_RENDERABLE               0x8286
#define GL_DEPTH_RENDERABLE               0x8287
#define GL_STENCIL_RENDERABLE             0x8288
#define GL_FRAMEBUFFER_RENDERABLE         0x8289
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_FRAMEBUFFER_BLEND              0x828B
#define GL_READ_PIXELS                    0x828C
#define GL_READ_PIXELS_FORMAT             0x828D
#define GL_READ_PIXELS_TYPE               0x828E
#define GL_TEXTURE_IMAGE_FORMAT           0x828F
#define GL_TEXTURE_IMAGE_TYPE             0x8290
#define GL_GET_TEXTURE_IMAGE_FORMAT       0x8291
#define GL_GET_TEXTURE_IMAGE_TYPE         0x8292
#define GL_MIPMAP                         0x8293
#define GL_MANUAL_GENERATE_MIPMAP         0x8294
#define GL_AUTO_GENERATE_MIPMAP           0x8295
#define GL_COLOR_ENCODING                 0x8296
#define GL_SRGB_READ                      0x8297
#define GL_SRGB_WRITE                     0x8298
#define GL_SRGB_DECODE_ARB                0x8299
#define GL_FILTER                         0x829A
#define GL_VERTEX_TEXTURE                 0x829B
#define GL_TESS_CONTROL_TEXTURE           0x829C
#define GL_TESS_EVALUATION_TEXTURE        0x829D
#define GL_GEOMETRY_TEXTURE               0x829E
#define GL_FRAGMENT_TEXTURE               0x829F
#define GL_COMPUTE_TEXTURE                0x82A0
#define GL_TEXTURE_SHADOW                 0x82A1
#define GL_TEXTURE_GATHER                 0x82A2
#define GL_TEXTURE_GATHER_SHADOW          0x82A3
#define GL_SHADER_IMAGE_LOAD              0x82A4
#define GL_SHADER_IMAGE_STORE             0x82A5
#define GL_SHADER_IMAGE_ATOMIC            0x82A6
#define GL_IMAGE_TEXEL_SIZE               0x82A7
#define GL_IMAGE_COMPATIBILITY_CLASS      0x82A8
#define GL_IMAGE_PIXEL_FORMAT             0x82A9
#define GL_IMAGE_PIXEL_TYPE               0x82AA
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE  0x82B3
#define GL_CLEAR_BUFFER                   0x82B4
#define GL_TEXTURE_VIEW                   0x82B5
#define GL_VIEW_COMPATIBILITY_CLASS       0x82B6
#define GL_FULL_SUPPORT                   0x82B7
#define GL_CAVEAT_SUPPORT                 0x82B8
#define GL_IMAGE_CLASS_4_X_32             0x82B9
#define GL_IMAGE_CLASS_2_X_32             0x82BA
#define GL_IMAGE_CLASS_1_X_32             0x82BB
#define GL_IMAGE_CLASS_4_X_16             0x82BC
#define GL_IMAGE_CLASS_2_X_16             0x82BD
#define GL_IMAGE_CLASS_1_X_16             0x82BE
#define GL_IMAGE_CLASS_4_X_8              0x82BF
#define GL_IMAGE_CLASS_2_X_8              0x82C0
#define GL_IMAGE_CLASS_1_X_8              0x82C1
#define GL_IMAGE_CLASS_11_11_10           0x82C2
#define GL_IMAGE_CLASS_10_10_10_2         0x82C3
#define GL_VIEW_CLASS_128_BITS            0x82C4
#define GL_VIEW_CLASS_96_BITS             0x82C5
#define GL_VIEW_CLASS_64_BITS             0x82C6
#define GL_VIEW_CLASS_48_BITS             0x82C7
#define GL_VIEW_CLASS_32_BITS             0x82C8
#define GL_VIEW_CLASS_24_BITS             0x82C9
#define GL_VIEW_CLASS_16_BITS             0x82CA
#define GL_VIEW_CLASS_8_BITS              0x82CB
#define GL_VIEW_CLASS_S3TC_DXT1_RGB       0x82CC
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA      0x82CD
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA      0x82CE
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA      0x82CF
#define GL_VIEW_CLASS_RGTC1_RED           0x82D0
#define GL_VIEW_CLASS_RGTC2_RG            0x82D1
#define GL_VIEW_CLASS_BPTC_UNORM          0x82D2
#define GL_VIEW_CLASS_BPTC_FLOAT          0x82D3
#endif

#ifndef GL_ARB_invalidate_subdata
#endif

#ifndef GL_ARB_multi_draw_indirect
#endif

#ifndef GL_ARB_program_interface_query
#define GL_UNIFORM                        0x92E1
#define GL_UNIFORM_BLOCK                  0x92E2
#define GL_PROGRAM_INPUT                  0x92E3
#define GL_PROGRAM_OUTPUT                 0x92E4
#define GL_BUFFER_VARIABLE                0x92E5
#define GL_SHADER_STORAGE_BLOCK           0x92E6
    /* reuse GL_ATOMIC_COUNTER_BUFFER */
#define GL_VERTEX_SUBROUTINE              0x92E8
#define GL_TESS_CONTROL_SUBROUTINE        0x92E9
#define GL_TESS_EVALUATION_SUBROUTINE     0x92EA
#define GL_GEOMETRY_SUBROUTINE            0x92EB
#define GL_FRAGMENT_SUBROUTINE            0x92EC
#define GL_COMPUTE_SUBROUTINE             0x92ED
#define GL_VERTEX_SUBROUTINE_UNIFORM      0x92EE
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_GEOMETRY_SUBROUTINE_UNIFORM    0x92F1
#define GL_FRAGMENT_SUBROUTINE_UNIFORM    0x92F2
#define GL_COMPUTE_SUBROUTINE_UNIFORM     0x92F3
#define GL_TRANSFORM_FEEDBACK_VARYING     0x92F4
#define GL_ACTIVE_RESOURCES               0x92F5
#define GL_MAX_NAME_LENGTH                0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES       0x92F7
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB
#define GL_OFFSET                         0x92FC
#define GL_BLOCK_INDEX                    0x92FD
#define GL_ARRAY_STRIDE                   0x92FE
#define GL_MATRIX_STRIDE                  0x92FF
#define GL_IS_ROW_MAJOR                   0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX    0x9301
#define GL_BUFFER_BINDING                 0x9302
#define GL_BUFFER_DATA_SIZE               0x9303
#define GL_NUM_ACTIVE_VARIABLES           0x9304
#define GL_ACTIVE_VARIABLES               0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER    0x9306
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_REFERENCED_BY_GEOMETRY_SHADER  0x9309
#define GL_REFERENCED_BY_FRAGMENT_SHADER  0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER   0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE           0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE         0x930D
#define GL_LOCATION                       0x930E
#define GL_LOCATION_INDEX                 0x930F
#define GL_IS_PER_PATCH                   0x92E7
    /* reuse GL_NUM_COMPATIBLE_SUBROUTINES */
    /* reuse GL_COMPATIBLE_SUBROUTINES */
#endif

#ifndef GL_ARB_robust_buffer_access_behavior
#endif

#ifndef GL_ARB_shader_image_size
#endif

#ifndef GL_ARB_shader_storage_buffer_object
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BUFFER_START    0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE     0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE  0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT     0x2000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS
    /* reuse GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS */
#endif

#ifndef GL_ARB_stencil_texturing
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#endif

#ifndef GL_ARB_texture_buffer_range
#define GL_TEXTURE_BUFFER_OFFSET          0x919D
#define GL_TEXTURE_BUFFER_SIZE            0x919E
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#endif

#ifndef GL_ARB_texture_query_levels
#endif

#ifndef GL_ARB_texture_storage_multisample
#endif


    /*************************************************************/
#ifndef IMAPP_IMPL
#define GLEXTERN extern
#else
#define GLEXTERN
#endif
#ifdef _WIN32
GLEXTERN void(APIENTRY* glActiveTexture) (GLenum texture);
#endif
GLEXTERN void(APIENTRY* glUniform1i) (GLint location, GLint v0);
GLEXTERN void(APIENTRY* glUniformMatrix3fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GLEXTERN void(APIENTRY* glBindFramebuffer) (GLenum target, GLuint framebuffer);
GLEXTERN void(APIENTRY* glDeleteFramebuffers) (GLsizei n, const GLuint* framebuffers);
GLEXTERN void(APIENTRY* glDeleteRenderbuffers) (GLsizei n, const GLuint* renderbuffers);
GLEXTERN void(APIENTRY* glFramebufferTexture2D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLEXTERN void(APIENTRY* glFramebufferRenderbuffer) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLEXTERN void(APIENTRY* glRenderbufferStorage) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLEXTERN void(APIENTRY* glGenFramebuffers) (GLsizei n, GLuint* framebuffers);
GLEXTERN void(APIENTRY* glGenRenderbuffers) (GLsizei n, GLuint* renderbuffers);
GLEXTERN void(APIENTRY* glBindRenderbuffer) (GLenum target, GLuint renderbuffer);
GLEXTERN GLenum(APIENTRY* glCheckFramebufferStatus) (GLenum target);
GLEXTERN void(APIENTRY* glGenerateMipmap) (GLenum target);
GLEXTERN void(APIENTRY* glBufferData) (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
GLEXTERN void(APIENTRY* glUseProgram) (GLuint program);
GLEXTERN GLint(APIENTRY* glGetUniformLocation) (GLuint program, const GLchar* name);
GLEXTERN GLint(APIENTRY* glGetAttribLocation) (GLuint program, const GLchar* name);
GLEXTERN void(APIENTRY* glDeleteBuffers) (GLsizei n, const GLuint* buffers);
GLEXTERN void(APIENTRY* glDeleteVertexArrays) (GLsizei n, const GLuint* arrays);
GLEXTERN void(APIENTRY* glEnableVertexAttribArray) (GLuint);
GLEXTERN void(APIENTRY* glVertexAttribPointer) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
GLEXTERN void(APIENTRY* glGenBuffers) (GLsizei n, GLuint* buffers);
GLEXTERN void(APIENTRY* glBindBuffer) (GLenum target, GLuint buffer);
GLEXTERN GLuint(APIENTRY* glCreateShader) (GLenum type);
GLEXTERN void(APIENTRY* glShaderSource) (GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
GLEXTERN void(APIENTRY* glCompileShader) (GLuint shader);
GLEXTERN GLuint(APIENTRY* glCreateProgram) (void);
GLEXTERN void(APIENTRY* glAttachShader) (GLuint program, GLuint shader);
GLEXTERN void(APIENTRY* glDeleteProgram) (GLuint program);
GLEXTERN void(APIENTRY* glDeleteShader) (GLuint shader);
GLEXTERN void(APIENTRY* glDisableVertexAttribArray) (GLuint);
GLEXTERN void(APIENTRY* glBindAttribLocation) (GLuint program, GLuint index, const GLchar* name);
GLEXTERN void(APIENTRY* glVertexAttribDivisor) (GLuint index, GLuint divisor);
GLEXTERN void(APIENTRY* glUniformMatrix4fv) (GLint location, GLsizei count, GLboolean transpose, const float* value);
GLEXTERN void(APIENTRY* glGetShaderiv) (GLuint shader, GLenum pname, GLint* param);
GLEXTERN void(APIENTRY* glLinkProgram) (GLuint program);
GLEXTERN void(APIENTRY* glGetProgramiv) (GLuint program, GLenum pname, GLint* param);
GLEXTERN void(APIENTRY* glBindVertexArray) (GLuint array);
GLEXTERN void(APIENTRY* glUniform2fv) (GLint location, GLsizei count, const float* value);
GLEXTERN void(APIENTRY* glUniform3f) (GLint location, float v0, float v1, float v2);
GLEXTERN void(APIENTRY* glUniform3fv) (GLint location, GLsizei count, const float* value);
GLEXTERN void(APIENTRY* glUniform4fv) (GLint location, GLsizei count, const float* value);
GLEXTERN void(APIENTRY* glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
GLEXTERN void(APIENTRY* glGenVertexArrays) (GLsizei n, const GLuint* arrays);
GLEXTERN void(APIENTRY* glGetShaderInfoLog) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
GLEXTERN void(APIENTRY* glGetProgramInfoLog) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
GLEXTERN GLuint(APIENTRY* glGetUniformBlockIndex) (GLuint program, const GLchar* uniformBlockName);
GLEXTERN void(APIENTRY* glUniformBlockBinding) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
GLEXTERN void(APIENTRY* glBindBufferBase) (GLenum target, GLuint index, GLuint buffer);
GLEXTERN void(APIENTRY* glTransformFeedbackVaryings) (GLuint, GLsizei, const GLchar**, GLenum);
GLEXTERN GLvoid* (APIENTRY* glMapBuffer) (GLenum target, GLenum access);
GLEXTERN GLboolean(APIENTRY* glUnmapBuffer) (GLenum target);
GLEXTERN void(APIENTRY* glDrawElementsInstanced) (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
GLEXTERN void(APIENTRY* glDrawArraysInstanced) (GLenum, GLint, GLsizei, GLsizei);
GLEXTERN void(APIENTRY* glDrawElementsInstancedBaseVertex) (GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount, GLint basevertex);
GLEXTERN void(APIENTRY* glBeginTransformFeedback) (GLenum);
GLEXTERN void(APIENTRY* glEndTransformFeedback) (void);
GLEXTERN void(APIENTRY* glUniform1f) (GLint location, float v0);
GLEXTERN void(APIENTRY* glUniform2f) (GLint location, float v0, float v1);
GLEXTERN void(APIENTRY* glBlendEquationSeparate) (GLenum, GLenum);
GLEXTERN void(APIENTRY* glBlendFuncSeparate) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
GLEXTERN void(APIENTRY* glGetBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data);
GLEXTERN void(APIENTRY* glGetShaderSource) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
GLEXTERN GLboolean(APIENTRY* glIsProgram)(GLuint program);
GLEXTERN void(APIENTRY* glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
GLEXTERN void(APIENTRY* glDrawBuffers)(GLsizei n, const GLenum* bufs);
GLEXTERN void(APIENTRY* glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
GLEXTERN void(APIENTRY* glBlendEquation)(GLenum mode);
GLEXTERN void(APIENTRY* glBindSampler)(GLuint unit, GLuint sampler);
GLEXTERN void(APIENTRY* glDetachShader)(GLuint program, GLuint shader);



namespace ImApp
{
#ifdef FMOD_API

#pragma comment(lib,"fmodstudio_vc.lib")
#pragma comment(lib,"fmod_vc.lib")

   void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)
#endif

   struct Config
   {
      Config() : mWidth(1280), mHeight(720), mFullscreen(false)
      {

      }
      int mWidth;
      int mHeight;
      bool mFullscreen;
   };

   struct ImApp
   {
      ImApp() : mInitExtensionsDone(false), mExtensionsPresent(false), mDone(true)
      {
         mInstance = this;
      }
      ~ImApp()
      {
         mInstance = NULL;
      }
      static ImApp* Instance() { return mInstance; }
      int Init(const Config& config = Config())
      {
         mConfig = config;
         wininfo = WININFO{ 0,0,0,0,
         { 'c','X','d',0 }
         };
         WININFO* info = &wininfo;

         info->hInstance = GetModuleHandle(0);

         static DEVMODEA screenSettings = { { 0 },
#if defined(_MSC_VER) && _MSC_VER < 1400
                0,0,148,0,0x001c0000,{ 0 },0,0,0,0,0,0,0,0,0,{ 0 },0,32,config.mWidth,config.mHeight,0,0,      // Visual C++ 6.0
#else
                0,0,156,0,0x001c0000,{ 0 },0,0,0,0,0,{ 0 },0,32,static_cast<DWORD>(config.mWidth), static_cast<DWORD>(config.mHeight),{ 0 }, 0,           // Visual Studio 2005
#endif
#if(WINVER >= 0x0400)
                0,0,0,0,0,0,
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
                0,0
#endif
#endif
         };
         if (config.mFullscreen)
         {
            if (config.mFullscreen && ChangeDisplaySettingsA(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
               return 0;
         }
#ifdef IMGUI_API
         ImGui::CreateContext();
#endif
         if (!WindowInit(info))
         {
            WindowEnd(info);
            return 0;
         }
         if (!InitExtension())
            return 0;

#ifdef IMGUI_API
         if (!ImGui_Init())
            return 0;

#endif
#ifdef FMOD_API
         void* extraDriverData = NULL;
         ERRCHECK(FMOD::Studio::System::create(&system));
         FMOD::System* lowLevelSystem = NULL;
         ERRCHECK(system->getLowLevelSystem(&lowLevelSystem));
         ERRCHECK(lowLevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0)); // on rajoute la config pour le stereo

#ifdef RETAIL
         ERRCHECK(system->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData));
#else
         ERRCHECK(system->initialize(32, FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_LIVEUPDATE | FMOD_STUDIO_INIT_SYNCHRONOUS_UPDATE, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, extraDriverData));
#endif
#endif
         mDone = false;
         return 1;
      }

      void LoadBanks(int bankCount, const char** bankPaths);
      void PlayEvent(const char* eventName);

      void NewFrame()
      {
         MSG msg;
         while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
               mDone = true;
         }

#ifdef IMGUI_API
         ImGui_NewFrame();
#endif
      }

      void EndFrame()
      {
#ifdef FMOD_API
         ERRCHECK(system->update());
#endif
#ifdef IMGUI_API
         ImGui::Render();
#endif
         ImGui_RenderDrawLists(ImGui::GetDrawData());
         SwapBuffers(wininfo.hDC);
      }
      void Finish()
      {
#ifdef IMGUI_API
         ImGui_Shutdown();
#endif
#ifdef FMOD_API
         ERRCHECK(system->unloadAll());
         ERRCHECK(system->flushCommands());
         ERRCHECK(system->release());
#endif
      }
      bool Done()
      {
         return mDone;
      }
   protected:
      static ImApp* mInstance;
      Config mConfig;

      bool mInitExtensionsDone;
      bool mExtensionsPresent;
      bool mDone;
#ifdef FMOD_API
      FMOD::Studio::System* system;
#endif
      typedef struct
      {
         //---------------
         HINSTANCE   hInstance;
         HDC         hDC;
         HGLRC       hRC;
         HWND        hWnd;
         char        wndclass[4];    // window class and title :)
                                     //---------------
      }WININFO;

      WININFO wininfo;

      static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
      {
#ifdef IMGUI_API
         if (ImGui_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;
#endif
         switch (msg)
         {
         case WM_SIZE:
         {
#ifdef IMGUI_API
            ImGuiIO& io = ImGui::GetIO();

            int w, h;
            int display_w, display_h;
            //glfwGetWindowSize(g_Window, &w, &h);
            //glfwGetFramebufferSize(g_Window, &display_w, &display_h);
            w = LOWORD(lParam); // width of client area
            h = HIWORD(lParam); // height of client area
            io.DisplaySize = ImVec2((float)w, (float)h);
            display_w = w;
            display_h = h;
            io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
#endif
         }
         return 0;
         case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
               return 0;
            break;
         case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
         }
         return DefWindowProc(hWnd, msg, wParam, lParam);
      }


      void WindowEnd(WININFO* info)
      {
         if (info->hRC)
         {
            wglMakeCurrent(0, 0);
            wglDeleteContext(info->hRC);
         }

         if (info->hDC) ReleaseDC(info->hWnd, info->hDC);
         if (info->hWnd) DestroyWindow(info->hWnd);

         UnregisterClassA(info->wndclass, info->hInstance);

         if (mConfig.mFullscreen)
         {
            ChangeDisplaySettings(0, 0);
            while (ShowCursor(1) < 0); // show cursor
         }
      }

      int WindowInit(WININFO* info)
      {
         unsigned int    PixelFormat;

         DWORD            dwExStyle, dwStyle;
         DEVMODE            dmScreenSettings;
         RECT            rec;

         WNDCLASSEXA WndClsEx;

         // Create the application window
         WndClsEx.cbSize = sizeof(WNDCLASSEX);
         WndClsEx.style = CS_HREDRAW | CS_VREDRAW;
         WndClsEx.lpfnWndProc = WndProc;
         WndClsEx.cbClsExtra = 0;
         WndClsEx.cbWndExtra = 0;
         WndClsEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
         WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
         WndClsEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
         WndClsEx.lpszMenuName = NULL;
         WndClsEx.lpszClassName = "ImGuizmoWndClass";
         WndClsEx.hInstance = GetModuleHandle(NULL);
         WndClsEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

         if (!RegisterClassExA(&WndClsEx))
            return 0;

         if (mConfig.mFullscreen)
         {
            dmScreenSettings.dmSize = sizeof(DEVMODE);
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            dmScreenSettings.dmBitsPerPel = 32;
            dmScreenSettings.dmPelsWidth = mConfig.mWidth;
            dmScreenSettings.dmPelsHeight = mConfig.mHeight;

            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
               return(0);

            dwExStyle = WS_EX_APPWINDOW;
            dwStyle = WS_VISIBLE | WS_POPUP;

            //while (ShowCursor(0) >= 0);    // hide cursor
         }
         else
         {
            dwExStyle = 0;
            //dwStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_OVERLAPPED;
            dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_POPUP;
         }

         rec.left = 0;
         rec.top = 0;
         rec.right = mConfig.mWidth;
         rec.bottom = mConfig.mHeight;
         AdjustWindowRect(&rec, dwStyle, 0);

         info->hWnd = CreateWindowExA(dwExStyle, "ImGuizmoWndClass", " ", dwStyle | WS_MAXIMIZE,
            (GetSystemMetrics(SM_CXSCREEN) - rec.right + rec.left) >> 1,
            (GetSystemMetrics(SM_CYSCREEN) - rec.bottom + rec.top) >> 1,
            rec.right - rec.left, rec.bottom - rec.top,
            0, 0, info->hInstance, 0);

         if (!info->hWnd)
            return(0);

         if (!(info->hDC = GetDC(info->hWnd)))
            return(0);

         static PIXELFORMATDESCRIPTOR pfd =
         {
             sizeof(PIXELFORMATDESCRIPTOR),
             1,
             PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
             PFD_TYPE_RGBA,
             32,
             0, 0, 0, 0, 0, 0, 8, 0,
             0, 0, 0, 0, 0,  // accum
             32,             // zbuffer
             0,              // stencil!
             0,              // aux
             PFD_MAIN_PLANE,
             0, 0, 0, 0
         };

         if (!(PixelFormat = ChoosePixelFormat(info->hDC, &pfd)))
            return(0);

         if (!SetPixelFormat(info->hDC, PixelFormat, &pfd))
            return(0);

         if (!(info->hRC = wglCreateContext(info->hDC)))
            return(0);

         if (!wglMakeCurrent(info->hDC, info->hRC))
            return(0);

         return(1);
      }


      bool loadExtension(const char* extensionName, void** functionPtr)
      {
#if defined(_WIN32)
         *functionPtr = glGetProcAddress(extensionName);
#else
         * functionPtr = (void*)glGetProcAddress((const GLubyte*)extensionName);
#endif
         return (*functionPtr != NULL);
      }
#define LE(x) mExtensionsPresent &= loadExtension(#x, (void**)&x);

      bool InitExtension()
      {
         if (mInitExtensionsDone)
            return true;

         mExtensionsPresent = true;

         LE(glUniform1i); //GLint location, GLint v0);
         LE(glUniformMatrix3fv) // GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
#ifdef _WIN32
            LE(glActiveTexture); //GLenum texture);
#endif
         LE(glBindFramebuffer); //GLenum target, TextureID framebuffer);
         LE(glDeleteFramebuffers); //GLsizei n, const TextureID* framebuffers);
         LE(glDeleteRenderbuffers); //GLsizei n, const TextureID* renderbuffers);
         LE(glFramebufferTexture2D); //GLenum target, GLenum attachment, GLenum textarget, TextureID texture, GLint level);
         LE(glFramebufferRenderbuffer); //GLenum target, GLenum attachment, GLenum renderbuffertarget, TextureID renderbuffer);
         LE(glRenderbufferStorage); //GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
         LE(glGenFramebuffers); //GLsizei n, TextureID* framebuffers);
         LE(glGenRenderbuffers); //GLsizei n, TextureID* renderbuffers);
         LE(glBindRenderbuffer); //GLenum target, TextureID renderbuffer);
         LE(glCheckFramebufferStatus); //GLenum target);
         LE(glGenerateMipmap); //GLenum target);
         LE(glBufferData); //GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
         LE(glUseProgram); //TextureID program);
         LE(glGetUniformLocation); //TextureID program, const GLchar* name);
         LE(glGetAttribLocation); //TextureID program, const GLchar* name);
         LE(glDeleteBuffers); //GLsizei n, const TextureID* buffers);
         LE(glDeleteVertexArrays); //GLsizei n, const TextureID* arrays);
         LE(glEnableVertexAttribArray); //TextureID);
         LE(glVertexAttribPointer); //TextureID index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
         LE(glGenBuffers); //GLsizei n, TextureID* buffers);
         LE(glBindBuffer); //GLenum target, TextureID buffer);
         LE(glCreateShader); //GLenum type);
         LE(glShaderSource); //TextureID shader, GLsizei count, const GLchar** strings, const GLint* lengths);
         LE(glCompileShader); //TextureID shader);
         LE(glCreateProgram); //void);
         LE(glAttachShader); //TextureID program, TextureID shader);
         LE(glDeleteProgram); //TextureID program);
         LE(glDeleteShader); //TextureID shader);
         LE(glDisableVertexAttribArray); //TextureID);
         LE(glBindAttribLocation); //TextureID program, TextureID index, const GLchar* name);
         LE(glVertexAttribDivisor); //TextureID index, TextureID divisor);
         LE(glUniformMatrix4fv); //GLint location, GLsizei count, GLboolean transpose, const float* value);
         LE(glGetShaderiv); //TextureID shader, GLenum pname, GLint* param);
         LE(glLinkProgram); //TextureID program);
         LE(glGetProgramiv); //TextureID program, GLenum pname, GLint* param);
         LE(glBindVertexArray); //TextureID array);
         LE(glUniform2fv);
         LE(glUniform3f); //GLint location, float v0, float v1, float v2);
         LE(glUniform3fv); //GLint location, GLsizei count, const float* value);
         LE(glUniform4fv); //GLint location, GLsizei count, const float* value);
         LE(glBufferSubData); //GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
         LE(glGenVertexArrays); //GLsizei n, const TextureID* arrays);
         LE(glGetShaderInfoLog); //TextureID shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
         LE(glGetProgramInfoLog); //TextureID program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
         LE(glGetUniformBlockIndex); //TextureID program, const GLchar* uniformBlockName);
         LE(glUniformBlockBinding); //TextureID program, TextureID uniformBlockIndex, TextureID uniformBlockBinding);
         LE(glBindBufferBase); //GLenum target, TextureID index, TextureID buffer);
         LE(glTransformFeedbackVaryings); //TextureID, GLsizei, const GLchar **, GLenum);
         LE(glMapBuffer); //GLenum target, GLenum access);
         LE(glUnmapBuffer); //GLenum target);
         LE(glDrawElementsInstanced); //GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
         LE(glDrawArraysInstanced); //GLenum, GLint, GLsizei, GLsizei);
         LE(glDrawElementsInstancedBaseVertex); //GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount, GLint basevertex);
         LE(glBeginTransformFeedback); //GLenum);
         LE(glEndTransformFeedback); //void);
         LE(glUniform1f); //GLint location, float v0);
         LE(glUniform2f); //GLint location, float v0, float v1);
         LE(glBlendEquationSeparate); //GLenum, GLenum);
         LE(glBlendFuncSeparate); //GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
         LE(glGetBufferSubData); //GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data);
         LE(glGetShaderSource);
         LE(glIsProgram);
         LE(glGetAttachedShaders);
         LE(glDrawBuffers);
         LE(glBlitFramebuffer);
         LE(glBlendEquation);
         LE(glBindSampler);
         LE(glDetachShader);

         return mExtensionsPresent;
      }

#ifdef IMGUI_API
      // Data
      static double       g_Time;
      static bool         g_MousePressed[3];
      static float        g_MouseWheel;
      static GLuint       g_FontTexture;
      static int          g_ShaderHandle, g_VertHandle, g_FragHandle;
      static int          g_AttribLocationTex, g_AttribLocationProjMtx;
      static int          g_AttribLocationPosition, g_AttribLocationUV, g_AttribLocationColor;
      static unsigned int g_VboHandle, g_VaoHandle, g_ElementsHandle;

      static bool IsAnyMouseButtonDown()
      {
         ImGuiIO& io = ImGui::GetIO();
         for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
            if (io.MouseDown[n])
               return true;
         return false;
      }

      // We use the Win32 capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinations when dragging mouse outside of our window bounds.
      static IMGUI_API LRESULT ImGui_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
      {
         ImGuiIO& io = ImGui::GetIO();
         switch (msg)
         {
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_MBUTTONDOWN:
         {
            int button = 0;
            if (msg == WM_LBUTTONDOWN) button = 0;
            if (msg == WM_RBUTTONDOWN) button = 1;
            if (msg == WM_MBUTTONDOWN) button = 2;
            if (!IsAnyMouseButtonDown() && GetCapture() == NULL)
               SetCapture(hwnd);
            io.MouseDown[button] = true;
            return 0;
         }
         case WM_LBUTTONUP:
         case WM_RBUTTONUP:
         case WM_MBUTTONUP:
         {
            int button = 0;
            if (msg == WM_LBUTTONUP) button = 0;
            if (msg == WM_RBUTTONUP) button = 1;
            if (msg == WM_MBUTTONUP) button = 2;
            io.MouseDown[button] = false;
            if (!IsAnyMouseButtonDown() && GetCapture() == hwnd)
               ReleaseCapture();
            return 0;
         }
         case WM_MOUSEWHEEL:
            io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
            return 0;
         case WM_MOUSEMOVE:
            io.MousePos.x = (signed short)(lParam);
            io.MousePos.y = (signed short)(lParam >> 16);
            return 0;
         case WM_KEYDOWN:
         case WM_SYSKEYDOWN:
            if (wParam < 256)
               io.KeysDown[wParam] = 1;
            return 0;
         case WM_KEYUP:
         case WM_SYSKEYUP:
            if (wParam < 256)
               io.KeysDown[wParam] = 0;
            return 0;
         case WM_CHAR:
            // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
            if (wParam > 0 && wParam < 0x10000)
               io.AddInputCharacter((unsigned short)wParam);
            return 0;
         }
         return 0;
      }
      // This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
      // Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
      // If text or lines are blurry when integrating ImGui in your engine: in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
      static void ImGui_RenderDrawLists(ImDrawData* draw_data)
      {
         // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
         ImGuiIO& io = ImGui::GetIO();
         int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
         int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
         if (fb_width == 0 || fb_height == 0)
            return;
         draw_data->ScaleClipRects(io.DisplayFramebufferScale);

         // Backup GL state
         GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
         glActiveTexture(GL_TEXTURE0);
         GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
         GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
         GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
         GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
         GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
         GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
         GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
         GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
         GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
         GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
         GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
         GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
         GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
         GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
         GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
         GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
         GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
         GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
         GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

         // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
         glEnable(GL_BLEND);
         glBlendEquation(GL_FUNC_ADD);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         glDisable(GL_CULL_FACE);
         glDisable(GL_DEPTH_TEST);
         glEnable(GL_SCISSOR_TEST);
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

         // Setup viewport, orthographic projection matrix
         glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
         const float ortho_projection[4][4] =
         {
             { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
             { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
             { 0.0f,                  0.0f,                  -1.0f, 0.0f },
             { -1.0f,                  1.0f,                   0.0f, 1.0f },
         };
         glUseProgram(g_ShaderHandle);
         glUniform1i(g_AttribLocationTex, 0);
         glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
         glBindVertexArray(g_VaoHandle);
         glBindSampler(0, 0); // Rely on combined texture/sampler state.

         for (int n = 0; n < draw_data->CmdListsCount; n++)
         {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx* idx_buffer_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
               const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
               if (pcmd->UserCallback)
               {
                  pcmd->UserCallback(cmd_list, pcmd);
               }
               else
               {
                  glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                  glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                  glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
               }
               idx_buffer_offset += pcmd->ElemCount;
            }
         }

         // Restore modified GL state
         glUseProgram(last_program);
         glBindTexture(GL_TEXTURE_2D, last_texture);
         glBindSampler(0, last_sampler);
         glActiveTexture(last_active_texture);
         glBindVertexArray(last_vertex_array);
         glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
         glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
         glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
         if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
         if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
         if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
         if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
         glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
         glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
         glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
      }
#if 0
      static const char* ImGui_GetClipboardText(void* user_data)
      {
         return glfwGetClipboardString((GLFWwindow*)user_data);
      }

      static void ImGui_SetClipboardText(void* user_data, const char* text)
      {
         glfwSetClipboardString((GLFWwindow*)user_data, text);
      }

      void ImGui_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
      {
         if (action == GLFW_PRESS && button >= 0 && button < 3)
            g_MousePressed[button] = true;
      }

      void ImGui_ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
      {
         g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
      }

      void ImGui_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
      {
         ImGuiIO& io = ImGui::GetIO();
         if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
         if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

         (void)mods; // Modifiers are not reliable across systems
         io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
         io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
         io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
         io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
      }

      void ImGui_CharCallback(GLFWwindow*, unsigned int c)
      {
         ImGuiIO& io = ImGui::GetIO();
         if (c > 0 && c < 0x10000)
            io.AddInputCharacter((unsigned short)c);
      }
#endif
      bool ImGui_CreateFontsTexture()
      {
         // Build texture atlas
         ImGuiIO& io = ImGui::GetIO();
         unsigned char* pixels;
         int width, height;
         io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

                                                                   // Upload texture to graphics system
         GLint last_texture;
         glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
         glGenTextures(1, &g_FontTexture);
         glBindTexture(GL_TEXTURE_2D, g_FontTexture);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

         // Store our identifier
         io.Fonts->TexID = (void*)(intptr_t)g_FontTexture;

         // Restore state
         glBindTexture(GL_TEXTURE_2D, last_texture);

         return true;
      }

      bool ImGui_CreateDeviceObjects()
      {
         // Backup GL state
         GLint last_texture, last_array_buffer, last_vertex_array;
         glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
         glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
         glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

         const GLchar* vertex_shader =
            "#version 330\n"
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

         const GLchar* fragment_shader =
            "#version 330\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
            "}\n";

         g_ShaderHandle = glCreateProgram();
         g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
         g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
         glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
         glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
         glCompileShader(g_VertHandle);
         glCompileShader(g_FragHandle);
         glAttachShader(g_ShaderHandle, g_VertHandle);
         glAttachShader(g_ShaderHandle, g_FragHandle);
         glLinkProgram(g_ShaderHandle);

         g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
         g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
         g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
         g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
         g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

         glGenBuffers(1, &g_VboHandle);
         glGenBuffers(1, &g_ElementsHandle);

         glGenVertexArrays(1, &g_VaoHandle);
         glBindVertexArray(g_VaoHandle);
         glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
         glEnableVertexAttribArray(g_AttribLocationPosition);
         glEnableVertexAttribArray(g_AttribLocationUV);
         glEnableVertexAttribArray(g_AttribLocationColor);
#ifndef OFFSETOF
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
#endif
         glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
         glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
         glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

         ImGui_CreateFontsTexture();

         // Restore modified GL state
         glBindTexture(GL_TEXTURE_2D, last_texture);
         glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
         glBindVertexArray(last_vertex_array);

         return true;
      }

      void    ImGui_InvalidateDeviceObjects()
      {
         if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
         if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
         if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
         g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

         if (g_ShaderHandle && g_VertHandle) glDetachShader(g_ShaderHandle, g_VertHandle);
         if (g_VertHandle) glDeleteShader(g_VertHandle);
         g_VertHandle = 0;

         if (g_ShaderHandle && g_FragHandle) glDetachShader(g_ShaderHandle, g_FragHandle);
         if (g_FragHandle) glDeleteShader(g_FragHandle);
         g_FragHandle = 0;

         if (g_ShaderHandle) glDeleteProgram(g_ShaderHandle);
         g_ShaderHandle = 0;

         if (g_FontTexture)
         {
            glDeleteTextures(1, &g_FontTexture);
            ImGui::GetIO().Fonts->TexID = 0;
            g_FontTexture = 0;
         }
      }

      bool    ImGui_Init()
      {
         ImGuiIO& io = ImGui::GetIO();
         io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
         io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
         io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
         io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
         io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
         io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
         io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
         io.KeyMap[ImGuiKey_Home] = VK_HOME;
         io.KeyMap[ImGuiKey_End] = VK_END;
         io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
         io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
         io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
         io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
         io.KeyMap[ImGuiKey_A] = 'A';
         io.KeyMap[ImGuiKey_C] = 'C';
         io.KeyMap[ImGuiKey_V] = 'V';
         io.KeyMap[ImGuiKey_X] = 'X';
         io.KeyMap[ImGuiKey_Y] = 'Y';
         io.KeyMap[ImGuiKey_Z] = 'Z';

        // Edit: not in imgui
         //io.ImeWindowHandle = this->wininfo.hWnd;
         //io.RenderDrawListsFn = ImGui_RenderDrawLists;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
         /*
         io.SetClipboardTextFn = ImGui_SetClipboardText;
         io.GetClipboardTextFn = ImGui_GetClipboardText;
         //io.ClipboardUserData = g_Window;
         */
         return true;
      }

      void ImGui_Shutdown()
      {
         ImGui_InvalidateDeviceObjects();
      }

      void ImGui_NewFrame()
      {
         if (!g_FontTexture)
            ImGui_CreateDeviceObjects();

         ImGuiIO& io = ImGui::GetIO();

         // Read keyboard modifiers inputs
         io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
         io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
         io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
         io.KeySuper = false;

         /*
         // Setup display size (every frame to accommodate for window resizing)
         int w, h;
         int display_w, display_h;
         glfwGetWindowSize(g_Window, &w, &h);
         glfwGetFramebufferSize(g_Window, &display_w, &display_h);
         io.DisplaySize = ImVec2((float)w, (float)h);
         io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

         // Setup time step
         double current_time = glfwGetTime();
         io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
         g_Time = current_time;

         // Setup inputs
         // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
         if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
         {
             if (io.WantMoveMouse)
             {
                 glfwSetCursorPos(g_Window, (double)io.MousePos.x, (double)io.MousePos.y);   // Set mouse position if requested by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
             }
             else
             {
                 double mouse_x, mouse_y;
                 glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
                 io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Get mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
             }
         }
         else
         {
             io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
         }

         for (int i = 0; i < 3; i++)
         {
             io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
             g_MousePressed[i] = false;
         }

         io.MouseWheel = g_MouseWheel;
         g_MouseWheel = 0.0f;

         // Hide OS mouse cursor if ImGui is drawing it
         glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
         */
         // Start the frame
         ImGui::NewFrame();
      }




#endif // IMGUI_API

   }; // IMAPP

#ifdef IMAPP_IMPL
   ImApp* ImApp::mInstance = NULL;
#ifdef IMGUI_API

   double       ImApp::g_Time = 0.0f;
   bool         ImApp::g_MousePressed[3] = { false, false, false };
   float        ImApp::g_MouseWheel = 0.0f;
   GLuint       ImApp::g_FontTexture = 0;
   int          ImApp::g_ShaderHandle = 0, ImApp::g_VertHandle = 0, ImApp::g_FragHandle = 0;
   int          ImApp::g_AttribLocationTex = 0, ImApp::g_AttribLocationProjMtx = 0;
   int          ImApp::g_AttribLocationPosition = 0, ImApp::g_AttribLocationUV = 0, ImApp::g_AttribLocationColor = 0;
   unsigned int ImApp::g_VboHandle = 0, ImApp::g_VaoHandle = 0, ImApp::g_ElementsHandle = 0;
#endif

#ifdef FMOD_API
   void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
   {
      if (result != FMOD_OK)
      {
         /*if (Common_Private_Error)
         {
             Common_Private_Error(result, file, line);
         }
         Common_Fatal("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
         */
         MessageBoxA(NULL, "", "FMOD Error", MB_OK);
      }
   }

   void ImApp::LoadBanks(int bankCount, const char** bankPaths)
   {
      for (int i = 0; i < bankCount; i++)
      {
         FMOD::Studio::Bank* bank;
         ERRCHECK(system->loadBankFile(bankPaths[i], FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &bank));

         //loadBank(system, (LoadBankMethod)i, Common_MediaPath(BANK_NAMES[i]), &banks[i]));
      }
   }
   void ImApp::PlayEvent(const char* eventName)
   {
      FMOD::Studio::ID FMODEventID = { 0 };
      //ERRCHECK( FMODsystem->lookupID(facts[j].mName.c_str(), &FMODEventID) );
      FMOD::Studio::EventDescription* eventDescription = NULL;
      ERRCHECK(system->getEvent(eventName, /*FMOD_STUDIO_LOAD_BEGIN_NOW,*/ &eventDescription));
      FMOD::Studio::EventInstance* FMODeventInstance = NULL;
      ERRCHECK(eventDescription->createInstance(&FMODeventInstance));
      ERRCHECK(FMODeventInstance->start());
   }
#else
   void ImApp::LoadBanks(int bankCount, const char** bankPaths) {}
   void ImApp::PlayEvent(const char* eventName) {}
#endif

#endif
}
