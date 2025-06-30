# xcontainer

Welcome to **xcontainer**, a high-performance C++ library offering a suite of thread-safe, modern container classes designed for demanding applications. From lockless hash maps to concurrent queues, `xcontainer` empowers developers to build scalable, multi-threaded systems with ease. Whether you're optimizing game engines, real-time analytics, or distributed servers, `xcontainer` delivers unmatched speed and reliability.

## Why xcontainer?

In a world where concurrency and performance are king, `xcontainer` stands out:
- **Lockless & Thread-Safe**: Containers like `unordered_lockless_map` use atomic operations for blazing-fast, contention-free access.
- **Versatile Collection**: A growing suite of containers for diverse use cases, all accessible via `xcontainer.h`.
- **Modern C++**: Built with C++20 for expressive, efficient, and safe code.
- **Lightweight Footprint**: Minimal dependencies, seamless integration, and zero bloat.

## Key Features

### Comprehensive Container Suite
`xcontainer` provides a rich set of containers, each optimized for performance and concurrency. Include them all via the single header `xcontainer.h`.

- **unordered_lockless_map**: A thread-safe, lock-free hash map using atomic operations for concurrent reads and writes. Ideal for low-latency key-value storage.
  ```cpp
  #include <xcontainer.h>
  xcontainer::unordered_lockless_map<int, std::string> map;
  map.Insert(1, [](std::string& v) { v = "Hello, xcontainer!"; });
  map.FindAsWriteOrDelete(1, [](std::string& v) {
      v += " Rocks!";
      return v.length() > 20; // Delete if too long
  });
  ```
- **lockless_queue** (Planned): A high-throughput, thread-safe queue for producer-consumer scenarios.
- **concurrent_vector** (Planned): A dynamic array with fine-grained concurrency for parallel access.
- **More to Come**: Expect additional containers tailored for real-time and concurrent applications.

### Core Strengths
- **Atomic-Based Concurrency**: Eliminates traditional locks for maximum throughput.
- **Dynamic Resizing**: Containers adapt to growing workloads with minimal overhead.
- **Flexible APIs**: Support for read, write, and conditional delete operations with customizable callbacks.
- **Cache-Friendly**: Optimized data structures for modern CPU architectures.

## Getting Started

### Prerequisites
- C++20 compatible compiler (e.g., GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.15 or higher

### Installation
Clone the repository and build the library:

```bash
git clone https://github.com/LIONant-depot/xcontainer.git
cd xcontainer
mkdir build && cd build
cmake ..
make
```

Integrate `xcontainer` into your CMake project:

```cmake
find_package(xcontainer REQUIRED)
target_link_libraries(your_target PRIVATE xcontainer::xcontainer)
```

### Usage
Include `xcontainer.h` to access the full suite of containers:

```cpp
#include <xcontainer.h>
#include <iostream>

int main() {
    xcontainer::unordered_lockless_map<int, int> map;
    map.Insert(42, [](int& v) { v = 100; });

    map.FindAsReadOnly(42, [](const int& v) {
        std::cout << "Value: " << v << std::endl; // Outputs: Value: 100
    });

    return 0;
}
```

## Performance
`xcontainer` is engineered for speed. Our `unordered_lockless_map` delivers up to 2x higher throughput than `std::unordered_map` with mutexes in high-concurrency scenarios, thanks to its lockless design. Other containers are similarly optimized for low-latency, multi-threaded access (performance varies by workload).

Run our benchmark suite to see the results:

```bash
cd build
./benchmarks/xcontainer_bench
```

## Contributing
We’re excited to grow `xcontainer` with your help! Whether it’s adding new containers, optimizing performance, or fixing bugs, your contributions are welcome.

1. Fork the repo.
2. Create a feature branch (`git checkout -b feature/new-container`).
3. Commit your changes (`git commit -m "Add new container"`).
4. Push to the branch (`git push origin feature/new-container`).
5. Open a Pull Request.

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License
`xcontainer` is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Acknowledgments
- Built for developers pushing the limits of concurrency and performance.
- Gratitude to the C++ community for inspiring modern, efficient libraries.

## Contact
Questions or ideas? Open an issue on [GitHub](https://github.com/LIONant-depot/xcontainer/issues) or follow us on [Twitter](https://twitter.com/LIONantDepot).

**xcontainer**: Your toolkit for high-performance, concurrent C++. Star the repo and build something extraordinary!

---
