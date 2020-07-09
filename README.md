# 说明
`OpenGL` 正背面剔除案例
代码中有详细注释，具体可看[（四）OpenGL正背面剔除](https://www.yuque.com/shiwenwen-qfo44/nrzz49/mflvgp)

# （四）OpenGL正背面剔除

## 渲染过程中可能存在的问题
当我们使用光源着色器在绘制一个3D场景时，默认背对着光源部分应该是黑色不可见的，如果我们对绘制的图形进行旋转等矩阵变换，那么OpenGL就不会到哪些是背光面了，就会出现渲染问题。
![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594274303802-7cd3a83c-bf49-49bc-90ef-23b561b58056.png#align=left&display=inline&height=683&margin=%5Bobject%20Object%5D&name=image.png&originHeight=683&originWidth=1498&size=130641&status=done&style=none&width=1498)所以在绘制3D场景的时候，我们需要决定哪些部分是对观察者课件的，哪些部分是对观察者不可见的。对于不可见的部分，应该及早丢弃掉，不进行渲染。例如在一个不透明的墙壁后的图形，就不应该渲染，这种情况叫做“隐藏面消除”(Hidden surface elimination)。
## 解决方案
### 油画(画家)算法

- 油画算法是渲染中常用的一种算法，其核心就是绘制场景时，按照场景中的物体或者图层远近顺序（相对观察者），由远到近的进行绘制，先绘制较远的物体，再绘制较近的物体。

如下图示例：先绘制红色的部分，在绘制黄色的部分，最后再绘制最上层的灰色部分，即可解决隐藏面消除的问题。
![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594274764700-d465cffe-0548-4af4-8a34-74ff9b4106c7.png#align=left&display=inline&height=199&margin=%5Bobject%20Object%5D&name=image.png&originHeight=199&originWidth=681&size=10260&status=done&style=none&width=681)

- 油画算法的弊端：

如果我们的物体图层发生叠加，那么这时候油画算法将无法处理，画家这时候就懵了，我到底应该先绘制哪个物体？
![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594275036743-e1cc8abd-77ec-4bb3-834b-8aa34bea88d8.png#align=left&display=inline&height=318&margin=%5Bobject%20Object%5D&name=image.png&originHeight=318&originWidth=390&size=26540&status=done&style=none&width=390)

所以油画算法并不能很好的解决问题。
### 正背面剔除（Face Culling）
> 思考:
> 一个3d的立方体图形，我们从一个方向去观察，最多可以观察到几个面？
> 答案是最多3面，你可以想象一下，无论你从什么角度和位置，你最多只能看到3个面。
> 那么，我们看不到的那3个面，我们还需要去绘制吗？如果我们能以某种方式丢弃这部分数据，不去渲染看不到这部分，那么OpenGL的渲染性能即可提高超过50%。

任何平面就像一张纸一样都有两个面：正面和背面。那么这意味着我们同一时刻只能看到一个面。
在OpenGL中，可以通过 **一个面的顶点数据顺序** 检查所有正面朝向观察者的面，并渲染他们，从而丢弃背面朝向的面，不去渲染，这样可以节约片元着色器的性能。
#### 正背面区分
在OpenGL中，默认的正背面规则是：
![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594277701978-02206168-0840-48ab-ae72-5adfb7776869.png#align=left&display=inline&height=536&margin=%5Bobject%20Object%5D&name=image.png&originHeight=536&originWidth=1112&size=60347&status=done&style=none&width=1112)

- 正⾯：按照逆时针顶点连接顺序的三⻆形⾯
- 背⾯：按照顺时针顶点连接顺序的三角形⾯

当然这个规则我们是可以修改的，不过并不建议去修改。


立方体中正背面分析：
![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594277802473-37beb9f2-557c-4405-bbf5-8f935af1ee5d.png#align=left&display=inline&height=425&margin=%5Bobject%20Object%5D&name=image.png&originHeight=425&originWidth=777&size=94655&status=done&style=none&width=777)

- 左侧三⻆形顶点顺序为: `1—> 2—> 3` ; 右侧三⻆形的顶点顺序为: `1—> 2—> 3` 。
- 当观察者在右侧时,则右边的三⻆形⽅向为逆时针⽅向则为正⾯,⽽左侧的三⻆形为顺时针则为背⾯。
- 当观察者在左侧时,则左边的三⻆形为逆时针⽅向判定为正⾯,⽽右侧的三⻆形为顺时针判定为背⾯。

**正⾯和背⾯是有三角形的顶点定义顺序和观察者方向共同决定的。若观察者的观察⽅向发生改变，正⾯和背面也会发生相应的改变。** 


#### OpenGL中的剔除
`GLTools` 中给提供了操作OpenGL正背面剔除的API：
```cpp
/**
开启表面剔除(默认背面剔除)
*/
glEnable(GL_CULL_FACE);

/**
关闭表面剔除(默认背面剔除)
*/
glDisable(GL_CULL_FACE);

/**
选择剔除那个面(正面/背面)
mode参数为: 
	GL_FRONT：正面, 
    GL_BACK：背面, 
    GL_FRONT_AND_BACK：正背面，
默认GL_BACK
*/
glCullFace(GLenum mode);

/**
用户指定绕序那个为正面
mode参数为: 
	GL_CW：顺时针, 
    GL_CCW：逆时钟,
默认值:GL_CCW
*/
glFrontFace(GL enum mode);

//剔除正面实现1： 背面 + 顺时针
glCullFace(GL_BACK);
glFrontFace(GL_CW); 

//剔除正面实现2：正面 + 逆时钟
glCullFace(GL_FRONT);
glFrontFace(GL_CCW);
```
#### 缺陷
我们使用背面剔除之后，上面的案例旋转之后：

![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594278912123-3a8d4055-7ee3-4ab2-bab8-3bb12d17669d.png#align=left&display=inline&height=222&margin=%5Bobject%20Object%5D&name=image.png&originHeight=443&originWidth=417&size=41879&status=done&style=none&width=209)

可以看出好像没有什么问题了，当我们继续旋转：

![image.png](https://cdn.nlark.com/yuque/0/2020/png/674886/1594278971765-7f056742-ea3f-40c7-a17f-cd8324644af3.png#align=left&display=inline&height=224&margin=%5Bobject%20Object%5D&name=image.png&originHeight=447&originWidth=384&size=24751&status=done&style=none&width=192)

又产生了新的问题，所以背面剔除也是存在缺陷的。
这是因为如果前后两个点都是正面或者背面，这时候OpenGL是无法区分哪个面在前，哪个面在后的，就出现了上图这样的问。我们将在下一篇 **深度测试** 中讨论如何解决这样的问题。
