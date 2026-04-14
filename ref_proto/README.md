# ref_proto

## 📌 Purpose
This project generates gRPC client/server code for C++ and Dart from .proto files.   
The generated output includes Protocol Buffers message types and gRPC service stubs, which are used by the application implementation.

## 🚀 How to Run
1. generates gRPC client/server code:  
   ```bash
   cd <this-dir>
   ./convert_grpc.sh
   ```

## 📖 Description

### ✅ Preconditions
- The following directories exist.
  - /work/ref_app  
  - /work/ref_ui

### 🧩 Software Architecture

```
ref_proto                       : gRPC proto file definition  
├─ convert_grpc.sh              : gRPC code generater  
├─ emergency_availability.proto : The toggle availability polling definition  
└─ emergency_toggle.proto       : The toggle state setting definition
```

## 📦 Dependencies
- gRPC / Protocol Buffers
