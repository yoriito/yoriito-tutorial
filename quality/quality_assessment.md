
# Quality Assessment

## yoriito-tutorial v0.1.0
All target items passed. See below for details.

| Category | Result | Reference |
| :--- | :--- | :--- |
| DAST | Checked by Sanitizer and no errors found. | [1] |
| Performance | CPU Usage: max 100% (All cores), Memory Usage: approx. 180 MB | [2] |

<br>
[1]

**Target device**
- Dev container on WSL.

**Precondition**
- Running tutorial app with `sanitizer.mp4` in the assets folder.

<br>
[2]

**Target device**: 
- Raspberry Pi 4B
    - Broadcom:BCM2711, Quad core Cortex-A72 (ARM v8) 64-bit SoC 1.5GHz
    - Memory 4GB

**Preconditions**:
- Using Top command.
- Running the tutorial app with `performance.mp4` in the assets folder.
