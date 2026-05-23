---
title: 开始按钮无响应，原因是模拟扫描器定时器在线程归属错误的上下文中启动
date: 2026-05-23
track: bug
problem_type: integration_issue
module: scan-pipeline
component: MockScannerDevice
tags:
  - qt
  - qthread
  - qtimer
  - signal-slot
  - start-button
  - thread-affinity
branch: feat/qscan-demo-prototype
---

## 问题

在 QScan 演示原型中，点击开始按钮之后，界面状态会变化，但扫描帧并没有稳定开始生成。应用看起来像是进入了启动流程却卡住了，所以即使按钮的信号链路已经连通，演示效果仍然表现为“点了没反应”。

## 现象

- 点击开始按钮后，没有可见的扫描画面输出。
- 流状态可能停留在类似“正在启动”的阶段，无法进入稳定的实时流状态。
- 在验证过程中，Qt 输出了与定时器和线程相关的告警，包括 `QObject::startTimer: Timers can only be used with threads started with QThread` 和 `QObject::startTimer: Timers cannot be started from another thread`。

## 没有解决问题的排查方向

- 只检查 UI 连线并不够。控制面板中的开始按钮本来就已经通过主窗口连接到应用控制器，所以这个表面现象并不是信号槽漏连导致的。
- 只看控制器状态变化会产生误导。控制器确实能把状态推进到 `Starting`，但这并不能证明模拟扫描器的定时器已经在 worker 线程里真正运行起来。
- 一开始就跑全量测试会引入噪声，因为仓库里还有和本问题无关的失败项。对这次问题最有效的验证切片，是直接绑定“开始扫描链路”的控制器测试和应用状态集成测试。

## 解决方案

这次修复主要做了两个改动，用来稳定启动链路。

第一，在扫描管线中，显式把 worker 持有的模拟设备对象移动到 worker 线程，再启动线程处理工作。

```cpp
auto *typedWorker = static_cast<PipelineWorker *>(m_worker);
typedWorker->moveOwnedObjectsToThread(m_workerThread);
```

这项改动位于 src/pipeline/ScanPipeline.cpp。

第二，模拟扫描器改为在其所属线程中惰性创建 `QTimer`，并在 `start()`、`stop()` 从其他线程被调用时，把执行切回对象自身所属线程。

```cpp
if (QThread::currentThread() != thread()) {
    bool started = false;
    QMetaObject::invokeMethod(this, [this, &started]() {
        started = start();
    }, Qt::BlockingQueuedConnection);
    return started;
}

if (m_timer == nullptr) {
    m_timer = new QTimer(this);
    m_timer->setInterval(kFrameIntervalMs);
    connect(m_timer, &QTimer::timeout, this, &MockScannerDevice::generateFrame);
}
```

这项改动位于 src/pipeline/MockScannerDevice.cpp。

## 原因分析

根因是 Qt 对象的线程归属问题。`MockScannerDevice` 虽然被用于扫描管线的 worker 流程中，但它的定时器生命周期仍然可能在错误的线程上下文里被创建或启动。在 Qt 中，定时器必须由拥有它、并且具备事件循环的线程来启动。如果定时器是在错误线程里创建或启动的，那么即使上层状态迁移看起来成功，扫描帧生成也不会真正开始。

把设备对象移动到 worker 线程之后，设备和扫描处理工作就处在同一个线程归属下；再把定时器改成在该线程中惰性创建，就能避免定时器继承错误的线程亲和性。与此同时，通过 `QMetaObject::invokeMethod(..., Qt::BlockingQueuedConnection)` 把 `start()` 和 `stop()` 调回对象所属线程执行，可以保证即使调用方来自其他线程，设备行为仍然是稳定的。

## 预防建议

- 当 `QObject` 持有 `QTimer` 时，应当在对象所属线程中创建并启动该定时器。
- 对于运行在 worker 线程中的服务对象，应当尽早把线程归属表达清楚，而不是依赖隐含的使用方式。
- 如果某个 `QObject` 方法可能被跨线程调用，要么明确限制调用约定，要么在方法内部把执行切回对象所属线程。
- 对于由 UI 触发的异步问题，优先做窄范围验证。对这次修复来说，最有价值的检查是：

```bash
cd /d/xcyxiner/qscan/build
export PATH="/d/Qt6/6.9.0/mingw_64/bin:/d/Qt6/Tools/mingw1310_64/bin:$PATH"
ctest --output-on-failure -R "qscan_(application_controller|app_state_integration)_tests"
```

这些测试在修复后已经通过，验证了点击开始按钮后，应用可以进入实时流状态，并且扫描帧能够到达应用状态层。