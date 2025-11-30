
#include "ModelViewer.h"
#include "MeshData.h"
#include "SoftwareRasterizer\Utility.h"
#include <cstdint>
#include <algorithm>// clamp

namespace Test
{
    void ModelViewer::onLButtonDrag(int xDelta, int yDelta)
    {
        _camera.angleX += yDelta * 0.01f;
        _camera.angleY += xDelta * 0.01f;
    }

    void ModelViewer::onMouseWweel(int zDelta)
    {
        _camera.zoom += zDelta * -0.001f;
        _camera.zoom = std::clamp(_camera.zoom, 0.001f, FLT_MAX);
    }

    void ModelViewer::onKeyDown(int vk)
    {
        switch (vk)
        {
        case 'W':
            _camera.focusPositionZ += 0.1f;
            break;
        case 'S':
            _camera.focusPositionZ -= 0.1f;
            break;
        case 'A':
            _camera.focusPositionX -= 0.1f;
            break;
        case 'D':
            _camera.focusPositionX += 0.1f;
            break;
        case 'Q':
            _camera.focusPositionY += 0.1f;
            break;
        case 'E':
            _camera.focusPositionY -= 0.1f;
            break;
        }
    }

    void ModelViewer::onPaint(RenderingContext* renderingContext)
    {
        renderScene(renderingContext);
    }

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

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * Vector4(position.getXYZ(), 1.0f));
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

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * Vector4(position.getXYZ(), 1.0f));
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

    void ModelViewer::renderScene(RenderingContext* renderingContext)
    {
        UniformBlock uniformBlock = {};
        uniformBlock.modelMatrix = Matrix4x4::kIdentity;

        renderingContext->setUniformBlock(&uniformBlock);

        renderingContext->clearRenderTarget();

        // 射影行列とビュー行列とを作成
        {
            int screenWidth = renderingContext->getViewportWidth();
            int screenHeight = renderingContext->getViewportHeight();
            float aspect = (float)screenWidth / (float)screenHeight;
            uniformBlock.projectionMatrix = MatrixUtility::CreatePerspective(_camera.fovY, aspect, _camera.nearZ, _camera.farZ);

            Matrix4x4 rotationX = MatrixUtility::CreateRotationX(_camera.angleX);
            Matrix4x4 rotationY = MatrixUtility::CreateRotationY(_camera.angleY);
            Vector4 offset(0.0f, 0.0f, _camera.zoom, 1.0f);
            offset = (rotationY * rotationX) * offset;
            Vector3 eye = Vector3(_camera.focusPositionX, _camera.focusPositionY, _camera.focusPositionZ) + Vector3(offset.x, offset.y, offset.z);
            Vector3 center(_camera.focusPositionX, _camera.focusPositionY, _camera.focusPositionZ);
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

            renderingContext->enableVertexAttribute(0);
            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(Vector3), gridPositions);
            renderingContext->enableVertexAttribute(1);
            renderingContext->setVertexAttribute(1, 4, ComponentDataType::kFloat, sizeof(Vector4), gridColors);
            renderingContext->setIndexBuffer(kDefaultIndices, 2 * 2 * gridSize);
            renderingContext->enableVarying(0);
            renderingContext->setVertexShaderProgram(LineVertexShaderMain);
            renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

            renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            renderingContext->disableVarying(0);
            renderingContext->disableVertexAttribute(0);
            renderingContext->disableVertexAttribute(1);
        }

        // 座標軸を描画
        {
            const Vector3 xAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } };
            const Vector4 xAxisColors[2] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } };
            const Vector3 yAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } };
            const Vector4 yAxisColors[2] = { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
            const Vector3 zAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } };
            const Vector4 zAxisColors[2] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

            renderingContext->enableVertexAttribute(0);
            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(Vector3), xAxisPositions);
            renderingContext->enableVertexAttribute(1);
            renderingContext->setVertexAttribute(1, 4, ComponentDataType::kFloat, sizeof(Vector4), xAxisColors);
            renderingContext->setIndexBuffer(kDefaultIndices, 2);
            renderingContext->enableVarying(0);
            renderingContext->setVertexShaderProgram(LineVertexShaderMain);
            renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

            renderingContext->setDepthFunc(ComparisonFuncType::kLessEqual);
            renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(Vector3), yAxisPositions);
            renderingContext->setVertexAttribute(1, 4, ComponentDataType::kFloat, sizeof(Vector4), yAxisColors);
            renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(Vector3), zAxisPositions);
            renderingContext->setVertexAttribute(1, 4, ComponentDataType::kFloat, sizeof(Vector4), zAxisColors);
            renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

            renderingContext->disableVarying(0);
            renderingContext->disableVertexAttribute(0);
            renderingContext->disableVertexAttribute(1);
            renderingContext->setDepthFunc(ComparisonFuncType::kDefault);

        }

        // 色付き三角形を描画（頂点色は赤緑青の順、反時計周り）
        {
            const Vector3 polygonPositions[3] = { { -1.0f, 0.0f, 0.0f }, { -1.0f, 2.0f,  0.0f }, { -3.0f,  0.0f,  0.0f } };
            const Vector4 polygonColors[3] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

            renderingContext->setIndexBuffer(kDefaultIndices, 3);
            renderingContext->enableVertexAttribute(0);
            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(Vector3), polygonPositions);
            renderingContext->enableVertexAttribute(1);
            renderingContext->setVertexAttribute(1, 4, ComponentDataType::kFloat, sizeof(Vector4), polygonColors);
            renderingContext->setVertexShaderProgram(LineVertexShaderMain);// 流用
            renderingContext->enableVarying(0);
            renderingContext->setFragmentShaderProgram(LinePixelShaderMain);// 流用

            renderingContext->setFrontFaceMode(FrontFaceMode::kCounterClockwise);
            renderingContext->setCullFaceMode(CullFaceMode::kBack);

            renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

            renderingContext->disableVarying(0);
            renderingContext->disableVertexAttribute(0);
            renderingContext->disableVertexAttribute(1);
            renderingContext->setFrontFaceMode(FrontFaceMode::kDefault);
            renderingContext->setCullFaceMode(CullFaceMode::kDefault);
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

            renderingContext->setIndexBuffer(kMeshTriangles, kMeshTrianglesLength);
            renderingContext->enableVertexAttribute(0);
            renderingContext->setVertexAttribute(0, 3, ComponentDataType::kFloat, sizeof(float) * 3, kMeshVertices);
            renderingContext->enableVertexAttribute(1);
            renderingContext->setVertexAttribute(1, 2, ComponentDataType::kFloat, sizeof(float) * 2, kMeshUvs);
            renderingContext->enableVertexAttribute(2);
            renderingContext->setVertexAttribute(2, 3, ComponentDataType::kFloat, sizeof(float) * 3, kMeshNormals);
            renderingContext->enableVarying(0);
            renderingContext->enableVarying(1);
            renderingContext->setVertexShaderProgram(MeshVertexShaderMain);
            renderingContext->setFragmentShaderProgram(MeshPixelShaderMain);

            renderingContext->setFrontFaceMode(FrontFaceMode::kCounterClockwise);
            renderingContext->setCullFaceMode(CullFaceMode::kBack);

            renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

            renderingContext->disableVarying(0);
            renderingContext->disableVarying(1);
            renderingContext->disableVertexAttribute(0);
            renderingContext->disableVertexAttribute(1);
            renderingContext->disableVertexAttribute(2);
            renderingContext->setFrontFaceMode(FrontFaceMode::kDefault);
            renderingContext->setCullFaceMode(CullFaceMode::kDefault);

            uniformBlock.modelMatrix = Matrix4x4::kIdentity;
            uniformBlock.meshTexture = nullptr;
        }

    }
}
