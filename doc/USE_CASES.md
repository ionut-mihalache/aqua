# What can you build with AQUA?

__AQUA__ lets one process **publish a named, versioned service** and lets other processes on the **same machine** call it through **shared memory**, without sockets or HTTP in between.

> **How data moves:** __AQUA__ is not zero-copy. The payload is copied into and out of shared memory, but the copy happens entirely in **user space**, allowing the service and the client to have full ownership over the data. The data doesn't pass through the kernel as it does with sockets or pipes. The kernel is involved only under congestion, when a process has to wait or take a lock.

Because __AQUA__ is a plain C library, any language that can call C can use it: C/C++ directly, PHP through FFI, Java through JNA, and in principle Python, Rust, Go, C#, Node.js and many others.

That combination opens up a lot of possibilities. This page collects ideas to get you started.

---

## The core idea

- A **service** installs itself under a string ID and a version (for example `"xslt-transformation"`).
- A **client** connects to that ID, sends a payload and receives a response.
- Client and service can be written in **different languages** and run as separate processes.

## When to use AQUA

- The processes run **on the same machine**
- You want to **combine languages**, using the best library for each job regardless of language
- You move **large payloads** (images, documents, audio, video frames, model inputs)
- **Latency and throughput** matter more than they would for a typical network call
- You want a heavy component to live in its **own process**, isolated from your main app

Currently, it is **not** the right tool when processes run on different machines (use HTTP, gRPC or a message broker for that).

---

## Real example already in the repository

The `test/` folder contains a working scenario: a **Java service** that performs **XSLT transformations** using Saxon, called by **PHP** and **C++** clients. The XML being transformed comes from the Erasmus Without Paper (EWP) university exchange network.

This is a common real-world situation. A web application written in one language (PHP) needs a capability that is best available in another ecosystem (Saxon on the JVM). Instead of running a separate HTTP server, the PHP code calls the Java service directly through shared memory.

The same folder includes benchmarks comparing __AQUA__ with **Unix domain sockets** and **ZeroMQ** for the same workload.

---

## Ideas for things to build

### Web applications calling heavy local services

Keep your web app in the language you like, and offload heavy work to a specialised local service:

- **PDF generation**: a PHP or Python site sends HTML or data, and a service renders the PDF
- **Image processing**: resizing, thumbnails, watermarks, format conversion with a C/C++ library such as libvips or OpenCV
- **Document conversion**: XML/XSLT, Markdown → HTML, office formats
- **Search or indexing**: a fast native indexer used by a scripting-language frontend

### Games, graphics and media

- A **game engine** in C++ with **scripting or tools** in another language
- **Video or camera frames** passed from a capture process to a processing or streaming process without going through the kernel
- **Audio pipelines** where effects or analysis run as separate services
- **Live visualisation**: a simulation in C/C++ streaming data to a dashboard in another language

### Hardware, drivers and devices

- A service that owns access to **a device** (sensor, serial port, camera, GPU) and lets several local programs use it safely
- **Robotics or IoT gateways**, where a native process talks to hardware and other processes consume the data
- Scientific or lab equipment software, where acquisition and analysis run separately

### Plugins and modular desktop apps

- An application whose **plugins run as separate processes**, so a crashing plugin doesn't take down the host
- A desktop app with a **native backend** (C/C++) and a UI in another technology
- **Wrapping a legacy library** in a service, so modern code in any language can use it without linking it directly

### Polyglot microservices on one machine

- Several services on the same server or container talking to each other through __AQUA__ instead of HTTP on localhost
- A **sidecar** process that provides caching, compression or encryption to your main application

---

## Projects the community could help build

These would make __AQUA__ easier to adopt. Great choices if you want to contribute something visible:

| Project | Why it helps |
|---|---|
| **Language bindings**: Python (`ctypes`/`cffi`), Rust, Go (`cgo`), C#, Node.js | Lets more developers use __AQUA__ in their favourite language |
| **A "hello world" client + service** in C | The smallest possible example, for newcomers |
| **Demo apps** from the ideas above, such as an image-resize service or a PDF service | Shows __AQUA__ solving real problems |
| **Higher-level wrappers**: a simple `call(service, data) → response` API in each language | Hides handles and queue setup from users |
| **Benchmark reports** on different hardware and operating systems | Helps people decide whether __AQUA__ fits their workload |
| **Documentation of the DSP protocol** | Makes it possible to write bindings and tools independently |

If you build something with __AQUA__, big or small, **we'd love to hear about it!** Open an issue or a pull request to add your project to this page. You can also discuss you idea using the [___Discussions___](https://github.com/ionut-mihalache/aqua/discussions) tab on GitHub.

## Showcase

*Projects using __AQUA__ will be listed here. Yours could be the first one!*
