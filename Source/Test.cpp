
#include "Test.h"
#include "MeshData.h"
#include "SoftwareRasterizer\Utility.h"
#include <cstdint>

namespace Test
{
    const uint16_t* kDefaultIndices = []
        {
            static uint16_t indices[0xffff + 1];// 0,1,2,3,..,65535
            for (uint32_t i = 0; i <= 0xffff; i++)
            {
                indices[i] = (uint16_t)i;
            }
            return indices;
        } ();


    struct UniformBlock
    {
        Matrix4x4 projectionMatrix;
        Matrix4x4 viewMatrix;
        Matrix4x4 modelMatrix;
        const Sampler2D* meshTexture;
    };

    void LineVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->attributes[0];
        const Vector4& color = input->attributes[1];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyings[0] = color;
        output->varyingNum = 1;
    }

    void LinePixelShaderMain(const FragmentShaderInput* input, FragmentShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector4& color = input->varyings[0];

        output->fragColor = color;
    }

    void MeshVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->attributes[0];
        const Vector4& uv = input->attributes[1];
        const Vector4& normal = input->attributes[2];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyings[0] = uv;
        output->varyings[1] = modelMatrix * normal;
        output->varyingNum = 2;
    }

    void MeshPixelShaderMain(const FragmentShaderInput* input, FragmentShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector2 uv = input->varyings[0].getXY();
        const Vector4& normal = input->varyings[1];

        output->fragColor = SamplerUtility::SampleTexture2d(uniformBlock->meshTexture, uv);
    }

    void ModelViewer::RenderScene(RenderingContext* _renderingContext, const TestCamera* camera)
    {
        UniformBlock uniformBlock = {};
        uniformBlock.modelMatrix = Matrix4x4::kIdentity;
        _renderingContext->setUniformBlock(&uniformBlock);

        _renderingContext->clearRenderTarget();

        // 射影行列とビュー行列とを作成
        {
            int screenWidth = _renderingContext->getViewportWidth();
            int screenHeight = _renderingContext->getViewportHeight();
            float aspect = (float)screenWidth / (float)screenHeight;
            uniformBlock.projectionMatrix = MatrixUtility::CreatePerspective(camera->fovY, aspect, camera->nearZ, camera->farZ);

            Matrix4x4 rotationX = MatrixUtility::CreateRotationX(camera->angleX);
            Matrix4x4 rotationY = MatrixUtility::CreateRotationY(camera->angleY);
            Vector4 offset(0.0f, 0.0f, camera->zoom, 1.0f);
            offset = (rotationY * rotationX) * offset;
            Vector3 eye = Vector3(camera->focusPositionX, camera->focusPositionY, camera->focusPositionZ) + Vector3(offset.x, offset.y, offset.z);
            Vector3 center(camera->focusPositionX, camera->focusPositionY, camera->focusPositionZ);
            Vector3 up(0.0f, 1.0f, 0.0f);
            uniformBlock.viewMatrix = MatrixUtility::CreateLookAt(eye, center, up);
        }

        // グリッドを描画
        {
            const int gridSize = 9;
            float harfGridSize = gridSize / 2.0f;
            Vector3 gridPositions[2 * 2 * gridSize];
            Vector4 gridColors[2 * 2 * gridSize];
            for (int i = 0; i < gridSize; i++)
            {
                float a = 1.0f * (i - (gridSize / 2));
                gridPositions[2 * i] = { -harfGridSize, 0.0f,  a };
                gridPositions[2 * i + 1] = { harfGridSize, 0.0f, a };
                gridPositions[2 * gridSize + 2 * i] = { a, 0.0f, -harfGridSize };
                gridPositions[2 * gridSize + 2 * i + 1] = { a, 0.0f, harfGridSize };
            }
            for (int i = 0; i < (2 * 2 * gridSize); i++)
            {
                gridColors[i] = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
            }

            _renderingContext->enableVertexAttribute(0);
            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), gridPositions);
            _renderingContext->enableVertexAttribute(1);
            _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), gridColors);
            _renderingContext->setIndexBuffer(kDefaultIndices, 2 * 2 * gridSize);
            _renderingContext->setVertexShaderProgram(LineVertexShaderMain);
            _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

            _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            _renderingContext->disableVertexAttribute(0);
            _renderingContext->disableVertexAttribute(1);
        }

        // 座標軸を描画
        {
            const Vector3 xAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } };
            const Vector4 xAxisColors[2] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } };
            const Vector3 yAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } };
            const Vector4 yAxisColors[2] = { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
            const Vector3 zAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } };
            const Vector4 zAxisColors[2] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

            _renderingContext->enableVertexAttribute(0);
            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), xAxisPositions);
            _renderingContext->enableVertexAttribute(1);
            _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), xAxisColors);
            _renderingContext->setIndexBuffer(kDefaultIndices, 2);
            _renderingContext->setVertexShaderProgram(LineVertexShaderMain);
            _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

            _renderingContext->setDepthFunc(ComparisonType::kLessEqual);
            _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), yAxisPositions);
            _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), yAxisColors);
            _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), zAxisPositions);
            _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), zAxisColors);
            _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            _renderingContext->disableVertexAttribute(0);
            _renderingContext->disableVertexAttribute(1);
            _renderingContext->setDepthFunc(ComparisonType::kDefault);

        }

        // 色付き三角形を描画（頂点色は赤緑青の順、反時計周り）
        {
            const Vector3 polygonPositions[3] = { { -1.0f, 0.0f, 0.0f }, { -1.0f, 2.0f,  0.0f }, { -3.0f,  0.0f,  0.0f } };
            const Vector4 polygonColors[3] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

            _renderingContext->setIndexBuffer(kDefaultIndices, 3);
            _renderingContext->enableVertexAttribute(0);
            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), polygonPositions);
            _renderingContext->enableVertexAttribute(1);
            _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), polygonColors);
            _renderingContext->setVertexShaderProgram(LineVertexShaderMain);// 流用
            _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);// 流用

            _renderingContext->setFrontFaceType(FrontFaceType::kCounterClockwise);
            _renderingContext->setCullFaceType(CullFaceType::kBack);

            _renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

            _renderingContext->disableVertexAttribute(0);
            _renderingContext->disableVertexAttribute(1);
            _renderingContext->setFrontFaceType(FrontFaceType::kDefault);
            _renderingContext->setCullFaceType(CullFaceType::kDefault);
        }

        // モデル（１メッシュ）を描画
        {
            uniformBlock.modelMatrix = MatrixUtility::CreateRotationX(90.0f * 3.14f / 180.0f);

            Texture2D texture = {};
            texture.addr = kTexture;
            texture.width = 256;
            texture.height = 256;
            texture.widthBytes = 4 * 256;

            Sampler2D sampler = {};
            sampler.texture = &texture;
            sampler.minFilter = FilterType::kBilinear;
            sampler.magFilter = FilterType::kBilinear;

            uniformBlock.meshTexture = &sampler;

            _renderingContext->setIndexBuffer(kMeshTriangles, kMeshTrianglesLength);
            _renderingContext->enableVertexAttribute(0);
            _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(float) * 3, kMeshVertices);
            _renderingContext->enableVertexAttribute(1);
            _renderingContext->setVertexAttribute(1, SemanticsType::kTexCoord, 2, ComponentType::kFloat, sizeof(float) * 2, kMeshUvs);
            _renderingContext->enableVertexAttribute(2);
            _renderingContext->setVertexAttribute(2, SemanticsType::kNormal, 3, ComponentType::kFloat, sizeof(float) * 3, kMeshNormals);
            _renderingContext->setVertexShaderProgram(MeshVertexShaderMain);
            _renderingContext->setFragmentShaderProgram(MeshPixelShaderMain);

            _renderingContext->setFrontFaceType(FrontFaceType::kCounterClockwise);
            _renderingContext->setCullFaceType(CullFaceType::kBack);

            _renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

            _renderingContext->disableVertexAttribute(0);
            _renderingContext->disableVertexAttribute(1);
            _renderingContext->disableVertexAttribute(2);
            _renderingContext->setFrontFaceType(FrontFaceType::kDefault);
            _renderingContext->setCullFaceType(CullFaceType::kDefault);

            uniformBlock.modelMatrix = Matrix4x4::kIdentity;
            uniformBlock.meshTexture = nullptr;
        }

    }
}
