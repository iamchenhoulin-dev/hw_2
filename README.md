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