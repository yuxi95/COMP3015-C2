初始化：

创建法线贴图：normal_texutre ,
创建投影贴图: proj_texture,

生成 FBO ( frame buffer object)，设置 FBO 仅写入 depth， 阴影仅需深度信息 
生成 depth-texture ， 使用 float 格式满足阴影计算所需精度
绑定 depth-texture 到 FBO 的depth-attachment
使用 std::vector保存场景中会产生阴影的物体

帧开始：

游戏逻辑：

玩家控制海豚（使用方向键），接触到场景中的方块物体就得分，分数和方块重置的剩余时间有关（100+ 剩余时间*100），总分数打印在窗口标题，每隔一定时间（2~3s),方块重置到随机位置，被玩家触碰到也会立即移动。

激活 FBO，绘制深度图
遍历场景物体vecotr，使用 depth.vert/frag shader 从灯光视角绘制场景，得到灯光方向的深度图

取消 FBO，绘制最终场景
遍历场景物体vecotr，使用 shadowmap.vert/frag shader 从正常视角绘制场景, 使用上一步得到的深度图结合shadow-map算法生成阴影， 使用normal_texutre处理法线贴图， 同时使用灯光矩阵投影贴图proj_texture

阴影图 viewprot
深度图的生成本身属于render-to-texture技术，我们可以设置一个128x128的viewport，然后绘制一个深度图的screen-quad，用来查看这个texture，

输入处理：
按数字键8开关shadow-map渲染管线，四个方向键控制海豚