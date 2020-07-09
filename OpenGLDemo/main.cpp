//
//  main.m
//  OpenGLDemo
//
//  Created by 首佑-石文文 on 2020/7/9.
//  Copyright © 2020 首佑-石文文. All rights reserved.
//

#include "GLTools.h" // GLTool.h头⽂件包含了⼤部分GLTool中类似C语⾔的独⽴函数
#include "GLMatrixStack.h" // 矩阵的⼯具类.可以利于GLMatrixStack 加载单元矩阵/矩阵/矩阵相乘/压栈/出栈/缩放/平移/旋转
#include "GLFrame.h" // 矩阵⼯具类,表示位置.通过设置 vOrigin, vForward ,vUp
#include "GLFrustum.h" // 矩阵⼯具类,⽤来快速设置正/透视投影矩阵.完成坐标从3D->2D映射过程
#include "GLGeometryTransform.h" // 变换管道类,⽤来快速在代码中传输视图矩阵/投影矩阵/视图投影变换矩阵等

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h> // 在Windows 和 Linux上，我们使⽤freeglut的静态库版本并且需要添加⼀个宏
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

////设置角色帧，作为相机
GLFrame             viewFrame; //用于设置观察者视图坐标
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum; // 设置图元绘制时的投影方式
GLTriangleBatch     torusBatch; // 辅助批次类，可以传输顶点/光照/纹理/颜色数据到存储着色器中
GLMatrixStack       modelViewMatix; // 模型视图矩阵
GLMatrixStack       projectionMatrix; // 投影矩阵
GLGeometryTransform transformPipeline; // 变换管道，存储模型视图/投影/模型视图投影矩阵
GLShaderManager     shaderManager; // 存储着色器管理类工具类

//标记：背面剔除、深度测试
int iCull = 0;

/// 窗口大小改变时回调
/// @param w 窗口宽度
/// @param h 窗口高度
void ChangeSize(int w,int h) {
    // 设置视口位置和大小，其中0,0代表窗口中视口的左下角坐标，w，h代表宽高像素，此处设置与窗口一样大小。
    //1.防止h变为0
    if(h == 0) h = 1;
    
   //2.设置视口窗口尺寸
   glViewport(0, 0, w, h);
   
   //3.setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
   // 设置透视模式，初始化其透视矩阵
   viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
   
   //4.把透视矩阵加载到透视矩阵对阵中
   projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
   
   //5.初始化渲染管线
   transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
    
}

//为程序作一次性的设置
void SetupRC() {
    
    //1.设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );
    
    //2.初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //3.将相机向后移动7个单元：肉眼到物体之间的距离
    viewFrame.MoveForward(7.0);
    
    //4.创建一个甜甜圈
    //void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    //参数1：GLTriangleBatch 容器帮助类
    //参数2：外边缘半径
    //参数3：内边缘半径
    //参数4、5：主半径和从半径的细分单元数量
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    //5.点的大小(方便点填充时,肉眼观察)
    glPointSize(4.0f);
    
}

//开始渲染
void RenderScene(void) {
    
    //1.清除窗口和深度缓冲区
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
     // 判断开启/关闭正背面剔除功能
     if (iCull) {
         glEnable(GL_CULL_FACE);
         // 下面两行是默认的，可以不写
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);
     } else {
         glDisable(GL_CULL_FACE);
     }
     
     //2.把摄像机矩阵压入模型矩阵中
     modelViewMatix.PushMatrix(viewFrame);
     
     //3.设置绘图颜色 红色
     GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
     
     //4.
     //使用平面着色器
     //参数1：平面着色器
     //参数2：模型视图投影矩阵
     //参数3：颜色
    // shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
     
     //使用默认光源着色器
     //通过光源、阴影效果跟提现立体效果
     //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
     //参数2：模型视图矩阵
     //参数3：投影矩阵
     //参数4：基本颜色值
     shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
     
     //5.绘制
     torusBatch.Draw();

     //6.出栈 绘制完成恢复
     modelViewMatix.PopMatrix();
     
     //7.交换缓存区
     glutSwapBuffers();
    
}

///  处理菜单
/// @param value
void ProcessMenu(int value)
{
    switch(value)
    {
        case 1:
            iCull = !iCull;
            break;
    }
    // 刷新
    glutPostRedisplay();
}

///  创建右击菜单 用于切换背面剔除，深度测试
void createMenu() {
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle cull backface", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/// 键位监听
/// @param key 键位
/// @param x
/// @param y
void SpecialKeys(int key, int x, int y)
{
    //根据键位设置，通过不同的键位对其进行设置，控制Camera的移动，从而改变视口
    
    //1.判断方向 上下沿着x轴旋转，左右沿着y轴旋转
    if(key == GLUT_KEY_UP)
        //2.根据方向调整观察者位置
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);

    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //3.重新刷新
    glutPostRedisplay();
}

int main(int argc,char* argv[]) {
    
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    
    /*
     初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区
     */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    glutInitWindowSize(800,600);
    // 窗口名称
    glutCreateWindow("Geometry Test Program");
    
    //注册回调函数 reshape在初次出现和每次改变窗口大小时触发回调
    glutReshapeFunc(ChangeSize);
    
    //显示回调函数，注册一个用于场景绘制的函数
    glutDisplayFunc(RenderScene);
    // 注册键盘特殊按监听
    glutSpecialFunc(SpecialKeys);
    
    // 创建右键菜单
    createMenu();
    
    //驱动程序的初始化
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    //调用SetupRC 初始化着色器配置
    SetupRC();
    
    glutMainLoop(); // 启动OpenGL运行循环
    return 0;
    
}

