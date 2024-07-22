#include "config.h"
#include "exampleapp.h"
#include <iostream>

namespace Example
{

    ExampleApp::ExampleApp()
    {

    }

    ExampleApp::~ExampleApp()
    {

    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void renderQuad();
    void renderCube();

    bool firstMouse = true;

    double lastX, lastY;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    bool ExampleApp::Open()
    {
        App::Open();
        this->window = new Display::Window;

        //srand((unsigned)time(nullptr));

        //assign ExampleApp variables
        w = a = s = d = q = e = false;
        window->GetSize(width, height);
        window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
            {
                //delta-time
                switch (keycode)
                {
                case GLFW_KEY_ESCAPE: window->Close(); break;
                case GLFW_KEY_W: w = action; break;
                case GLFW_KEY_S: s = action; break;
                case GLFW_KEY_A: a = action; break;
                case GLFW_KEY_D: d = action; break;

                case GLFW_KEY_Q: q = action; break;
                case GLFW_KEY_E: e = action; break;
                }
            });

        window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
            {
                isRotate = button == GLFW_MOUSE_BUTTON_1 && action;
                if (!isRotate)
                {
                    prevX = senseX;
                    prevY = senseY;
                }
            });

        window->SetMouseMoveFunction([this](float64 x, float64 y)
            {
                if (isRotate)
                {
                    senseX = prevX + (0.002 * (x - width / 2));
                    senseY = prevY + (0.002 * (y - height / 2));
                    //Evp = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
                }
            });


        if (this->window->Open())
        {
            // set clear color to gray
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            //MeshResource
            //std::shared_ptr<MeshResource> mesh = MeshResource::LoadObj("textures/cube.obj");

            model = GraphicNode::load_gltf("textures/Avocado.glb");

            texture = std::make_shared<TextureResource>("textures/test.jpg", "textures/perfect.jpg", "textures/perfect.jpg");

            //TextureResource

            if (model != nullptr)
            {
                mesh = MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*model));

                if (!model->materials.empty())
                {
                    const tinygltf::Material& material = model->materials[0];
                    const int baseColorIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                    if (baseColorIndex != -1)
                    {
                        const unsigned char* texBuf = (const unsigned char*)(&model->images[baseColorIndex].image[0]);
                        int texW = model->images[baseColorIndex].width;
                        int texH = model->images[baseColorIndex].height;
                        int texComp = model->images[baseColorIndex].component;
                        texture->LoadTextureFromBuffer(&texture->diffuseAlbedo, 0, texBuf, texW, texH, texComp);
                    }
                    else
                    {
                        texture->LoadTextureFromFile(&texture->diffuseAlbedo, 0, "textures/perfect.jpg");
                    }

                    const int normalMapIndex = material.normalTexture.index;
                    if (normalMapIndex != -1)
                    {
                        const unsigned char* normalBuf = (const unsigned char*)(&model->images[normalMapIndex].image[0]);
                        int normalW = model->images[normalMapIndex].width;
                        int normalH = model->images[normalMapIndex].height;
                        int normalComp = model->images[normalMapIndex].component;
                        texture->LoadTextureFromBuffer(&texture->normalMap, 1, normalBuf, normalW, normalH, normalComp);
                    }
                    else
                    {
                        texture->LoadTextureFromFile(&texture->normalMap, 1, "textures/perfect.jpg");
                    }

                    
                    const int specularMapIndex = material.occlusionTexture.index;
                    if (specularMapIndex != -1)
                    {
                        const unsigned char* specBuf = (const unsigned char*)(&model->images[specularMapIndex].image[0]);
                        int normalW = model->images[specularMapIndex].width;
                        int normalH = model->images[specularMapIndex].height;
                        int normalComp = model->images[specularMapIndex].component;
                        texture->LoadTextureFromBuffer(&texture->specularMap, 2, specBuf, normalW, normalH, normalComp);
                    }
                    else
                    {
                        texture->LoadTextureFromFile(&texture->specularMap, 2, "textures/perfect.jpg");
                    }
                    const int other = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
                }
                else
                {
                    texture->LoadTextureFromFile(&texture->diffuseAlbedo, 0, "textures/test.jpg");
                    texture->LoadTextureFromFile(&texture->normalMap, 1, "textures/perfect.jpg");
                    texture->LoadTextureFromFile(&texture->specularMap, 2, "textures/perfect.jpg");
                }
            }

            //shaderObject
            //shaderResource = std::make_shared<ShaderResource>("textures/vs.glsl", "textures/ps.glsl");

            //Checklist
            //std::make_shared<GraphicNode>(mesh, texture, shaderResource, Identity());

            if (model != nullptr && model->cameras.size())
            {
                for (const tinygltf::Camera& camera : model->cameras)
                {
                    if (camera.type == "perspective")
                    {
                        const tinygltf::PerspectiveCamera& per = camera.perspective;
                        per.yfov;
                        per.aspectRatio;
                        per.znear;
                        per.zfar;
                        cam = std::make_shared<Camera>(per.yfov, per.aspectRatio, per.znear, per.zfar);
                    }
                }
            }
            else
            {
                cam = std::make_shared<Camera>(90, (float)width / height, 0.1, 1000);
            }

            return true;
        }
        return false;
    }

    void ExampleApp::Run()
    {
        cam->setPos(V3(0.f, 0.f, 5.f));

        stbi_set_flip_vertically_on_load(true);
        glEnable(GL_DEPTH_TEST);

        // build and compile shaders
        // -------------------------
        shaderGeometryPass = std::make_shared<ShaderResource>("textures/g_buffer.vs", "textures/g_buffer.ps");
        shaderLightingPass = std::make_shared<ShaderResource>("textures/deferred_shading.vs", "textures/deferred_shading.ps");
        shaderLightBoxPass = std::make_shared<ShaderResource>("textures/lightSource.vs", "textures/lightSource.ps");

        std::shared_ptr<tinygltf::Model> avocadoModel = GraphicNode::load_gltf("textures/Avocado.glb");

        mesh = MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*avocadoModel));
        std::shared_ptr<GraphicNode> avocadoNode = std::make_shared<GraphicNode>(mesh, texture,shaderGeometryPass, Identity());

        // TODO: random locations
        std::vector<V3> objectPositions;
        objectPositions.push_back(V3(-3.0, -0.5, -3.0));
        objectPositions.push_back(V3(0.0, -0.5, -3.0));
        objectPositions.push_back(V3(3.0, -0.5, -3.0));
        objectPositions.push_back(V3(-3.0, -0.5, 0.0));
        objectPositions.push_back(V3(0.0, -0.5, 0.0));
        objectPositions.push_back(V3(3.0, -0.5, 0.0));
        objectPositions.push_back(V3(-3.0, -0.5, 3.0));
        objectPositions.push_back(V3(0.0, -0.5, 3.0));
        objectPositions.push_back(V3(3.0, -0.5, 3.0));


        // configure g-buffer framebuffer
        // ------------------------------
        unsigned int gBuffer;
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        unsigned int gPosition, gNormal, gAlbedoSpec;
        // position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        // normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        // color + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // lighting info
        // -------------
        const unsigned int NR_LIGHTS = 20;
        std::vector<V3> lightPositions;
        std::vector<V3> lightColors;
        srand(1338);
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            lightPositions.push_back(V3(xPos, yPos, zPos));
            // also calculate random color
            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            lightColors.push_back(V3(rColor, gColor, bColor));
        }

        // shader configuration
        // --------------------
        shaderLightingPass->useProgram();
        shaderLightingPass->setInt(0, "gPosition");
        shaderLightingPass->setInt(1, "gNormal");
        shaderLightingPass->setInt(2, "gAlbedoSpec");

        // render loop
        // -----------
        while (window->IsOpen())
        {
            auto currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            cam->setPos(cam->getPos() + Normalize(V3(a-d, q-e, s-w)) * deltaTime);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //cam->pvm();
            M4 proj = projection(90.f, (float)width / (float)height, 0.1f, 100.0f);
            M4 view = cam->view(false);
            M4 model = Identity();
            shaderGeometryPass->useProgram();
            shaderGeometryPass->setM4(proj, "projection");
            shaderGeometryPass->setM4(view, "view");
            for (unsigned int i = 0; i < objectPositions.size(); i++)
            {
                model = Scalar(V3(0.5f, 0.5f, 0.5f)) *
                Translate(objectPositions[i]);
                shaderGeometryPass->setM4(model, "model");
                //avocadoNode->getGeometry()->Draw(shaderGeometryPass);
            }
            float dst = Length(objectPositions[0] - cam->getPos());
            
            std::cout << ("dist: %.3f", dst) << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
            // -----------------------------------------------------------------------------------------------------------------------
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaderLightingPass->useProgram();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            // send light relevant uniforms
            for (unsigned int i = 0; i < lightPositions.size(); i++)
            {
                shaderLightingPass->setV3(lightPositions[i], "lights[" + std::to_string(i) + "].Position");
                shaderLightingPass->setV3(lightColors[i], "lights[" + std::to_string(i) + "].Color");
                shaderLightingPass->setFloat(0.7f, "lights[" + std::to_string(i) + "].Linear");
                shaderLightingPass->setFloat(1.8f, "lights[" + std::to_string(i) + "].Quadratic");
            }
            shaderLightingPass->setV3(
                cam->getPos(), "viewPos");
            // finally render quad
            renderQuad();

            // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
            // ----------------------------------------------------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer // WARNING: API_ID_REDUNDANT_FBO
            // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
            // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
            // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 3. render lights on top of scene
            // --------------------------------
            shaderLightBoxPass->useProgram();
            shaderLightBoxPass->setM4(proj, "projection");
            shaderLightBoxPass->setM4(view, "view");
            for (unsigned int i = 0; i < lightPositions.size(); i++)
            {
                model = Identity();
                model = Translate(lightPositions[i]);
                model = Scalar(V3(0.125f, 0.125f, 0.125f));
                shaderLightBoxPass->setM4(model, "model");
                shaderLightBoxPass->setV3(lightColors[i], "lightColor");
                renderCube();
            }


            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            this->window->SwapBuffers();
            glfwPollEvents();
        }

        glfwTerminate();
        return;
        // TODO:
#define testRH
    }

    // renderCube() renders a 1x1 3D cube in NDC.
    // -------------------------------------------------
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    void renderCube()
    {
        // initialize (if necessary)
        if (cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                 // bottom face
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 // top face
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }


    // renderQuad() renders a 1x1 XY quad in NDC
    // -----------------------------------------
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        //camera.ProcessMouseMovement(xoffset, yoffset);
    }
}
