#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

//렌더링된 이미지가 저장되는 버퍼 == 프레임 버퍼인데, 주로 화면 크기가 달라질때 -> 픽셀 크기가 조정되면서 변경됨
void framebuffer_size_callback(GLFWwindow* window, int width, int height); // 프레임버퍼의 크기가 변경될 때 호출되는 콜백 함수
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw 를 사용하기 위한 초기 설정 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // 윈도우 포인터 가져오기 -> 위의 전역변수에서 높이/너비 정하고, 이름이 LearnOpenGL 이 되는 창 열기
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 중간설정 - > 윈도우 포인터 설정해주고, 
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad - opengl 함수들에 대해서 , 운영체제에 맞게 함수를 바인딩해줌 내가 운영체제 구분없이 쓰는 함수가 -> 운영체제에 따라 달라질 수 있도록
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // 렌더링 루프 - 윈도우가 열려 있을 때 반복문을 사용하여 지속적으로 렌더링 
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        // 화면을 지울 때 사용할 색상을 설정 - RGBA - 마지막은 투명도
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // 설정된 색상으로 지움

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //  현재 그려진 프레임과 이전 프레임을 교체
        glfwSwapBuffers(window);
        // 이벤트(예: 키보드 입력, 마우스 움직임 등)를 처리
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// 사용자 입력 처리 함수
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 윈도우 사이즈가 변경될때 콜백
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}