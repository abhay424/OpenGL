
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MyWindow.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "ShadowMap.h"
#include "MatrixTransform.h"
#include "SkyBox.h"

// Window dimensions
//const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLfloat deltaTime = 0.0f,
        lastTime  = 0.0f;

// Global variables.

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;
Texture brickTexture,
        dirtTexture,
        planeTexture;

DirectionalLight mainLight;
PointLight  pointLights[MAX_NUM_POINT_LIGHTS];
SpotLight   spotLights[MAX_NUM_SPOT_LIGHTS];

Material shinyMaterial,
         dullMaterial;

Model xWing, blackHawk;

float blackHawkAngle = 0.0f;

SkyBox* skybox = nullptr;

// Vertex Shader
static const char* vShader = "Shaders/vertex.shader";

// Fragment shader
static const char* fShader = "Shaders/fragment.shader";

//------------------------------------------------------------------------------
void CreateObjects()
{
    unsigned int indices[]{
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //   x      y     z         u     v         Nx    Ny    Nz
        -1.0f, -1.0f,-0.6f,    0.0f, 0.0f,      0.0f, 0.0f, 0.0f,
         0.0f, -1.0f, 1.0f,    0.5f, 0.0f,      0.0f, 0.0f, 0.0f,
         1.0f, -1.0f,-0.6f,    1.0f, 0.0f,      0.0f, 0.0f, 0.0f,
         0.0f,  1.0f, 0.0f,    0.5f, 1.0f,      0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
    //   x      y     z         u      v         Nx    Ny    Nz
        -10.0f, 0.0f, -10.0f,   0.0f,  0.0f,    0.0f, -1.0f, 0.0f,
         10.0f, 0.0f, -10.0f,  10.0f,  0.0f,    0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f,  10.0f,   0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
         10.0f, 0.0f,  10.0f,  10.0f, 10.0f,    0.0f, -1.0f, 0.0f
    };

    unsigned int nbVertices     = 32,
                 nbIndices      = 12;

    Mesh* obj1 = new Mesh();
    obj1->Create( vertices, nbVertices, indices, nbIndices );
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->Create( vertices, nbVertices, indices, nbIndices );
    meshList.push_back(obj2);

    nbIndices = 6;

    Mesh* obj3 = new Mesh();
    obj3->Create( floorVertices, nbVertices, floorIndices, nbIndices, true );
    meshList.push_back(obj3);
}

//------------------------------------------------------------------------------
void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFile( vShader, nullptr, fShader );
    shaderList.push_back(*shader1);

    directionalShadowShader.CreateFromFile( "Shaders/directional_shadow_map_vertex.shader", nullptr,
                                            "Shaders/directional_shadow_map_fragment.shader" );

    omniShadowShader.CreateFromFile( "Shaders/omni_shadow_map_vertex.shader",
                                     "Shaders/omni_shadow_map_geometry.shader",
                                     "Shaders/omni_shadow_map_fragment.shader" );
}

//------------------------------------------------------------------------------
void InitializeMainLight()
{
    // Directional light source.
    mainLight.SetAmbientColor(1.0f, 0.53f, 0.3f);
    mainLight.SetAmbientIntensity(0.1f);
    mainLight.SetDiffuseIntensity(0.9f);
    mainLight.SetDirection(-8.0f, -12.0f, 18.5f);
    mainLight.CreateShadowMap(2048, 2048);
}

//------------------------------------------------------------------------------
unsigned int InitializePointLights()
{
    pointLights[0] = PointLight();
    pointLights[0].SetAmbientColor(0.0f, 0.0f, 1.0f);
    pointLights[0].SetAmbientIntensity(0.1f);
    pointLights[0].SetDiffuseIntensity(1.0f);
    pointLights[0].SetPosition(1.0f, 2.0f, 0.0f);
    pointLights[0].SetAttenuationCoefficients(0.3f, 0.2f, 0.1f);
    pointLights[0].CreateShadowMap(1024, 1024);
    pointLights[0].SetFarAndNearPlane(0.01f, 100.0f);

    pointLights[1] = PointLight();
    pointLights[1].SetAmbientColor(0.0f, 1.0f, 0.0f);
    pointLights[1].SetAmbientIntensity(0.1f);
    pointLights[1].SetDiffuseIntensity(1.0f);
    pointLights[1].SetPosition(-4.0f, 3.0f, 0.0f);
    pointLights[1].SetAttenuationCoefficients(0.3f, 0.2f, 0.1f);
    pointLights[1].CreateShadowMap(1024, 1024);
    pointLights[1].SetFarAndNearPlane(0.01f, 100.0f);

    return 2;
}

//------------------------------------------------------------------------------
unsigned int InitializeSpotLights()
{
    spotLights[0] = SpotLight();
    spotLights[0].SetAmbientColor(1.0f, 1.0f, 1.0f);
    spotLights[0].SetAmbientIntensity(0.1f);
    spotLights[0].SetDiffuseIntensity(2.0f);
    spotLights[0].SetPosition(0.0f, 0.0f, 0.0f);
    spotLights[0].SetAttenuationCoefficients(1.0f, 0.0f, 0.0f);
    spotLights[0].SetDirection(0.0f, -1.0f, 0.0f);
    spotLights[0].SetEdgeAngle(20.f);
    spotLights[0].CreateShadowMap(1024, 1024);
    spotLights[0].SetFarAndNearPlane(0.01f, 100.0f);

    spotLights[1] = SpotLight();
    spotLights[1].SetAmbientColor(1.0f, 1.0f, 1.0f);
    spotLights[1].SetAmbientIntensity(0.1f);
    spotLights[1].SetDiffuseIntensity(1.0f);
    spotLights[1].SetPosition(0.0f, -1.5f, 0.0f);
    spotLights[1].SetAttenuationCoefficients(1.0f, 0.0f, 0.0f);
    spotLights[1].SetDirection(-100.0f, -1.0f, 0.0f);
    spotLights[1].SetEdgeAngle(20.f);
    spotLights[1].CreateShadowMap(1024, 1024);
    spotLights[1].SetFarAndNearPlane(0.01f, 100.0f);

    return 2;
}

//------------------------------------------------------------------------------
void Render( Shader& shader, Texture& texture,
             Mesh& mesh, Material& material,
             MatrixTransform &transform )
{
    shader.SetModelMat4(transform.AsGlmMat4());
    texture.Use();
    shader.SetMaterial(material);
    mesh.Render();
}

//------------------------------------------------------------------------------
void Render( Shader& shader,
             Model& model, Material& material,
             MatrixTransform &transform )
{
    shader.SetModelMat4(transform.AsGlmMat4());
    shader.SetMaterial(material);
    model.Render();
}

//------------------------------------------------------------------------------
void RenderScene( Shader& shader )
{
    shader.Validate();

    // Render brick pyramid.
    MatrixTransform modelTransform;
    modelTransform.Translate(glm::vec3(0.0f, 0.0f, -2.50f));
    Render(shader, brickTexture,
           *meshList[0], shinyMaterial, modelTransform);

    // Render dirt pyramid.
    modelTransform = MatrixTransform();
    modelTransform.Translate(glm::vec3(0.0f, 4.0f, -2.50f));
    Render(shader, dirtTexture, *meshList[1],
           dullMaterial, modelTransform);

    // Render floor
    modelTransform = MatrixTransform();
    modelTransform.Translate(glm::vec3(0.0f, -2.0f, 0.0f));
    Render(shader, dirtTexture, *meshList[2],
           shinyMaterial, modelTransform);

    // Render Model-> xwing
    modelTransform = MatrixTransform();
    modelTransform.Translate(glm::vec3(-7.0f, 0.0f, 10.0f));
    modelTransform.Scale(0.006f, 0.006f, 0.006f);
    Render(shader, xWing, shinyMaterial,
           modelTransform);

    // Render Model-> blackhawk
    blackHawkAngle += 0.1f;
    if( blackHawkAngle > 360.0f )
    {
        blackHawkAngle = 0.0f;
    }

    modelTransform = MatrixTransform();
    // Rotate about y-axis passing thorough its own center.
    modelTransform.Rotate(-blackHawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    modelTransform.Translate(glm::vec3(-8.0f, 2.0f, 0.0f));
    // Tilt it by 20 degree to give rotation feeling.
    modelTransform.Rotate(-20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    modelTransform.Rotate(-90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTransform.Scale(0.4f, 0.4f, 0.4f);
    Render(shader, blackHawk, shinyMaterial,
           modelTransform);
}

//------------------------------------------------------------------------------
void DirectionalLightShadowPass( MyWindow& myWindow, DirectionalLight& dLight )
{
    directionalShadowShader.Use();

    ShadowMap* shadowMap = dLight.GetShadowMap();
    // Set same viewport to window as shadow map/frame buffer.
    // So that our viewport is of same dimention as our frame buffer.
    myWindow.SetViewport(shadowMap->GetWidth(), shadowMap->GetHeight());

    shadowMap->Write();
    // Clear depth buffer if any depth buffer is there in frame buffer.
    glClear(GL_DEPTH_BUFFER_BIT);

    directionalShadowShader.SetDirectionalLigthTransform(dLight.CalculateLightTransform());

    RenderScene(directionalShadowShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//------------------------------------------------------------------------------
void OmniLightShadowPass( MyWindow& myWindow, PointLight& pLight )
{
    omniShadowShader.Use();

    ShadowMap* shadowMap = pLight.GetShadowMap();
    // Set same viewport to window as shadow map/frame buffer.
    // So that our viewport is of same dimention as our frame buffer.
    myWindow.SetViewport(shadowMap->GetWidth(), shadowMap->GetHeight());

    shadowMap->Write();
    // Clear depth buffer if any depth buffer is there in frame buffer.
    glClear(GL_DEPTH_BUFFER_BIT);

    omniShadowShader.SetOmniLightPosition(pLight.Position());
    omniShadowShader.SetOmniLightFarPlane(pLight.GetFarPlane());
    omniShadowShader.SetOmniLightMatrices(pLight.CalculateLightTransforms());

    RenderScene(omniShadowShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//------------------------------------------------------------------------------
void RenderPass( MyWindow& myWindow,
                 const glm::mat4& projectionMat, const glm::mat4& viewMatrix,
                 unsigned int pointLightCount, unsigned int spotLightCount )
{
    // Set viewport to initial window viewport as shadow map might have changed it.
    myWindow.SetViewport(1366, 768);

    // Clear window.
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //myWindow.SetBackgroundColor(0.21f, 0.32f, 0.40f, 1.0f);
    myWindow.SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox->Draw(viewMatrix, projectionMat);

    shaderList[0].Use();

    // Set Matrices.
    shaderList[0].SetProjectionMat4(projectionMat);
    shaderList[0].SetViewMat4(viewMatrix);
    // Set Eye position.
    // Set camera position as eye position.
    shaderList[0].SetEyePosition(camera.Position());

    // For light.
    shaderList[0].SetDirectionalLight(mainLight);
    shaderList[0].SetPointLight(pointLights, pointLightCount, 3 /*textureUnitOffset*/);
    shaderList[0].SetSpotLight(spotLights, spotLightCount, 3 + pointLightCount/*textureUnitOffset*/);

    shaderList[0].SetDirectionalLigthTransform(mainLight.CalculateLightTransform());

    // Will use 0 for default texture.

    mainLight.GetShadowMap()->Read(GL_TEXTURE2);
    shaderList[0].SetTexture(1);
    shaderList[0].SetDirectionalLightShadowMap(2);

    spotLights[0].SetFlash(camera.Position(), camera.Direction());

    RenderScene(shaderList[0]);
}

int main()
{
    MyWindow mainWindow(1366, 768); // 1280, 1024 or 1024, 768
    if( !mainWindow.Initialize() )
    {
        return 1;
    }

    CreateObjects();
    CreateShaders();
    //camera = Camera( glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f);

    char brick[50] = "Textures/brick.png";
    brickTexture = Texture(brick);
    brickTexture.Load();
    char dirt[50] = "Textures/dirt.png";
    dirtTexture = Texture(dirt);
    dirtTexture.Load();
    char plane[50] = "Textures/plain.png";
    planeTexture = Texture(plane);
    planeTexture.Load();

    xWing.Load("Models/x-wing.obj");
    blackHawk.Load("Models/uh60.obj");

    // Directional light source.
    InitializeMainLight();
    //mainLight.TurnOff();

    unsigned int pointLightCount = InitializePointLights();
    unsigned int spotLightCount = InitializeSpotLights();

    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

    skybox = new SkyBox(skyboxFaces);

    shinyMaterial.SetSpecularIntensity(4.0f);
    shinyMaterial.SetShininess(256.0f);

    dullMaterial.SetSpecularIntensity(0.3f);
    dullMaterial.SetShininess(4.0f);

    //glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.BufferWidth() / mainWindow.BufferHeight(), 0.1f, 100.0f );

    // Loop until window closed.
    while( !mainWindow.ShouldClose() )
    {
        double now = glfwGetTime(); // SDL_GetPerformanceCounter();
        deltaTime = now - lastTime; // (now - lastTime) * 1000/SDL_GetPerformanceFrequency();
        lastTime = now;

        // Get and Handle user input events.
        // This will chech if any user event happened.
        glfwPollEvents();

        camera.OnKeyPressed(&mainWindow, deltaTime);
        if( mainWindow.IsKeyPressed(GLFW_KEY_L) &&
            !mainWindow.IsKeyEventAccepted(GLFW_KEY_L) )
        {
            spotLights[0].Toggle();
            mainWindow.AcceptKeyEvent(GLFW_KEY_L);
        }
        camera.OnMouseMoved(&mainWindow);
        //camera.OnMouseScrolled(&mainWindow);

        // Render to frame buffer and save it to texture,
        // then Render pass will use that texture.
        DirectionalLightShadowPass(mainWindow, mainLight);
        for( unsigned int i = 0; i < pointLightCount; ++i )
        {
            OmniLightShadowPass(mainWindow, pointLights[i]);
        }
        for( unsigned int i = 0; i < spotLightCount; ++i )
        {
            OmniLightShadowPass(mainWindow, spotLights[i]);
        }

        //RenderPass(mainWindow, projection, camera.ViewMatrix(),
        //           pointLightCount, spotLightCount);
        RenderPass(mainWindow, mainWindow.GetProjectionMatrix(), camera.ViewMatrix(),
                   pointLightCount, spotLightCount);

        glUseProgram(0);

        mainWindow.SwapBuffer(); // Swap with the one being shown.
    }

    return 0;
}
