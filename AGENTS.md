# AGENTS.md

## 项目概览

Argon2PoW 是使用 C++17、C99、Qt 6 Widgets 和 CMake 构建的本地桌面项目，提供两个独立程序：出题端 `Argon2PoW-Challenge` 和解答端 `Argon2PoW-Solver`。功能、使用方法和构建入口见 [README.md](README.md)。

## 代码位置

- `src/common/pow.*`：挑战码解析、Argon2 哈希、解锁码校验和有序并行搜索。
- `src/common/ui.*`、`src/common/i18n.*`：公共界面、启动参数和国际化。
- `src/generator/`：出题端界面；`src/solver/`：解答端界面与后台工作线程。
- `tests/`：算法、界面和翻译测试，测试目标定义在 [CMakeLists.txt](CMakeLists.txt)。
- `translations/argon2pow_en.ts`：英文翻译源文件，源码语言为简体中文。
- `scripts/`：构建、翻译更新、打包与独立包验证脚本。
- `third_party/argon2/`：随项目提供的第三方实现，保留其源码、许可和来源声明。

## 实现约定

- 保持 Qt 原生 Widgets 界面，与现有控件、布局和平台样式一致。
- Argon2 计算和搜索在后台执行；界面更新遵守 Qt 线程归属，处理好停止操作和输入变化后过期结果的失效。
- 保持协议兼容：挑战码为 `CHALLENGE:<内存 KiB>:<时间成本>:<盐>:<难度>`；使用 Argon2id v19、并行度 1、32 字节输出，盐按原始字节传入。
- 哈希前 4 字节按大端解释，通过条件为严格小于 `round(2^32 / 难度)`。阈值需能表示 `2^32`，修改时保留现有边界行为。
- 搜索序号从 1 开始；并行搜索仍返回最小的有效序号，保持有序批次的语义。
- 内存成本在协议和核心中以 KiB 表示，界面换算必须保留精度；多线程内存估算需考虑每个线程独立占用一份 Argon2 内存。
- 新增或修改界面文案时使用现有 Qt 翻译机制，并同步英文翻译、占位符和无障碍文案；协议内容不随语言变化。
- 第一方源码沿用 `GPL-3.0-only` 许可标记；第三方源码保留原许可。

## 构建与验证

要求 CMake 3.21+、Qt 6.5+（Widgets、LinguistTools）和匹配的编译器。Windows 推荐通过项目脚本构建并运行测试：

```powershell
.\scripts\build.ps1
```

脚本默认使用 `C:\Qt\6.12.0\mingw_64`、`C:\Qt\Tools\mingw1310_64` 和 `C:\Qt\Tools\Ninja`，可通过 `-QtDir`、`-MingwDir`、`-NinjaDir` 和 `-BuildDir` 指定本机位置。复用构建目录前确认其生成器和编译器匹配。

已有构建目录可运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

测试进程需要能找到 Qt 和编译器运行库；构建脚本会设置所需 PATH。`pow_tests` 验证算法与搜索，`ui_tests_zh`、`ui_tests_en` 验证两种语言下的交互与布局，`i18n_tests` 验证翻译。界面和翻译测试由 CTest 配置为使用 offscreen 平台。

修改翻译时运行 `.\scripts\update-translations.ps1` 并补全英文条目。涉及发布包时运行 `.\scripts\deploy.ps1`，再运行 `.\scripts\test-packages.ps1` 验证独立运行。

只修改文档时检查文档位置、相对链接及差异即可。完成工作后说明实际执行的验证和未能执行的检查。

## 文档与技能

创建、编辑、移动、审查文档或提交变更前，必须读取并遵循本项目的 [docs-placement 技能](.agents/skills/docs-placement/SKILL.md)。

- `.agents/`：开发计划、调查记录、验证报告、待办和交接等阶段性工作文档。
- `docs/dev/`：供维护者长期使用的架构、设计、契约和运维说明。
- `docs/user/`：面向最终用户的指南、帮助及配套资源；用户截图放在 `docs/user/images/`。
- 根目录 `README.md`、`README.zh-CN.md`、`CHANGELOG.md`、`LICENSE` 和代理指令文件保留惯例位置。`docs/adr/`、`docs/vendor/`、第三方源码及其许可/来源说明、生成文件和 `.agents/skills/` 按技能中的例外处理。

不要在 `docs/` 根目录新增零散 Markdown。移动文档或资源时同步更新所有引用，使用相对最终位置可解析的链接。

技能复制自 EncoreHub，其中的 Gateway/OpenAPI、`engine/vendor/`、当前目录示例和 pnpm 检查命令是来源项目的背景。本项目第三方目录是 `third_party/`；当前没有 `package.json`、`pnpm test:docs`、`pnpm test:contracts` 或 `CHANGELOG.md`。文档检查使用实际文件清单及本地链接校验，无需为套用来源项目流程而引入这些工具或文件。

提交前检查 `git status --short` 和 `git diff --cached --name-status`，确认文档分类、引用和提交范围。`build*`、`cmake-build-*`、`dist*` 以及 IDE 缓存目录中的构建产物不作为人工维护文档归档。
