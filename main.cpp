#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "include/filesystem.h"
#include "include/shader_s.h"
#include "include/Camara.h"
#include "Objecto.h"
#include "gly_ply.h"
#include "include/stb_image.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float fuerza = 0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float tiempoInicial = 0.0f, tiempoTranscurrido = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 30.0f, 2.0f);

Esfera esfera(vec3(0),2., 20, 20);
Esfera *pEsfera = new Esfera(vec3(0),2, 50, 50);
Esfera modelo(vec3(5,5,5));
vector<Objeto*> objetos;
bool boton_presionado = false;

Model_PLY conejo;

int main() {
    char *archivo = "../resources/models/bunny.ply";
    conejo.Load(archivo);

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)     {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))     {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader lightingShader("../resources/shaders/2.1.basic_lighting.vs", "../resources/shaders/2.1.basic_lighting.fs");
    //Shader lightCubeShader("../2.2.light_cube.vs", "../2.2.light_cube.fs");

    esfera.setup();
    //objetos.emplace_back(pEsfera);
    modelo.vao = esfera.vao;
    modelo.color=vec3(0.5f,0.5f,0.5f);
    modelo.radius = esfera.radius;
    modelo.indices_size = esfera.indices_size;
    modelo.x0 = 0.5f;
    modelo.y0 = 0.5f;
    modelo.z0 = 0.5f;
    modelo.actualizarBS();
    float intersect_t = 0;

    conejo.setup();
    conejo.centro = vec3(0.6,0.5,3);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        tiempoTranscurrido = currentFrame - tiempoInicial; //static_cast<float>(glfwGetTime());
        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        //esfera.display(lightingShader);
        //pEsfera->display(lightingShader);
        int objT;
        bool prev_intersect = false;
        for (auto &obj: objetos) {
            objT = 0;
            // be sure to activate shader when setting uniforms/drawing objects
            lightingShader.use();
            lightingShader.setVec3("objectColor", obj->color.x, obj->color.y, obj->color.z);
            lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            lightingShader.setVec3("lightPos", lightPos);
            lightingShader.setVec3("viewPos", camera.Position);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                    0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            lightingShader.setMat4("model", model);
            obj->actualizarPosicion(tiempoTranscurrido);
            obj->display(lightingShader,0.1);
            if (obj->bs->intersecta(*modelo.bs)) {
                modelo.color = vec3(1, 0, 0);
                modelo.centro = vec3 (rand()%20-10,rand()%20-10,0);
                prev_intersect = true;
                modelo.actualizarBS();
                intersect_t = tiempoTranscurrido;
            } else {
                if (!prev_intersect){
                    if(intersect_t + 1 <= tiempoTranscurrido){
                        modelo.color = vec3(0.5f, 0.5f, 0.5f);
                        intersect_t = 0;
                    }
                }
            }
            if (obj->expire(tiempoTranscurrido)) {
                objetos.erase(objetos.begin() + objT);
            }
        }
        lightingShader.setVec3("objectColor", modelo.color.x, modelo.color.y, modelo.color.z);
        modelo.display(lightingShader,0.4f);
        objT=0;
        lightingShader.setVec3("objectColor", (float) rand()/RAND_MAX, (float)rand()/RAND_MAX,(float)rand()/RAND_MAX);
        conejo.display(lightingShader,1);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    //   glDeleteVertexArrays(1, &cubeVAO);
    //   glDeleteVertexArrays(1, &lightCubeVAO);
    //   glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        boton_presionado = true;
        fuerza+=0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE){
        if (boton_presionado) {
            float x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
            float y =  sin(glm::radians(camera.Pitch));
            float z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
            Objeto *pE = new Esfera(glm::vec3(x,y,z));
            pE->v0 = vec3(x*100,y*70,z*fuerza);
            pE->a0 = 40 + rand() % 20;
            pE->x0 = x;
            pE->y0 = y;
            pE->z0 = z+100;
            pE->vao = esfera.vao;
            pE->color = vec3((float) rand()/RAND_MAX,(float) rand()/RAND_MAX,(float) rand()/RAND_MAX);
            pE->startTime = static_cast<float>(glfwGetTime());
            pE->indices_size = esfera.indices_size;
            Esfera * pES = (Esfera *) pE;
            pES->radius=esfera.radius;
            objetos.emplace_back(pE);
            boton_presionado = false;
            fuerza=0;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}