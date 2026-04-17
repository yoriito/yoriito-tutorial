# ref_ui
## 📌 Purpose
A Flutter Yoriito Vehicle API demo UI.  
Connects to ref_app (gRPC server) to set toggle state.

## 🚀 How to Run
1. Backend prerequisites
    - Start ref_pal  
        Please refer to "ref_pal/README.md"  

    - Start ref_app
        ```bash
        cd <build-dir>
        ./ref_app
        ```

2. Flutter setup & launch
    ```bash
    cd <this repo>
    flutter clean
    flutter pub get
    flutter run -d linux
    ```

## 📖 Description
- UI toggles ON/OFF
- When the toggle state is ON:  
  - the car lights shall turn on
  - the car doors shall close  
- When the toggle state is OFF:  
  - the car lights shall turn off  
  - the doors shall open
- Toggle state unavailability  
  - If all light and door signals are unavailable, the toggle state shall be unavailable.

## ✅ Preconditions
- ref_app is reachable (default: 127.0.0.1:50055)  
  - Set the host and port of ref_ui as follows:
    lib/services/emergency_rpc.dart  
    - static const String _host = '127.0.0.1';  
    - static const int _port = 50055;

## 🧩 Software Architecture
```
lib/
├─ main.dart                      : runApp(MyApp)
├─ app.dart                       : MaterialApp / screen scaling (414x896 baseline)
├─ pages/
│   ├─ splash_page.dart           : Logo fade-in → navigate to demo
│   └─ emergency_demo_page.dart   : Main demo (state, RPC calls, subscription)
├─ services/
│   └─ emergency_rpc.dart         : gRPC helper
│       - setToggleState(is_on)
└─ gen/                           : Generated gRPC files (.pb.dart / .pbgrpc.dart / .pbenum.dart)
```
