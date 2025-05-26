# 音频播放器

这是一个简单的命令行WAV音频文件播放器。

## 依赖项

- ALSA (Advanced Linux Sound Architecture) library: 用于音频播放。
  在基于Debian/Ubuntu的系统上，您可以使用以下命令安装：
  ```bash
  sudo apt-get update
  sudo apt-get install libasound2-dev
  ```

## 如何编译

项目包含一个 `Makefile` 用于简化编译过程。在项目根目录下运行：

```bash
make
```

这将编译源代码并在项目根目录下生成一个名为 `audio_player` 的可执行文件。

## 如何运行

编译成功后，您可以使用以下命令运行播放器：

```bash
./audio_player <your_wav_file.wav>
```

例如：

```bash
./audio_player test_melody.wav
```

请确保提供的WAV文件是有效的16位PCM编码格式。

## 项目结构

```
.
├── Makefile                # 编译脚本
├── README.md               # 本文件
├── include/                # 头文件目录
│   ├── audio.h             # 音频播放相关函数声明
│   └── wav.h               # WAV文件处理相关函数和结构声明
├── src/                    # 源文件目录
│   ├── audio.c             # 音频播放功能实现 (ALSA)
│   ├── main.c              # 主程序入口
│   └── wav.c               # WAV文件读取和解析功能实现
└── *.wav                   # 示例WAV文件 (例如: test_melody.wav, file_example_WAV_5MG.wav)
```

## 清理

要删除编译生成的文件，可以运行：

```bash
make clean
```