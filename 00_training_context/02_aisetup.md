# Session 2: AI Toolchain Setup

**Time:** ~25 minutes

---

## Learning Objectives

By the end of this session, you will:
- **Install and configure** VS Code with required extensions.
- **Enable GitHub Copilot** for AI-assisted coding.
- **Verify the Arduino toolchain** and verify end-to-end build/upload.
- **Set up Google Test framework** for C++ unit testing.
- **Confirm the environment is ready** for the desk automation project.

---

## Prerequisites

- Computer with Windows
- Internet connection (to download tools and extensions)
- ~15 minutes of setup time
- [Completed Session 1 (LED blink working)](01_hello-world.md)

---

## Part 1: Install and Configure VS Code

### Step 1: Download VS Code
1. Go to **Company Portal** or [code.visualstudio.com](https://code.visualstudio.com)
2. Download the installer for your OS.
3. Run the installer and accept defaults.
4. Launch VS Code.

### Step 2: Install Required Extensions

In VS Code, press `Ctrl+Shift+X` (or `Cmd+Shift+X` on macOS) to open **Extensions**. Search for and install:

| Extension | Publisher | Purpose |
|-----------|-----------|---------|
| **C/C++** | Microsoft | Syntax highlighting, IntelliSense, debugging |
| **GitHub Copilot** | GitHub | AI code generation and suggestions |
| **CMake** | Twxs | Build configuration for tests |
| **Markdown All in One** | Yu Zhang | Markdown editing and preview |

**Installation steps:**
1. Search for "C/C++" → Click **Install**.
2. Search for "GitHub Copilot" → Click **Install**.
   - You may be prompted to sign in with your GitHub account. Do so.
3. Search for "CMake" → Click **Install**.
4. Search for "Markdown All in One" → Click **Install**.

**Restart VS Code after all installations complete.**

## Part 2: Enable GitHub Copilot

### Step 1: Sign In
1. Open VS Code.
2. Click the **GitHub Copilot icon** (bottom right, or in the activity bar).
3. Click **"Sign in to use GitHub Copilot"**.
4. Authenticate with your GitHub account => **[RACFID]_deere**

### Step 2: Verify Copilot Works

1. Create a new file: `test_copilot.cpp`
2. Type a comment:
   ```cpp
   // Function to add two numbers
   int
   ```
3. Press `Ctrl+I` (or `Cmd+I` on Mac) and type:
   ```
   Write a simple C++ function to add two integers and return the sum.
   ```
4. Copilot should generate:
   ```cpp
   int add(int a, int b) {
       return a + b;
   }
   ```

If Copilot generates code, ✅ it's working. If not, check your GitHub Copilot subscription (trial or paid).

### Step 3: Delete Test File
Remove `test_copilot.cpp` (we don't need it anymore).


## Part 2: Configure Your AI Agent (Development Mode)

Use this lightweight prompt to put your AI helper into “development mode” before you start installing tools. Paste it once, then proceed with the steps.

```
You are an AI coding assistant helping me set up an embedded C++ toolchain on Windows.
- Be concise; prefer numbered steps and short commands.
- When giving commands, target PowerShell unless I specify otherwise.
- Assume Arduino + C++ + CMake + Google Test context.
- Call out when admin rights are needed.
- Always show a one-line verify command after each install (e.g., --version).
- If there are multiple options, recommend the simplest and most reliable for beginners.
- Perform the installations steps for me
```

---

## Part 3: Verify Arduino Toolchain

### Step 0: Arduino IDE

In case not yet installed, download **Arduino IDE** from **Company Portal**.

### Step 1: Open or Create a Sketch

1. Create a new file: `test_verify.ino`
2. Paste the LED blink code from Session 1:
   ```cpp
   const int LED_PIN = 13;
   const int BLINK_DELAY = 1000;

   void setup() {
     pinMode(LED_PIN, OUTPUT);
   }

   void loop() {
     digitalWrite(LED_PIN, HIGH);
     delay(BLINK_DELAY);
     digitalWrite(LED_PIN, LOW);
     delay(BLINK_DELAY);
   }
   ```

### Step 2: Verify the Sketch (Compile Only)

If compilation succeeds, the toolchain is working.

### Step 3: Create a Test Arduino Project

1. Select your board type: **Arduino UNO**.
2. Select your COM port (e.g., `COM3`, `/dev/ttyACM0`).

### Step 4: Upload and Test (Optional)

1. Connect your Arduino UNO via USB.
4. Upload code, Wait for **"Done uploading"**.
5. Your LED should blink (same as Session 1).

---

## Part 4: Setup Development and Test Auotmation Toolchain 

### Step 1: Install toolchain needed

Prompt te ai agent:
```
Setup the following tools on my windows environment:
- GCC/g++ (MSYS2)
- CMake
- Google Test
- Cppcheck
```

You can also refer to [toolain.md](toolchain.md) for manual commands.

### Step 2: Create a Test Project Structure

1. Create a new folder: `desk-automation-project`
2. Inside, create this structure:
   ```
   desk-automation-project/
   ├── 01_src/
   │   ├── Calculator.cpp
   │   └── Calculator.h
   ├── 02_tests/
   │   ├── CMakeLists.txt
   │   └── test_calculator.cpp
   ├── CMakeLists.txt
   └── README.md
   ```

### Step 3: Create CMakeLists.txt (Top Level)

Create `desk-automation-project/CMakeLists.txt`:

Prompt the agent to set it up:
```
Review this project and setup CMakeLists.txt for this project.
```

### Step 4: Create a Simple Test

Create `tests/test_calculator.cpp`:

```
#include <gtest/gtest.h>
#include "calculator.h"

TEST(CalculatorTests, SumTwoPositiveNumbers)
{
    EXPECT_EQ(sum(2, 3), 5);
}

TEST(CalculatorTests, SumPositiveAndNegative)
{
    EXPECT_EQ(sum(4, -7), -3);
}

TEST(CalculatorTests, SumZeros)
{
    EXPECT_EQ(sum(0, 0), 0);
}

```

### Step 5: Build and Run Tests

Commands (PowerShell, copy/paste):
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

```
cmake --build build
```

```
ctest --test-dir build
```


✅ If tests pass, Google Test is set up correctly.

---

## Part 5: Verify the Full Toolchain

### Checklist

Before moving to the next session, confirm:

- ✅ VS Code installed and running
- ✅ C/C++ extension installed and configured
- ✅ GitHub Copilot installed and signed in
- ✅ Arduino is OK
- ✅ Arduino sketch verifies (compiles) without errors
- ✅ Arduino sketch uploads and LED blinks
- ✅ CMake installed
- ✅ Google Test tests build and run successfully

If any step fails, revisit the documentation.

---

## Part 7: Next Steps & Reflection

### What You've Done
- ✅ Set up a professional embedded systems development environment.
- ✅ Enabled AI-assisted coding with GitHub Copilot.
- ✅ Verified end-to-end Arduino compilation and upload.
- ✅ Configured automated testing with Google Test.

### Reflection Questions
1. **Why is automated testing (Google Test) important for embedded systems?**
   - (Hint: Hardware failures can be costly and dangerous; tests catch bugs before deployment.)
2. **What happens if Copilot suggests code you don't fully understand?**
   - (Hint: Read it carefully, ask Copilot to explain, or refactor it yourself.)
3. **How would you verify a tool is installed correctly without VS Code?**
   - (Hint: Use command-line tools: `which`, `where`, version flags.)
   - or try to run it directly on terminal.

---

## Key Takeaways

- ✅ **VS Code + Copilot + Arduino IDE = a professional workflow** for embedded AI-assisted development.
- ✅ **Testing frameworks (Google Test) catch bugs early** before they become field failures.
- ✅ **Automated verification (CMake, ctest) scales** as projects grow.
- ✅ **A working toolchain is the foundation** for productive engineering.

---

## What's Next?

Next session: [Session 3 – Prompting Techniques](03_prompting-techniques.md)

---

## Additional Resources

- **VS Code Documentation:** [code.visualstudio.com/docs](https://code.visualstudio.com/docs)
- **GitHub Copilot Guide:** [github.com/features/copilot](https://github.com/features/copilot)
- **Arduino IDE Setup:** [arduino.cc/en/Guide](https://arduino.cc/en/Guide)
- **Google Test Primer:** [github.com/google/googletest](https://github.com/google/googletest)
- **CMake Documentation:** [cmake.org/documentation](https://cmake.org/documentation)


