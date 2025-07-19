# Video Enhancement Pipeline

Video processing tools for enhancement and codec benchmarking.

## Requirements

**C++ Enhancement:**
- OpenCV 4.x
- CMake 3.16+
- C++17 compiler

**Python Benchmarking:**
- Python 3.7+
- FFmpeg

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

### Video Enhancement
```bash
./main input.mp4 output.avi
```

### Codec Benchmarking
```bash
python benchmark.py input.mp4
python benchmark.py input.mp4 --save
```

## Features

**Enhancement Pipeline:**
- Non-local means denoising
- CLAHE contrast enhancement
- Real-time processing feedback

**Benchmarking Tool:**
- Tests H.264, H.265, and AV1 codecs
- Measures encoding time and compression ratios
- JSON result export