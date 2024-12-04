#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 위치 셰이더 -> 위치를 정함 (transform vertices)
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // 3차원 벡터에 homogeneous coordinates -> 마지막 원소 1 
    "}\0";

// 프래그먼트셰이더 (Colorization)
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // 마찬가지 - 대신 플롯트로 표현하는듯
    "}\n\0";

int main()
{
    // 여기서부터
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3버전으로 만드는거군아 ;;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 여기까진 항상 들어가는 초기화 맨

    // build and compile our shader program
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // 버텍스셰이더 객체 생성
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 위의 char * 로 만드는듯
    glCompileShader(vertexShader);


    // 셰이더 생성 로그
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // 프래그먼트 셰이더 객체 생성
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // 마찬가지로 위의 char * 로 선언
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }




    // link shaders
    unsigned int shaderProgram = glCreateProgram();// 새로운 셰이더 프로그램 객체를 생성 - 여러개의 셰이더 연결
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);// 다 연결하고 링크한다네요
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 다 연결했으니 지워주고..
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // 버텍스! - x y z 좌표
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    /* glGenVertexArrays(1, &VAO);: 하나의 버텍스 배열 객체(VAO)를 생성합니다. 
    VAO는 버텍스 속성(예: 위치, 색상 등)과 관련된 모든 상태를 저장하는데 사용됩니다.
    glGenBuffers(1, &VBO);: 하나의 버텍스 버퍼 객체(VBO)를 생성합니다. 
    VBO는 버텍스 데이터를 GPU에 저장하는데 사용됩니다.*/
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // VAO를 바인딩하여 이후의 모든 버텍스 속성 설정이 이 VAO와 연관되도록 합니다.
    glBindVertexArray(VAO);

    /* glBindBuffer(GL_ARRAY_BUFFER, VBO);: 현재 사용 중인 VBO를 바인딩합니다. 여기서는 버텍스 데이터를 저장할 배열 버퍼를 선택합니다.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);: 선택한 VBO에 데이터를 전송합니다.
    GL_ARRAY_BUFFER: 버퍼의 종류로, 이 경우 버텍스 데이터에 사용됩니다.
    sizeof(vertices): 전송할 데이터의 크기입니다.
    vertices: 전송할 데이터의 포인터입니다.
    GL_STATIC_DRAW: 데이터가 변경되지 않을 것이라는 힌트를 제공합니다. 이는 GPU가 데이터를 어떻게 최적화할지를 결정하는 데 도움을 줍니다.*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    /* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);:
    첫 번째 인자 0: 이 속성의 인덱스입니다. 이 경우, 0은 첫 번째 버텍스 속성을 의미합니다.
    두 번째 인자 3: 각 버텍스가 3개의 구성 요소(즉, x, y, z 좌표)를 가짐을 나타냅니다.
    세 번째 인자 GL_FLOAT: 각 구성 요소의 데이터 타입을 나타냅니다.
    네 번째 인자 GL_FALSE: 데이터를 정규화하지 않겠다는 의미입니다.
    다섯 번째 인자 3 * sizeof(float): 다음 버텍스 속성 간의 간격(바이트 단위)을 설정합니다. 이 경우는 각 버텍스가 3개의 float 값으로 구성되어 있습니다.
    여섯 번째 인자 (void*)0: 버퍼의 시작 지점을 지정합니다.
    glEnableVertexAttribArray(0);: 위에서 설정한 속성을 활성화합니다. 즉, VAO에서 인덱스가 0인 버텍스 속성을 사용할 수 있도록 합니다.*/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}