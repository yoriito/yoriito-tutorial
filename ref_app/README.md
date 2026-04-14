# ref_app

## 📌 Purpose
This is a reference implementation of the vehicle-api application layer for emergency toggle button control.  
It exposes gRPC endpoints backed by DataBroker(kura), allowing clients to control various actuators.

## 🚀 How to Run
1. Backend prerequisites
    - Start ref_pal  
        Please refer to "ref_pal/README.md"

2. Launch the reference app:  
   ```bash
   cd <this dir>
   cmake -S . -B build
   cmake --build build -j
   ./build/ref_app
   ```

## 📖 Description

### ✅ Preconditions
- The Databroker must be running before starting ref_app.

### 🧩 Software Architecture

```
ref_app              : gRPC server exposing toggle state setting API  
├─ src/  
│   ├─ main.cc       : Entry point
│   └─ ref_app.cc    : Set values ​​of various actuators via VSS from toggle state  
├─ inc/  
│   └─ ref_app.h     : ref_app.cc header
└─ gen/              : Generated gRPC files (.pb.cc / .pb.h / .grpc.pb.cc / .grpc.pb.h)
```

## 📦 Dependencies
- gRPC / Protocol Buffers
