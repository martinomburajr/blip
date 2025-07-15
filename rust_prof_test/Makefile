# === Paths ===
RUNTIME_DIR := ../runtime
PLUGIN_DIR := ../plugin/build
LIB_CRATE := ../adder_lib
BUILD_DIR := build_output
OUT := $(BUILD_DIR)/main

# === Files ===
LL := $(BUILD_DIR)/adder.ll
BC := $(BUILD_DIR)/instrumented_adder.bc
OBJ := $(BUILD_DIR)/instrumented_adder.o
RUNTIME_OBJ := $(BUILD_DIR)/record_func_call.o

# === Default Target ===
all: $(OUT)

# === Step 1: Emit LLVM IR from adder_lib ===
.PHONY: emit-llvm
emit-llvm:
	cd $(LIB_CRATE) && \
	RUSTFLAGS="--emit=llvm-ir -C opt-level=0 -C lto=off" cargo build

# === Step 2: Copy IR file containing adder to build_output ===
$(LL): emit-llvm
	@mkdir -p $(BUILD_DIR)
	cp `grep -l '@adder' $(LIB_CRATE)/target/debug/deps/*.ll | head -n1` $(LL)

# === Step 3: Run LLVM pass to instrument the IR ===
$(BC): $(LL)
	opt -load-pass-plugin $(PLUGIN_DIR)/MyProfiler.dylib \
	    -passes=function-profiler \
	    -o $(BC) $(LL)

# === Step 4: Compile IR to object file ===
$(OBJ): $(BC)
	llc -filetype=obj -o $(OBJ) $(BC)

# === Step 5: Compile runtime C++ to object ===
$(RUNTIME_OBJ): $(RUNTIME_DIR)/record_func_call.cpp
	@mkdir -p $(BUILD_DIR)
	clang++ -std=c++17 -c $< -o $@

# === Step 6: Build final Rust binary and link extra objects ===
$(OUT): $(OBJ) $(RUNTIME_OBJ)
	cargo rustc -- \
	    -Clink-args="$(OBJ) $(RUNTIME_OBJ) -lc++" \
	    && cp target/debug/rust_prof_test $(OUT)

# === Clean everything ===
clean:
	cargo clean
	rm -rf $(BUILD_DIR)

.PHONY: all clean emit-llvm
