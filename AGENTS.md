# QScan 仓库规则

## 项目概览

QScan 当前是一个基于 Qt6 和 C++17 的桌面演示原型，目标是证明一条完整的扫描演示闭环：模拟扫描输入、异步图像处理、实时 2D 显示、参数调节、状态展示和日志输出。

当前实现优先保证演示链路稳定、模块边界清晰、测试可运行，不以真实设备接入、复杂 3D 渲染或识别算法为第一阶段目标。

更偏项目说明、实现范围和演示流程的描述见 `README.md`，更详细的补充材料位于 `docs/README.md`。

## 主要技术栈

- Qt6 Widgets
- Qt Test
- Qt Concurrent
- CMake
- C++17

仓库文档里提到过 VTK/OpenGL 等后续扩展方向，但当前代码主线仍然是 Qt Widgets + 2D 图像链路。

## 目录结构

- `src/app`：应用控制器与共享状态模型
- `src/core`：扫描帧、处理参数等核心数据结构
- `src/pipeline`：模拟设备、图像处理器、扫描处理链路
- `src/ui`：主窗口、扫描视图、控制面板、状态面板、日志面板
- `tests`：Qt Test 测试
- `docs/brainstorms`：需求讨论与范围确认
- `docs/plans`：实施计划
- `docs/solutions`：已解决问题和沉淀经验，按类别组织，文档包含 YAML frontmatter，可通过 `module`、`tags`、`problem_type` 等字段检索

## 构建与环境约束

### Qt 和编译器

- 工程当前要求 Qt6，不再回退到 Qt5。
- 如果使用 MinGW kit，必须搭配同一套 ABI 的 MinGW 版 Qt6。
- 不要让 CMake 解析到 Conda/Anaconda 的 MSVC 版 Qt 库，否则在链接阶段会出现大量未定义符号。
- 当前仓库的 `CMakeLists.txt` 已包含 MinGW + 错误 Qt 导入库的保护逻辑。

推荐环境：

- Qt 6 MinGW kit
- CMake 3.20+
- C++17 编译器

### Bash 下的本地验证

在当前 Windows + bash 环境里，如果直接运行 build 目录下的测试，通常还需要补充 Qt 和 MinGW 运行时路径：

```bash
export PATH="/d/Qt6/6.9.0/mingw_64/bin:/d/Qt6/Tools/mingw1310_64/bin:$PATH"
```

否则测试进程可能会以 `0xc0000135` 退出。

## 常用命令

### 配置与构建

如果本地还没有可用的 build 目录，可参考：

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=D:/Qt6/6.9.0/mingw_64
cmake --build build
```

### 运行应用

```bash
./build/qscan_demo.exe
```

### 运行测试

```bash
cd /d/xcyxiner/qscan/build
export PATH="/d/Qt6/6.9.0/mingw_64/bin:/d/Qt6/Tools/mingw1310_64/bin:$PATH"
ctest --output-on-failure
```

如果只想验证与启动扫描链路直接相关的行为，可优先运行：

```bash
ctest --output-on-failure -R "qscan_(application_controller|app_state_integration)_tests"
```

## 开发约定

### 代码组织

- UI 层消费 `AppState` 暴露出的稳定状态，不要让控件直接依赖底层扫描器或处理链路内部对象。
- 新增状态或交互时，优先沿着 `ApplicationController -> AppState -> UI` 这条主链扩展，而不是在 UI 中直接拼接业务逻辑。
- `src/pipeline` 中的对象如果涉及 `QObject`、`QThread`、`QTimer`，必须明确线程归属和跨线程调用方式。

### 测试约定

- 改动行为逻辑时，优先补或改对应 Qt Test，而不是只做手工验证。
- 与 UI 按钮触发的异步链路相关的问题，优先做窄验证：控制器测试、应用状态集成测试、对应 pipeline 测试。
- 如果某个问题涉及线程、定时器或对象归属，验证时不要只看状态切换，要确认真实帧或真实结果已经流到状态层。

### 变更边界

- 第一阶段聚焦完整演示闭环，不要在同一次改动里顺手引入真实设备接入、重型 3D 流程或复杂识别算法。
- 修复问题时优先处理根因，不要只在 UI 层做表面补丁。
- 尽量保持改动集中，避免顺手重构无关模块。

## 文档与知识沉淀

`docs/solutions` 是仓库内的经验库，用来记录已经定位并解决的问题、开发约定或可复用经验。这里的文档按类别组织，并带有 frontmatter，适合在以下场景优先检索：

- 修复已有问题前，先看是否已经记录过相同或相邻问题
- 对某个模块做较大改动前，先查是否已有历史约束或踩坑记录
- 需要理解某类问题的根因、验证方式或防回归策略时

如果一次问题已经被确认修复，并且排查过程或根因对后续有复用价值，优先补一篇 `docs/solutions` 文档，而不是只留在对话或提交记录里。