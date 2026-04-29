# EDA SQL Helper

A C++ & Python based EDA (Electronic Design Automation) standard cell library SQL query assistant with integrated visualization and AI-powered natural language interface.

## Features
- **Standard Cell Library Query**: Direct SQL query interface for Sky130 and other standard cell libraries (supporting cell name, function type, drive strength, power, area, and delay filtering)
- **Visualization**: Built-in power, delay, and area comparison charts for query results
- **AI-Powered NL2SQL**: Fine-tuned Qwen2-1.5B model for converting natural language queries directly into executable SQL statements
- **Client-Server Architecture**: High-performance C++ backend with optional vLLM model serving integration

## Tech Stack
- **Backend**: C++17, SQLite3, Qt5
- **AI/ML**: Python 3.x, PyTorch, Transformers, PEFT (LoRA), vLLM
- **Build System**: CMake

## Getting Started

### Prerequisites
- CMake 3.14+
- GCC 11+ or Clang 14+
- Qt5 development libraries
- Python 3.9+ (for fine-tuning and vLLM serving)

### Building
```bash
# Clone the repository
git clone https://github.com/xing-zouing/EDA_SQL_Helper.git
cd EDA_SQL_Helper

# Build the C++ project
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running
```bash
# From the build directory
./EDA_SQL_Helper
```

## Project Structure
```
EDA_SQL_Helper/
├── src/             # C++ source files
├── include/         # Header files
├── scripts/         # Python fine-tuning scripts
├── data/            # Standard cell library datasets
└── CMakeLists.txt   # Build configuration
```

## License
MIT License
