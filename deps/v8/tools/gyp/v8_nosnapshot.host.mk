# This file is generated by gyp; do not edit.

TOOLSET := host
TARGET := v8_nosnapshot
DEFS_Debug := '-DENABLE_LOGGING_AND_PROFILING' \
	'-DENABLE_DEBUGGER_SUPPORT' \
	'-DENABLE_VMSTATE_TRACKING' \
	'-DV8_FAST_TLS' \
	'-DV8_TARGET_ARCH_IA32' \
	'-DENABLE_DISASSEMBLER' \
	'-DDEBUG' \
	'-D_DEBUG' \
	'-DV8_ENABLE_CHECKS' \
	'-DOBJECT_PRINT'

# Flags passed to both C and C++ files.
CFLAGS_Debug := -Wall \
	-pthread \
	-fno-rtti \
	-fno-exceptions \
	-m32 \
	-ansi \
	-fvisibility=hidden \
	-g \
	-O0

# Flags passed to only C (and not C++) files.
CFLAGS_C_Debug := 

# Flags passed to only C++ (and not C) files.
CFLAGS_CC_Debug := 

INCS_Debug := -Isrc

DEFS_Release := '-DENABLE_LOGGING_AND_PROFILING' \
	'-DENABLE_DEBUGGER_SUPPORT' \
	'-DENABLE_VMSTATE_TRACKING' \
	'-DV8_FAST_TLS' \
	'-DV8_TARGET_ARCH_IA32'

# Flags passed to both C and C++ files.
CFLAGS_Release := -Wall \
	-pthread \
	-fno-rtti \
	-fno-exceptions \
	-m32 \
	-ansi \
	-fvisibility=hidden \
	-O3 \
	-fomit-frame-pointer \
	-fdata-sections \
	-ffunction-sections \
	-fomit-frame-pointer \
	-O3

# Flags passed to only C (and not C++) files.
CFLAGS_C_Release := 

# Flags passed to only C++ (and not C) files.
CFLAGS_CC_Release := 

INCS_Release := -Isrc

OBJS := $(obj).host/$(TARGET)/gen/libraries.o \
	$(obj).host/$(TARGET)/gen/experimental-libraries.o \
	$(obj).host/$(TARGET)/src/snapshot-empty.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(obj).host/tools/gyp/js2c.stamp

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE)) $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE)) $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := -pthread \
	-m32

LDFLAGS_Release := -pthread \
	-m32

LIBS := 

$(obj).host/tools/gyp/libv8_nosnapshot.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).host/tools/gyp/libv8_nosnapshot.a: LIBS := $(LIBS)
$(obj).host/tools/gyp/libv8_nosnapshot.a: TOOLSET := $(TOOLSET)
$(obj).host/tools/gyp/libv8_nosnapshot.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink)

all_deps += $(obj).host/tools/gyp/libv8_nosnapshot.a
# Add target alias
.PHONY: v8_nosnapshot
v8_nosnapshot: $(obj).host/tools/gyp/libv8_nosnapshot.a

# Add target alias to "all" target.
.PHONY: all
all: v8_nosnapshot

