# 第二次培训作业

本项目使用 C++17、OpenCV、Eigen、Ceres 和 CMake 完成图像处理、参数拟合与目标跟踪任务。

## 环境依赖

- Ubuntu 22.04
- GCC 11.4
- CMake 3.22
- OpenCV 4
- Eigen 3
- Ceres Solver 2

## 构建与运行

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build -j4
./build/task1
```

输入图片：

```text
resources/test_image.jpg
```

任务1结果：

```text
result/task1/
```

## 任务1：OpenCV图片处理

### 1. 颜色空间转换

程序将原始BGR图像转换为灰度图和HSV图像，并将HSV图像拆分为H、S、V三个单通道图。

OpenCV采用BGR通道顺序。HSV中H表示色相，S表示饱和度，V表示明度。

输出：

- `result/task1/gray.png`
- `result/task1/hsv_h.png`
- `result/task1/hsv_s.png`
- `result/task1/hsv_v.png`

### 2. 滤波对比

使用的参数：

| 滤波方法 | 参数 |
|---|---|
| 均值滤波 | 5×5核 |
| 高斯滤波 | 5×5核，sigmaX=1.5 |
| 中值滤波 | 核尺寸5 |

均值滤波对邻域像素直接求平均，平滑效果明显，但花瓣边缘较模糊。高斯滤波给予中心像素更高权重，结果相对自然。中值滤波取邻域中位数，较能保留边缘，适合处理椒盐噪声。

输出：

- `result/task1/mean_filter.png`
- `result/task1/gaussian_filter.png`
- `result/task1/median_filter.png`

### 3. HSV红色提取

使用两个色相范围提取红色：

```text
H: 0～10，S: 100～255，V: 100～255
H: 170～179，S: 100～255，V: 100～255
```

红色横跨HSV色相区间的首尾，因此需要使用两个范围并合并。红色花瓣大部分能够被提取；黄色边缘因色相不同通常被排除；较暗阴影可能因V值低于阈值而丢失。

输出：

- `result/task1/red_mask.png`

### 4. 形态学与轮廓

形态学操作使用5×5椭圆形结构元素：

- 腐蚀：缩小白色区域。
- 膨胀：扩大白色区域。
- 开运算：去除小型白色噪点。
- 闭运算：填补小型黑色孔洞。

后续选择开运算结果提取轮廓，因为其细小噪点和狭长干扰较少。

轮廓参数：

| 参数 | 数值 |
|---|---|
| 轮廓层级 | `RETR_EXTERNAL` |
| 轮廓压缩 | `CHAIN_APPROX_SIMPLE` |
| 最小面积 | 500像素² |

筛选后的轮廓面积已经标注在 `contours_boxes.png` 中。框选对象为红色连通区域，不要求每个矩形对应一朵完整花。

输出：

- `result/task1/erode.png`
- `result/task1/dilate.png`
- `result/task1/open.png`
- `result/task1/close.png`
- `result/task1/contours_boxes.png`

### 5. 绘制与几何变换

在原图副本上绘制了圆、矩形和文字。

旋转参数：

- 旋转中心：图像中心
- 旋转角度：逆时针35°
- 缩放比例：1.0
- 插值方法：双线性插值

旋转结果保持原始画布大小，因此四角可能出现黑色区域。

裁剪区域从坐标 `(0, 0)` 开始，宽度和高度均为原图的一半，因此裁剪面积为原图的四分之一。

输出：

- `result/task1/drawing.png`
- `result/task1/rotated_35deg.png`
- `result/task1/crop_top_left.png`



## Task 2：青色目标旋转运动分析

### 1. 任务目标

读取 `resources/task_2.mp4`，识别视频中的青色目标，提取其运动轨迹，并分析目标随时间变化的旋转角度。最后使用线性模型拟合角度与时间的关系，求出目标的角速度和初始角度。

---

### 2. 基本原理

#### 2.1 青色目标检测

首先将图像从 BGR 色彩空间转换到 HSV 色彩空间。

HSV 分别表示：

- `H`：色相，用于区分颜色
- `S`：饱和度，表示颜色的鲜艳程度
- `V`：亮度，表示颜色的明暗程度

然后设置青色对应的 HSV 范围，通过 `inRange()` 得到二值图像：

- 白色区域：可能是青色目标
- 黑色区域：背景

#### 2.2 形态学处理

由于二值图像中可能存在噪点和空洞，因此使用开运算和闭运算进行处理。

- 开运算：先腐蚀，再膨胀，用于去除小噪点
- 闭运算：先膨胀，再腐蚀，用于填补目标内部的小空洞

#### 2.3 目标中心提取

在处理后的二值图像中寻找目标轮廓，并选取面积最大的轮廓作为目标。

通过图像矩计算目标中心：

\[
x_c=\frac{m_{10}}{m_{00}}
\]

\[
y_c=\frac{m_{01}}{m_{00}}
\]

其中 \((x_c,y_c)\) 为目标中心坐标。

#### 2.4 旋转角度计算

设旋转中心为 \((x_0,y_0)\)，目标中心为 \((x,y)\)，则目标相对于旋转中心的角度为：

\[
\theta=\operatorname{atan2}(y-y_0,x-x_0)
\]

`atan2()` 的结果位于 \([-\pi,\pi]\) 范围内。为了避免角度从 \(\pi\) 突然跳到 \(-\pi\)，程序还需要进行角度展开，使角度随时间连续变化。

#### 2.5 线性拟合

如果目标近似匀速旋转，则角度与时间满足：

\[
\theta(t)=\omega t+\theta_0
\]

其中：

- \(\theta(t)\)：时刻 \(t\) 的旋转角度
- \(\omega\)：目标的角速度
- \(\theta_0\)：目标的初始角度

程序使用最小二乘法求出最佳的 \(\omega\) 和 \(\theta_0\)。

---

### 3. 项目文件

Task2 的主要代码位于 `task2/` 文件夹中。

```text
task2/
├── main.cpp
├── cyan_detector.cpp
├── cyan_detector.hpp
├── motion_math.cpp
├── motion_math.hpp
├── linear_fit.cpp
└── annotate_video.cpp
```

各文件的作用：

- `main.cpp`：读取视频并完成目标检测与运动数据提取
- `cyan_detector.cpp/.hpp`：实现青色目标检测
- `motion_math.cpp/.hpp`：完成角度计算和运动学处理
- `linear_fit.cpp`：拟合角度与时间的线性关系
- `annotate_video.cpp`：生成带目标检测标注的视频

---

### 4. 编译方法

在项目根目录 `hw_2/` 中执行：

```bash
cmake -S . -B build
cmake --build build -j4
```

命令含义：

- `cmake -S . -B build`：读取当前目录的 `CMakeLists.txt`，并在 `build/` 中生成构建文件
- `cmake --build build -j4`：使用 4 个并行任务编译项目

---

### 5. 运行方法

#### 5.1 提取目标运动数据

```bash
./build/task2
```

该程序读取视频、检测青色目标，并生成目标的位置和角度数据。

#### 5.2 进行线性拟合

```bash
./build/task2_fit
```

该程序读取目标运动数据，拟合角度和时间的关系，并计算角速度、初始角度和拟合误差。

#### 5.3 生成标注视频

```bash
./build/task2_annotate
```

该程序生成带有目标位置、轨迹等信息的标注视频。

---

### 6. 输出结果

Task2 的结果保存在：

```text
result/task2/
```

主要输出文件如下：

```text
result/task2/
├── track.csv
├── fit.csv
├── parameters.txt
├── fit_curve.png
├── error_curve.png
└── annotated.mp4
```

各文件含义：

- `track.csv`：每一帧对应的时间、目标中心坐标和旋转角度
- `fit.csv`：原始角度、拟合角度和拟合误差
- `parameters.txt`：角速度、初始角度及误差指标
- `fit_curve.png`：实际角度与拟合直线的对比图
- `error_curve.png`：拟合误差随时间变化的曲线
- `annotated.mp4`：带有目标检测和运动轨迹标注的视频

---

### 7. 拟合结果

最终得到的旋转模型为：

\[
\theta(t)=1.34775t+0.632815
\]

拟合参数：

| 参数 | 数值 | 含义 |
|---|---:|---|
| \(\omega\) | `1.34775 rad/s` | 目标角速度 |
| \(\theta_0\) | `0.632815 rad` | 初始角度 |
| SSE | `77.749` | 误差平方和 |
| RMSE | `0.232363 rad` | 均方根误差 |

结果表明，目标的旋转角度与时间近似呈线性关系，因此可以认为目标在视频中近似做匀速圆周运动。