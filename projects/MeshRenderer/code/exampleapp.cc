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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path, bool gammaCorrection);
void renderQuad();
void renderCube();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera = Camera(90, SCR_WIDTH / SCR_HEIGHT, 0.1, 1000);

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
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
        Em = Evp = Translate(M4(), V4());
        window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
        {
            //deltatime
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
                Evp = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
            }
        });
        
         // five randomly selected models: avocado, sphere, cube, pyramid, monkey, teapot
        return true;
        if (this->window->Open())
        {
            // set clear color to gray
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            //MeshResource

            avocadoModel = GraphicNode::load_gltf("textures/Avocado.glb");
            
            //cube = MeshResource::LoadObj("textures/cube.obj");
            std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/test.jpg");
            
            //TextureResource
            
            if (avocadoModel != nullptr)
            {
                cube = MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*avocadoModel));
                
                if (!avocadoModel->materials.empty())
                {
                    const tinygltf::Material& material = avocadoModel->materials[0];
                    const int baseColorIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                    if (baseColorIndex != -1)
                    {
                        const unsigned char* texBuf = (const unsigned char*)(&avocadoModel->images[baseColorIndex].image[0]);
                        int texW = avocadoModel->images[baseColorIndex].width;
                        int texH = avocadoModel->images[baseColorIndex].height;
                        int texComp = avocadoModel->images[baseColorIndex].component;
                        texture->LoadTextureFromModel(texBuf, texW, texH, texComp);
                    }
                    else
                    {
                        texture->LoadTextureFromFile();
                    }

                    const int normalMapIndex = material.normalTexture.index;
                    if (normalMapIndex != -1)
                    {
                        const unsigned char* normalBuf = (const unsigned char*)(&avocadoModel->images[normalMapIndex].image[0]);
                        int normalW = avocadoModel->images[normalMapIndex].width;
                        int normalH = avocadoModel->images[normalMapIndex].height;
                        int normalComp = avocadoModel->images[normalMapIndex].component;
                        texture->LoadNormalMapFromModel(normalBuf, normalW, normalH, normalComp);
                    }
                    else
                    {
                        texture->LoadNormalMapFromFile("textures/perfect.jpg");
                    }
                }
                else
                {
                    texture->LoadTextureFromFile();
                    texture->LoadNormalMapFromFile("textures/perfect.jpg");
                }
            }


            //shaderObject
            //shaderResource = std::make_shared<ShaderResource>(this->vertexShader, this->pixelShader/*, this->program*/);
            
            //GraphicNode
            node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Identity());

            if (avocadoModel != nullptr)
            {
                for (const tinygltf::Camera& camera : avocadoModel->cameras)
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

            return true;
        }
        return false;
    }

void ExampleApp::Run()
{
    camera.setPos(V3(0.f, 0.f, 5.f));
    
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    std::shared_ptr<ShaderResource> shaderGeometryPass = std::make_shared<ShaderResource>("textures/g_buffer.vs", "textures/g_buffer.ps");
    std::shared_ptr<ShaderResource> shaderLightingPass = std::make_shared<ShaderResource>("textures/deferred_shading.vs", "textures/deferred_shading.ps");
    std::shared_ptr<ShaderResource> shaderLightBox = std::make_shared<ShaderResource>("textures/light_source.vs", "textures/light_source.ps");

    std::shared_ptr<tinygltf::Model> avocadoModel = GraphicNode::load_gltf("textures/Avocado.glb");

    std::shared_ptr<GraphicNode> avocadoNode;
    avocadoNode->setGeometry(MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*avocadoModel)));

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
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
    glUseProgram(shaderLightingPass->program);
    shaderLightingPass->setInt(0, "gPosition");
    shaderLightingPass->setInt(1, "gNormal");
    shaderLightingPass->setInt(2, "gAlbedoSpec");

    // render loop
    // -----------
    while (!window->IsOpen())
    {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        M4 proj = projection(90.f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        M4 view = camera.view(true);
        M4 model = Identity();
        glUseProgram(shaderGeometryPass->program);
        shaderGeometryPass->setM4(proj, "projection");
        shaderGeometryPass->setM4(view, "view");
        for (unsigned int i = 0; i < objectPositions.size(); i++)
        {
            model = Identity();
            model = Translate(model, objectPositions[i]);
            model = Scalar(model, V3(0.5f, 0.5f, 0.5f));
            shaderGeometryPass->setM4(model, "model");
            //avocado->Draw(shaderGeometryPass);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderLightingPass->program);
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
        shaderLightingPass->setV3(camera.getPos(), "viewPos");
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. render lights on top of scene
        // --------------------------------
        glUseProgram(shaderLightBox->program);
        shaderLightBox->setM4(proj, "projection");
        shaderLightBox->setM4(view, "view");
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = Identity();
            model = Translate(model, lightPositions[i]);
            model = Scalar(model, V3(0.125f, 0.125f, 0.125f));
            shaderLightBox->setM4(model, "model");
            shaderLightBox->setV3(lightColors[i], "lightColor");
            renderCube();
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        this->window->SwapBuffers();
        glfwPollEvents();
    }

    glfwTerminate();
    return;
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