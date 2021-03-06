#!/bin/bash

FLAG=$1

case "$OSTYPE" in

  cygwin*)
    rm ./example.exe
    rm ./example.ilk
    rm ./example.pdb
    clicommand="clang"
    compilefiles="./example.c"
    optimize="-O0"
    if [ '--production' = "${FLAG}" ]; then
      echo "production build..."
      optimize="-O3"
    fi
    options="-D DEBUG=1 -g -fsanitize=address -Wall -Werror -Wextra -Wl,-subsystem:console"
    includes=""
    linked_libraries=" -luser32.lib -lshell32.lib -ladvapi32.lib  -lgdi32.lib -ldbghelp.lib -lOpenGL32.lib" # -ld3d9
    framework=""
    output="-o ./run/example.exe -v"
    update_perms=""
  ;;

# building correctly requires that we have command-line XCode tools installed:
  darwin*)
    rm ./run/example
    clicommand="/usr/bin/clang"
    compilefiles="./example.c"
    optimize="-O0"
    if [ '--production' = "${FLAG}" ]; then
      echo "production build..."
      optimize="-O3"
    fi
    options="-D DEBUG=1 -g -fsanitize=address -Wall -Werror -Wextra -ObjC"
    # options="-O3" # higher optimizations
    includes=""
    # library_locs=""
    linked_libraries=""
    framework="-framework AppKit -framework OpenGL"
    output="-o ./run/example -v"
    update_perms="chmod a+x ./run/example"
  ;;
esac

echo "Building with cli command:"
echo $clicommand $compilefiles $optimize $options $linked_libraries $framework $output
$clicommand $compilefiles $optimize $options $includes $linked_libraries $framework $output
$update_perms

# ifdef compiler flags to use:
# _WIN64 (Defined as 1 when the compilation target is 64-bit ARM or x64. Otherwise, undefined.)
# _WIN32 (Defined as 1 when the compilation target is 32-bit ARM, 64-bit ARM, x86, or x64. Otherwise, undefined.)
#
# TARGET_OS_MAC (For Mac OS or iOS)
# __APPLE__ (for Mac OS, will always be there if TARGET_OS_MAC is also defined...)

# OVERVIEW: clang LLVM compiler
#
# USAGE: clang [options] <inputs>
#
# OPTIONS:
# https://clang.llvm.org/docs/ClangCommandLineReference.html
# OVERVIEW: LLVM 'Clang' Compiler: http://clang.llvm.org
#
# USAGE: clang -cc1 [options] <inputs>
#
# OPTIONS:
#   -add-plugin <name>      Use the named plugin action in addition to the default action
#   -analyze-function <value>
#                           Run analysis on specific function (for C++ include parameters in name)
#   -analyzer-checker-help  Display the list of analyzer checkers that are available
#   -analyzer-checker <value>
#                           Choose analyzer checkers to enable
#   -analyzer-config <value>
#                           Choose analyzer options to enable
#   -analyzer-constraints <value>
#                           Source Code Analysis - Symbolic Constraint Engines
#   -analyzer-disable-all-checks
#                           Disable all static analyzer checks
#   -analyzer-disable-checker <value>
#                           Choose analyzer checkers to disable
#   -analyzer-disable-retry-exhausted
#                           Do not re-analyze paths leading to exhausted nodes with a different strategy (may decrease code coverage)
#   -analyzer-display-progress
#                           Emit verbose output about the analyzer's progress
#   -analyzer-eagerly-assume
#                           Eagerly assume the truth/falseness of some symbolic constraints
#   -analyzer-inline-max-stack-depth <value>
#                           Bound on stack depth while inlining (4 by default)
#   -analyzer-inlining-mode <value>
#                           Specify the function selection heuristic used during inlining
#   -analyzer-list-enabled-checkers
#                           Display the list of enabled analyzer checkers
#   -analyzer-max-loop <value>
#                           The maximum number of times the analyzer will go through a loop
#   -analyzer-opt-analyze-headers
#                           Force the static analyzer to analyze functions defined in header files
#   -analyzer-opt-analyze-nested-blocks
#                           Analyze the definitions of blocks in addition to functions
#   -analyzer-output <value>
#                           Source Code Analysis - Output Options
#   -analyzer-purge <value> Source Code Analysis - Dead Symbol Removal Frequency
#   -analyzer-stats         Print internal analyzer statistics.
#   -analyzer-store <value> Source Code Analysis - Abstract Memory Store Models
#   -analyzer-viz-egraph-graphviz
#                           Display exploded graph using GraphViz
#   -analyzer-viz-egraph-ubigraph
#                           Display exploded graph using Ubigraph
#   -analyze                Run static analysis engine
#   -arange_sections        Emit DWARF .debug_arange sections
#   -arcmt-check            Check for ARC migration issues that need manual handling
#   -arcmt-migrate-emit-errors
#                           Emit ARC errors even if the migrator can fix them
#   -arcmt-migrate-report-output <value>
#                           Output path for the plist report
#   -arcmt-migrate          Apply modifications and produces temporary files that conform to ARC
#   -arcmt-modify           Apply modifications to files to conform to ARC
#   -ast-dump-all           Build ASTs and then debug dump them, forcing deserialization
#   -ast-dump-filter <dump_filter>
#                           Use with -ast-dump or -ast-print to dump/print only AST declaration nodes having a certain substring in a qualified name. Use -ast-list to list all filterable declaration node names.
#   -ast-dump-lookups       Build ASTs and then debug dump their name lookup tables
#   -ast-dump               Build ASTs and then debug dump them
#   -ast-list               Build ASTs and print the list of declaration node qualified names
#   -ast-merge <ast file>   Merge the given AST file into the translation unit being compiled.
#   -ast-print              Build ASTs and then pretty-print them
#   -ast-view               Build ASTs and view them with GraphViz
#   -aux-triple <value>     Auxiliary target triple.
#   -backend-option <value> Additional arguments to forward to LLVM backend (during code gen)
#   -c-isystem <directory>  Add directory to the C SYSTEM include search path
#   -CC                     Include comments from within macros in preprocessed output
#   -cfg-add-implicit-dtors Add C++ implicit destructors to CFGs for all analyses
#   -chain-include <file>   Include and chain a header file after turning it into PCH
#   -cl-denorms-are-zero    OpenCL only. Allow denormals to be flushed to zero.
#   -cl-ext=<value>         OpenCL only. Enable or disable OpenCL extensions. The argument is a comma-separated sequence of one or more extension names, each prefixed by '+' or '-'.
#   -cl-fast-relaxed-math   OpenCL only. Sets -cl-finite-math-only and -cl-unsafe-math-optimizations, and defines __FAST_RELAXED_MATH__.
#   -cl-finite-math-only    OpenCL only. Allow floating-point optimizations that assume arguments and results are not NaNs or +-Inf.
#   -cl-fp32-correctly-rounded-divide-sqrt
#                           OpenCL only. Specify that single precision floating-point divide and sqrt used in the program source are correctly rounded.
#   -cl-kernel-arg-info     OpenCL only. Generate kernel argument metadata.
#   -cl-mad-enable          OpenCL only. Allow use of less precise MAD computations in the generated binary.
#   -cl-no-signed-zeros     OpenCL only. Allow use of less precise no signed zeros computations in the generated binary.
#   -cl-opt-disable         OpenCL only. This option disables all optimizations. By default optimizations are enabled.
#   -cl-single-precision-constant
#                           OpenCL only. Treat double precision floating-point constant as single precision constant.
#   -cl-std=<value>         OpenCL language standard to compile for.
#   -cl-strict-aliasing     OpenCL only. This option is added for compatibility with OpenCL 1.0.
#   -cl-unsafe-math-optimizations
#                           OpenCL only. Allow unsafe floating-point optimizations.  Also implies -cl-no-signed-zeros and -cl-mad-enable.
#   -code-completion-at <file>:<line>:<column>
#                           Dump code-completion information at a location
#   -code-completion-brief-comments
#                           Include brief documentation comments in code-completion results.
#   -code-completion-macros Include macros in code-completion results
#   -code-completion-patterns
#                           Include code patterns in code-completion results
#   -compress-debug-sections=<value>
#                           DWARF debug sections compression type
#   -compress-debug-sections
#                           DWARF debug sections compression
#   -coverage-cfg-checksum  Emit CFG checksum for functions in .gcno files.
#   -coverage-data-file <value>
#                           Emit coverage data to this filename.
#   -coverage-exit-block-before-body
#                           Emit the exit block before the body blocks in .gcno files.
#   -coverage-no-function-names-in-data
#                           Emit function names in .gcda files.
#   -coverage-notes-file <value>
#                           Emit coverage notes to this filename.
#   -coverage-version=<value>
#                           Four-byte version string for gcov files.
#   -cxx-isystem <directory>
#                           Add directory to the C++ SYSTEM include search path
#   -C                      Include comments in preprocessed output
#   -dD                     Print macro definitions in -E mode in addition to normal output
#   -debug-forward-template-params
#                           Emit complete descriptions of template parameters in forward declarations
#   -debug-info-macro       Emit macro debug information
#   -dependency-dot <value> Filename to write DOT-formatted header dependencies to
#   -dependency-file <value>
#                           Filename (or -) to write dependency output to
#   --dependent-lib=<value> Add dependent library
#   -detailed-preprocessing-record
#                           include a detailed record of preprocessing actions
#   -diagnostic-log-file <value>
#                           Filename (or -) to log diagnostics to
#   -disable-free           Disable freeing of memory on exit
#   -disable-lifetime-markers
#                           Disable lifetime-markers emission even when optimizations are enabled
#   -disable-llvm-passes    Use together with -emit-llvm to get pristine LLVM IR from the frontend by not running any LLVM passes at all
#   -disable-llvm-verifier  Don't run the LLVM IR verifier pass
#   -disable-O0-optnone     Disable adding the optnone attribute to functions at O0
#   -disable-objc-default-synthesize-properties
#                           disable the default synthesis of Objective-C properties
#   -disable-red-zone       Do not emit code that uses the red zone.
#   -discard-value-names    Discard value names in LLVM IR
#   -dI                     Print include directives in -E mode in addition to normal output
#   -dM                     Print macro definitions in -E mode instead of normal output
#   -dump-coverage-mapping  Dump the coverage mapping records, for testing
#   -dump-deserialized-decls
#                           Dump declarations that are deserialized from PCH, for testing
#   -dump-raw-tokens        Lex file in raw mode and dump raw tokens
#   -dump-tokens            Run preprocessor, dump internal rep of tokens
#   -dwarf-column-info      Turn on column location information.
#   -dwarf-debug-flags <value>
#                           The string to embed in the Dwarf debug flags record.
#   -dwarf-explicit-import  Generate explicit import from anonymous namespace to containing scope
#   -dwarf-ext-refs         Generate debug info with external references to clang modules or precompiled headers
#   -D <macro>=<value>      Define <macro> to <value> (or 1 if <value> omitted)
#   -emit-codegen-only      Generate machine code, but discard output
#   -emit-html              Output input source as HTML
#   -emit-llvm-bc           Build ASTs then convert to LLVM, emit .bc file
#   -emit-llvm-only         Build ASTs and convert to LLVM, discarding output
#   -emit-llvm-uselists     Preserve order of LLVM use-lists when serializing
#   -emit-llvm              Use the LLVM representation for assembler and object files
#   -emit-module-interface  Generate pre-compiled module file from a C++ module interface
#   -emit-module            Generate pre-compiled module file from a module map
#   -emit-obj               Emit native object files
#   -emit-pch               Generate pre-compiled header file
#   -emit-pth               Generate pre-tokenized header file
#   -enable-split-dwarf     Use split dwarf/Fission
#   -Eonly                  Just run preprocessor, no output (for timings)
#   -error-on-deserialized-decl <value>
#                           Emit error if a specific declaration is deserialized from PCH, for testing
#   -E                      Only run the preprocessor
#   -faddress-space-map-mangling=<yes|no|target>
#                           Set the mode for address space map based mangling; OpenCL testing purposes only
#   -faligned-alloc-unavailable
#                           Aligned allocation/deallocation functions are unavailable
#   -faligned-allocation    Enable C++17 aligned allocation functions
#   -fallow-editor-placeholders
#                           Treat editor placeholders as valid source code
#   -fallow-half-arguments-and-returns
#                           Allow function arguments and returns of type half
#   -fallow-pch-with-compiler-errors
#                           Accept a PCH file that was created with compiler errors
#   -fansi-escape-codes     Use ANSI escape codes for diagnostics
#   -fapinotes-modules      Enable module-based external API notes support
#   -fapinotes-swift-version=<version>
#                           Specify the Swift version to use when filtering API notes
#   -fapinotes              Enable external API notes support
#   -fapple-kext-vtable-mitigation
#                           Enable mitigation for kernel v-table exploits through null pointers
#   -fapple-kext            Use Apple's kernel extensions ABI
#   -fapple-pragma-pack     Enable Apple gcc-compatible #pragma pack handling
#   -fapplication-extension Restrict code to those available for App Extensions
#   -fblocks-runtime-optional
#                           Weakly link in the blocks runtime
#   -fblocks                Enable the 'blocks' language feature
#   -fborland-extensions    Accept non-standard constructs supported by the Borland compiler
#   -fbracket-depth <value> Maximum nesting level for parentheses, brackets, and braces
#   -fbuild-session-timestamp=<time since Epoch in seconds>
#                           Time when the current build session started
#   -fcaret-diagnostics-max-lines <N>
#                           Set the maximum number of source lines to show in a caret diagnostic
#   -fclang-abi-compat=<version>
#                           Attempt to match the ABI of Clang <version>
#   -fcolor-diagnostics     Use colors in diagnostics
#   -fcomment-block-commands=<arg>
#                           Treat each comma separated argument in <arg> as a documentation comment block command
#   -fconcepts-ts           Enable C++ Extensions for Concepts.
#   -fconst-strings         Use a const qualified type for string literals in C and ObjC
#   -fconstant-string-class <class name>
#                           Specify the class to use for constant Objective-C string objects.
#   -fconstexpr-backtrace-limit <N>
#                           Set the maximum number of entries to print in a constexpr evaluation backtrace (0 = no limit).
#   -fconstexpr-depth <value>
#                           Maximum depth of recursive constexpr function calls
#   -fconstexpr-steps <value>
#                           Maximum number of steps in constexpr function evaluation
#   -fcoroutines-ts         Enable support for the C++ Coroutines TS
#   -fcoverage-mapping      Generate coverage mapping to enable code coverage analysis
#   -fcuda-allow-variadic-functions
#                           Allow variadic functions in CUDA device code.
#   -fcuda-approx-transcendentals
#                           Use approximate transcendental functions
#   -fcuda-flush-denormals-to-zero
#                           Flush denormal floating point values to zero in CUDA device mode.
#   -fcuda-include-gpubinary <value>
#                           Incorporate CUDA device-side binary into host object file.
#   -fcuda-is-device        Generate code for CUDA device
#   -fcxx-exceptions        Enable C++ exceptions
#   -fdata-sections         Place each data in its own section (ELF Only)
#   -fdebug-compilation-dir <value>
#                           The compilation directory to embed in the debug info.
#   -fdebug-info-for-profiling
#                           Emit extra debug info to make sample profile more accurate.
#   -fdebug-pass-manager    Prints debug information for the new pass manager
#   -fdebug-prefix-map=<value>
#                           remap file source paths in debug info
#   -fdebug-types-section   Place debug types in their own section (ELF Only)
#   -fdebugger-cast-result-to-id
#                           Enable casting unknown expression results to id
#   -fdebugger-objc-literal Enable special debugger support for Objective-C subscripting and literals
#   -fdebugger-support      Enable special debugger support behavior
#   -fdeclspec              Allow __declspec as a keyword
#   -fdefault-calling-conv=<value>
#                           Set default calling convention
#   -fdelayed-template-parsing
#                           Parse templated function definitions at the end of the translation unit
#   -fdeprecated-macro      Defines the __DEPRECATED macro
#   -fdiagnostics-absolute-paths
#                           Print absolute paths in diagnostics
#   -fdiagnostics-format <value>
#                           Change diagnostic formatting to match IDE and command line tools
#   -fdiagnostics-hotness-threshold=<number>
#                           Prevent optimization remarks from being output if they do not have at least this profile count
#   -fdiagnostics-parseable-fixits
#                           Print fix-its in machine parseable form
#   -fdiagnostics-print-source-range-info
#                           Print source range spans in numeric form
#   -fdiagnostics-show-category <value>
#                           Print diagnostic category
#   -fdiagnostics-show-hotness
#                           Enable profile hotness information in diagnostic line
#   -fdiagnostics-show-note-include-stack
#                           Display include stacks for diagnostic notes
#   -fdiagnostics-show-option
#                           Print option name with mappable diagnostics
#   -fdiagnostics-show-template-tree
#                           Print a template comparison tree for differing templates
#   -fdisable-module-hash   Disable the module hash
#   -fdollars-in-identifiers
#                           Allow '$' in identifiers
#   -fdouble-square-bracket-attributes
#                           Enable '[[]]' attributes in all C and C++ language modes
#   -fdump-record-layouts-simple
#                           Dump record layout information in a simple form used for testing
#   -fdump-record-layouts   Dump record layout information
#   -fdump-vtable-layouts   Dump the layouts of all vtables that will be emitted in a translation unit
#   -fdwarf-exceptions      Use DWARF style exceptions
#   -fembed-bitcode=<option>
#                           Embed LLVM bitcode (option: off, all, bitcode, marker)
#   -femit-all-decls        Emit all declarations, even if unused
#   -femit-coverage-data    Instrument the program to emit gcov coverage data when run.
#   -femit-coverage-notes   Emit a gcov coverage notes file when compiling.
#   -femulated-tls          Use emutls functions to access thread_local variables
#   -fencode-extended-block-signature
#                           enable extended encoding of block type signature
#   -ferror-limit <N>       Set the maximum number of errors to emit before stopping (0 = no limit).
#   -fexceptions            Enable support for exception handling
#   -fexperimental-new-pass-manager
#                           Enables an experimental new pass manager in LLVM.
#   -fexternc-nounwind      Assume all functions with C linkage do not unwind
#   -ffake-address-space-map
#                           Use a fake address space map; OpenCL testing purposes only
#   -ffast-math             Allow aggressive, lossy floating-point optimizations
#   -ffine-grained-bitfield-accesses
#                           Use separate accesses for bitfields with legal widths and alignments.
#   -fforbid-guard-variables
#                           Emit an error if a C++ static local initializer would need a guard variable
#   -ffp-contract=<value>   Form fused FP ops (e.g. FMAs): fast (everywhere) | on (according to FP_CONTRACT pragma, default) | off (never fuse)
#   -ffreestanding          Assert that the compilation takes place in a freestanding environment
#   -ffunction-sections     Place each function in its own section (ELF Only)
#   -fgnu-keywords          Allow GNU-extension keywords regardless of language standard
#   -fgnu89-inline          Use the gnu89 inline semantics
#   -fignore-objc-weak      ignore __weak qualifiers in manual reference counting
#   -fimplicit-module-maps  Implicitly search the file system for module map files.
#   -finclude-default-header
#                           Include the default header file for OpenCL
#   -find-pch-source=<value>
#                           When building a pch, try to find the input file in include directories, as if it had been included by the argument passed to this flag.
#   -finline-functions      Inline suitable functions
#   -finline-hint-functions Inline functions which are (explicitly or implicitly) marked inline
#   -finstrument-function-entry-bare
#                           Instrument function entry only, after inlining, without arguments to the instrumentation call
#   -finstrument-functions-after-inlining
#                           Like -finstrument-functions, but insert the calls after inlining
#   -finstrument-functions  Generate calls to instrument function entry and exit
#   -fix-only-warnings      Apply fix-it advice only for warnings, not errors
#   -fix-what-you-can       Apply fix-it advice even in the presence of unfixable errors
#   -fixit-recompile        Apply fix-it changes and recompile
#   -fixit-to-temporary     Apply fix-it changes to temporary files
#   -fixit=<value>          Apply fix-it advice creating a file with the given suffix
#   -fixit                  Apply fix-it advice to the input source
#   -flto-jobs=<value>      Controls the backend parallelism of -flto=thin (default of 0 means the number of threads will be derived from the number of CPUs detected)
#   -flto-unit              Emit IR to support LTO unit features (CFI, whole program vtable opt)
#   -flto-visibility-public-std
#                           Use public LTO visibility for classes in std and stdext namespaces
#   -flto=<value>           Set LTO mode to either 'full' or 'thin'
#   -flto                   Enable LTO in 'full' mode
#   -fmacro-backtrace-limit <N>
#                           Set the maximum number of entries to print in a macro expansion backtrace (0 = no limit).
#   -fmath-errno            Require math functions to indicate errors by setting errno
#   -fmax-type-align=<value>
#                           Specify the maximum alignment to enforce on pointers lacking an explicit alignment
#   -fmerge-functions       Permit merging of identical functions when optimizing.
#   -fmessage-length <N>    Format message diagnostics so that they fit within N columns or fewer, when possible.
#   -fmodule-feature <feature>
#                           Enable <feature> in module map requires declarations
#   -fmodule-file=[<name>=]<file>
#                           Specify the mapping of module name to precompiled module file, or load a module file if name is omitted.
#   -fmodule-format=<value> Select the container format for clang modules and PCH. Supported options are 'raw' and 'obj'.
#   -fmodule-map-file-home-is-cwd
#                           Use the current working directory as the home directory of module maps specified by -fmodule-map-file=<FILE>
#   -fmodule-map-file=<file>
#                           Load this module map file
#   -fmodule-name=<name>    Specify the name of the module to build
#   -fmodules-cache-path=<directory>
#                           Specify the module cache path
#   -fmodules-codegen       Generate code for uses of this module that assumes an explicit object file will be built for the module
#   -fmodules-debuginfo     Generate debug info for types in an object file built from this module and do not generate them elsewhere
#   -fmodules-decluse       Require declaration of modules used within a module
#   -fmodules-disable-diagnostic-validation
#                           Disable validation of the diagnostic options when loading the module
#   -fmodules-embed-all-files<value>
#                           Embed the contents of all files read by this compilation into the produced module file.
#   -fmodules-embed-file=<file>
#                           Embed the contents of the specified file into the module file being compiled.
#   -fmodules-hash-content  Enable hashing the content of a module file
#   -fmodules-hash-error-diagnostics
#                           Use a separate module cache for modules compiled with conflicting -Werror options
#   -fmodules-ignore-macro=<value>
#                           Ignore the definition of the given macro when building and loading modules
#   -fmodules-local-submodule-visibility
#                           Enforce name visibility rules across submodules of the same top-level module.
#   -fmodules-prune-after=<seconds>
#                           Specify the interval (in seconds) after which a module file will be considered unused
#   -fmodules-prune-interval=<seconds>
#                           Specify the interval (in seconds) between attempts to prune the module cache
#   -fmodules-search-all    Search even non-imported modules to resolve references
#   -fmodules-strict-decluse
#                           Like -fmodules-decluse but requires all headers to be in modules
#   -fmodules-ts            Enable support for the C++ Modules TS
#   -fmodules-user-build-path <directory>
#                           Specify the module user build path
#   -fmodules-validate-once-per-build-session
#                           Don't verify input files for the modules if the module has been successfully validated or loaded during this build session
#   -fmodules-validate-system-headers
#                           Validate the system headers that a module depends on when loading the module
#   -fmodules               Enable the 'modules' language feature
#   -fms-compatibility-version=<value>
#                           Dot-separated value representing the Microsoft compiler version number to report in _MSC_VER (0 = don't define it (default))
#   -fms-compatibility      Enable full Microsoft Visual C++ compatibility
#   -fms-extensions         Accept some non-standard constructs supported by the Microsoft compiler
#   -fnative-half-arguments-and-returns
#                           Use the native __fp16 type for arguments and returns (and skip ABI-specific lowering)
#   -fnative-half-type      Use the native half type for __fp16 instead of promoting to float
#   -fnew-alignment=<align> Specifies the largest alignment guaranteed by '::operator new(size_t)'
#   -fno-access-control     Disable C++ access control
#   -fno-apinotes-modules   Disable module-based external API notes support
#   -fno-apinotes           Disable external API notes support
#   -fno-assume-sane-operator-new
#                           Don't assume that C++'s global operator new can't alias any pointer
#   -fno-autolink           Disable generation of linker directives for automatic library linking
#   -fno-bitfield-type-align
#                           Ignore bit-field types when aligning structures
#   -fno-builtin-<value>    Disable implicit builtin knowledge of a specific function
#   -fno-builtin            Disable implicit builtin knowledge of functions
#   -fno-common             Compile common globals like normal definitions
#   -fno-const-strings      Don't use a const qualified type for string literals in C and ObjC
#   -fno-constant-cfstrings Disable creation of CodeFoundation-type constant strings
#   -fno-cuda-host-device-constexpr
#                           Don't treat unattributed constexpr functions as __host__ __device__.
#   -fno-debug-pass-manager Disables debug printing for the new pass manager
#   -fno-declspec           Disallow __declspec as a keyword
#   -fno-deprecated-macro   Undefines the __DEPRECATED macro
#   -fno-diagnostics-fixit-info
#                           Do not include fixit information in diagnostics
#   -fno-diagnostics-use-presumed-location
#                           Ignore #line directives when displaying diagnostic locations
#   -fno-dollars-in-identifiers
#                           Disallow '$' in identifiers
#   -fno-double-square-bracket-attributes
#                           Disable '[[]]' attributes in all C and C++ language modes
#   -fno-elide-constructors Disable C++ copy constructor elision
#   -fno-elide-type         Do not elide types when printing diagnostics
#   -fno-experimental-new-pass-manager
#                           Disables an experimental new pass manager in LLVM.
#   -fno-fine-grained-bitfield-accesses
#                           Use large-integer access for consecutive bitfield runs.
#   -fno-gnu-inline-asm     Disable GNU style inline asm
#   -fno-integrated-as      Disable the integrated assembler
#   -fno-jump-tables        Do not use jump tables for lowering switches
#   -fno-lax-vector-conversions
#                           Disallow implicit conversions between vectors with a different number of elements or different element types
#   -fno-math-builtin       Disable implicit builtin knowledge of math functions
#   -fno-merge-all-constants
#                           Disallow merging of constants
#   -fno-modules-error-recovery
#                           Do not automatically import modules for error recovery
#   -fno-modules-global-index
#                           Do not automatically generate or update the global module index
#   -fno-objc-infer-related-result-type
#                           do not infer Objective-C related result type based on method family
#   -fno-openmp-simd        Disable OpenMP code for SIMD-based constructs.
#   -fno-operator-names     Do not treat C++ operator name keywords as synonyms for operators
#   -fno-pch-timestamp      Disable inclusion of timestamp in precompiled headers
#   -fno-plt                Do not use the PLT to make function calls
#   -fno-preserve-as-comments
#                           Do not preserve comments in inline assembly
#   -fno-rtti-data          Control emission of RTTI data
#   -fno-rtti               Disable generation of rtti information
#   -fno-sanitize-address-use-after-scope
#                           Disable use-after-scope detection in AddressSanitizer
#   -fno-sanitize-blacklist Don't use blacklist file for sanitizers
#   -fno-sanitize-cfi-cross-dso
#                           Disable control flow integrity (CFI) checks for cross-DSO calls.
#   -fno-sanitize-coverage=<value>
#                           Disable specified features of coverage instrumentation for Sanitizers
#   -fno-sanitize-memory-track-origins
#                           Disable origins tracking in MemorySanitizer
#   -fno-sanitize-memory-use-after-dtor
#                           Disable use-after-destroy detection in MemorySanitizer
#   -fno-sanitize-recover=<value>
#                           Disable recovery for specified sanitizers
#   -fno-sanitize-stats     Disable sanitizer statistics gathering.
#   -fno-sanitize-thread-atomics
#                           Disable atomic operations instrumentation in ThreadSanitizer
#   -fno-sanitize-thread-func-entry-exit
#                           Disable function entry/exit instrumentation in ThreadSanitizer
#   -fno-sanitize-thread-memory-access
#                           Disable memory access instrumentation in ThreadSanitizer
#   -fno-sanitize-trap=<value>
#                           Disable trapping for specified sanitizers
#   -fno-show-column        Do not include column number on diagnostics
#   -fno-show-source-location
#                           Do not include source location information with diagnostics
#   -fno-signed-char        Char is unsigned
#   -fno-signed-wchar       Use an unsigned type for wchar_t
#   -fno-signed-zeros       Allow optimizations that ignore the sign of floating point zeros
#   -fno-spell-checking     Disable spell-checking
#   -fno-stack-check        Disable stack checking
#   -fno-threadsafe-statics Do not emit code to make initialization of local statics thread safe
#   -fno-trigraphs          Do not process trigraph sequences
#   -fno-unroll-loops       Turn off loop unroller
#   -fno-use-cxa-atexit     Don't use __cxa_atexit for calling destructors
#   -fno-use-init-array     Don't use .init_array instead of .ctors
#   -fno-validate-pch       Disable validation of precompiled headers
#   -fno-wchar              Disable C++ builtin type wchar_t
#   -fnoopenmp-relocatable-target
#                           Do not compile OpenMP target code as relocatable.
#   -fobjc-arc-cxxlib=<value>
#                           Objective-C++ Automatic Reference Counting standard library kind
#   -fobjc-arc-exceptions   Use EH-safe code when synthesizing retains and releases in -fobjc-arc
#   -fobjc-arc-unsafeclaim  Enable an optimization to dynamically avoid retain/releases of call results in __unsafe_unretained contexts
#   -fobjc-arc              Synthesize retain and release calls for Objective-C pointers
#   -fobjc-dispatch-method=<value>
#                           Objective-C dispatch method to use
#   -fobjc-exceptions       Enable Objective-C exceptions
#   -fobjc-gc-only          Use GC exclusively for Objective-C related memory management
#   -fobjc-gc               Enable Objective-C garbage collection
#   -fobjc-runtime-has-weak The target Objective-C runtime supports ARC weak operations
#   -fobjc-runtime=<value>  Specify the target Objective-C runtime kind and version
#   -fobjc-subscripting-legacy-runtime
#                           Allow Objective-C array and dictionary subscripting in legacy runtime
#   -fobjc-weak             Enable ARC-style weak references in Objective-C
#   -fopenmp-host-ir-file-path <value>
#                           Path to the IR file produced by the frontend for the host.
#   -fopenmp-is-device      Generate code only for an OpenMP target device.
#   -fopenmp-relocatable-target
#                           OpenMP target code is compiled as relocatable using the -c flag. For OpenMP targets the code is relocatable by default.
#   -fopenmp-simd           Emit OpenMP code only for SIMD-based constructs.
#   -fopenmp-targets=<value>
#                           Specify comma-separated list of triples OpenMP offloading targets to be supported
#   -foperator-arrow-depth <value>
#                           Maximum number of 'operator->'s to call for a member access
#   -foverride-record-layout=<value>
#                           Override record layouts with those in the given file
#   -fpack-struct=<value>   Specify the default maximum struct packing alignment
#   -fpascal-strings        Recognize and construct Pascal-style string literals
#   -fpcc-struct-return     Override the default ABI to return all structs on the stack
#   -fplt                   Use the PLT to make function calls
#   -fprebuilt-module-path=<directory>
#                           Specify the prebuilt module path
#   -fpreserve-vec3-type    Preserve 3-component vector type
#   -fprofile-instrument-path=<value>
#                           Generate instrumented code to collect execution counts into <file> (overridden by LLVM_PROFILE_FILE env var)
#   -fprofile-instrument-use-path=<value>
#                           Specify the profile path in PGO use compilation
#   -fprofile-instrument=<value>
#                           Enable PGO instrumentation. The accepted value is clang, llvm, or none
#   -fprofile-sample-accurate
#                           Specifies that the sample profile is accurate
#   -fprofile-sample-use=<value>
#                           Enable sample-based profile guided optimizations
#   -fptrauth-calls         Enable signing and authentication of all indirect calls
#   -fptrauth-intrinsics    Enable pointer-authentication intrinsics
#   -fptrauth-returns       Enable signing and authentication of return addresses
#   -fptrauth-soft          Enable software lowering of pointer authentication
#   -freciprocal-math       Allow division operations to be reassociated
#   -freg-struct-return     Override the default ABI to return small structs in registers
#   -frelaxed-template-template-args
#                           Enable C++17 relaxed template template argument matching
#   -freroll-loops          Turn on loop reroller
#   -fsanitize-address-field-padding=<value>
#                           Level of field padding for AddressSanitizer
#   -fsanitize-address-globals-dead-stripping
#                           Enable linker dead stripping of globals in AddressSanitizer
#   -fsanitize-address-use-after-scope
#                           Enable use-after-scope detection in AddressSanitizer
#   -fsanitize-blacklist=<value>
#                           Path to blacklist file for sanitizers
#   -fsanitize-cfi-cross-dso
#                           Enable control flow integrity (CFI) checks for cross-DSO calls.
#   -fsanitize-cfi-icall-generalize-pointers
#                           Generalize pointers in CFI indirect call type signature checks
#   -fsanitize-coverage-8bit-counters
#                           Enable frequency counters in sanitizer coverage
#   -fsanitize-coverage-indirect-calls
#                           Enable sanitizer coverage for indirect calls
#   -fsanitize-coverage-inline-8bit-counters
#                           Enable inline 8-bit counters in sanitizer coverage
#   -fsanitize-coverage-no-prune
#                           Disable coverage pruning (i.e. instrument all blocks/edges)
#   -fsanitize-coverage-pc-table
#                           Create a table of coverage-instrumented PCs
#   -fsanitize-coverage-stack-depth
#                           Enable max stack depth tracing
#   -fsanitize-coverage-trace-bb
#                           Enable basic block tracing in sanitizer coverage
#   -fsanitize-coverage-trace-cmp
#                           Enable cmp instruction tracing in sanitizer coverage
#   -fsanitize-coverage-trace-div
#                           Enable div instruction tracing in sanitizer coverage
#   -fsanitize-coverage-trace-gep
#                           Enable gep instruction tracing in sanitizer coverage
#   -fsanitize-coverage-trace-pc-guard
#                           Enable PC tracing with guard in sanitizer coverage
#   -fsanitize-coverage-trace-pc
#                           Enable PC tracing in sanitizer coverage
#   -fsanitize-coverage-type=<value>
#                           Sanitizer coverage type
#   -fsanitize-coverage=<value>
#                           Specify the type of coverage instrumentation for Sanitizers
#   -fsanitize-memory-track-origins=<value>
#                           Enable origins tracking in MemorySanitizer
#   -fsanitize-memory-track-origins
#                           Enable origins tracking in MemorySanitizer
#   -fsanitize-memory-use-after-dtor
#                           Enable use-after-destroy detection in MemorySanitizer
#   -fsanitize-recover=<value>
#                           Enable recovery for specified sanitizers
#   -fsanitize-stats        Enable sanitizer statistics gathering.
#   -fsanitize-thread-atomics
#                           Enable atomic operations instrumentation in ThreadSanitizer (default)
#   -fsanitize-thread-func-entry-exit
#                           Enable function entry/exit instrumentation in ThreadSanitizer (default)
#   -fsanitize-thread-memory-access
#                           Enable memory access instrumentation in ThreadSanitizer (default)
#   -fsanitize-trap=<value> Enable trapping for specified sanitizers
#   -fsanitize-undefined-strip-path-components=<number>
#                           Strip (or keep only, if negative) a given number of path components when emitting check metadata.
#   -fsanitize=<check>      Turn on runtime checks for various forms of undefined or suspicious behavior. See user manual for available checks
#   -fseh-exceptions        Use SEH style exceptions
#   -fshort-enums           Allocate to an enum type only as many bytes as it needs for the declared range of possible values
#   -fshow-overloads=<value>
#                           Which overload candidates to show when overload resolution fails: best|all; defaults to all
#   -fsigned-wchar          Use a signed type for wchar_t
#   -fsized-deallocation    Enable C++14 sized global deallocation functions
#   -fsjlj-exceptions       Use SjLj style exceptions
#   -fspell-checking-limit <N>
#                           Set the maximum number of times to perform spell checking on unrecognized identifiers (0 = no limit).
#   -fsplit-dwarf-inlining  Place debug types in their own section (ELF Only)
#   -fstack-check           Enable stack checking
#   -fstrict-enums          Enable optimizations based on the strict definition of an enum's value range
#   -fstrict-return         Always treat control flow paths that fall off the end of a non-void function as unreachable
#   -fstrict-vtable-pointers
#                           Enable optimizations based on the strict rules for overwriting polymorphic C++ objects
#   -ftabstop <N>           Set the tab stop distance.
#   -ftemplate-backtrace-limit <N>
#                           Set the maximum number of entries to print in a template instantiation backtrace (0 = no limit).
#   -ftemplate-depth <value>
#                           Maximum depth of recursive template instantiation
#   -ftest-module-file-extension=<value>
#                           introduce a module file extension for testing purposes. The argument is parsed as blockname:major:minor:hashed:user info
#   -fthin-link-bitcode=<value>
#                           Write minimized bitcode to <file> for the ThinLTO thin link only
#   -fthinlto-index=<value> Perform ThinLTO importing using provided function summary index
#   -ftrap-function=<value> Issue call to specified function rather than a trap instruction
#   -ftrapv                 Trap on integer overflow
#   -ftrigraphs             Process trigraph sequences
#   -ftype-visibility <value>
#                           Default type visibility
#   -funique-section-names  Use unique names for text and data sections (ELF Only)
#   -funknown-anytype       Enable parser support for the __unknown_anytype type; for testing purposes only
#   -funroll-loops          Turn on loop unroller
#   -fuse-init-array        Use .init_array instead of .ctors
#   -fuse-register-sized-bitfield-access
#                           Use register sized accesses to bit-fields, when possible.
#   -fveclib=<value>        Use the given vector functions library
#   -fvisibility-inlines-hidden
#                           Give inline C++ member functions default visibility by default
#   -fvisibility <value>    Default type and symbol visibility
#   -fwchar-type=<value>    Select underlying type for wchar_t
#   -fwhole-program-vtables Enables whole-program vtable optimization. Requires -flto
#   -fwrapv                 Treat signed integer overflow as two's complement
#   -fwritable-strings      Store string literals as writable data
#   -fxray-always-emit-customevents
#                           Determine whether to always emit __xray_customevent(...) calls even if the function it appears in is not always instrumented.
#   -fxray-always-instrument= <value>
#                           Filename defining the whitelist for imbuing the 'always instrument' XRay attribute.
#   -fxray-instruction-threshold= <value>
#                           Sets the minimum function size to instrument with XRay
#   -fxray-instrument       Generate XRay instrumentation sleds on function entry and exit
#   -fxray-never-instrument= <value>
#                           Filename defining the whitelist for imbuing the 'never instrument' XRay attribute.
#   -fzvector               Enable System z vector language extension
#   -F <value>              Add directory to framework include search path
#   -gcodeview              Generate CodeView debug information
#   -gnu-pubnames           Emit newer GNU style pubnames
#   -header-include-file <value>
#                           Filename (or -) to write header include output to
#   -help                   Display available options
#   -H                      Show header includes and nesting depth
#   -iapinotes-modules <directory>
#                           Add directory to the API notes search path referenced by module name
#   -idirafter <value>      Add directory to AFTER include search path
#   -iframeworkwithsysroot <directory>
#                           Add directory to SYSTEM framework search path, absolute paths are relative to -isysroot
#   -iframework <value>     Add directory to SYSTEM framework search path
#   -imacros <file>         Include macros from file before parsing
#   -include-pch <file>     Include precompiled header file
#   -include-pth <file>     Include file before parsing
#   -include <file>         Include file before parsing
#   -index-header-map       Make the next included directory (-I or -F) an indexer header map
#   -index-ignore-system-symbols
#                           Ignore symbols from system headers
#   -index-record-codegen-name
#                           Record the codegen name for symbols
#   -index-store-path <value>
#                           Enable indexing with the specified data store path
#   -init-only              Only execute frontend initialization
#   -internal-externc-isystem <directory>
#                           Add directory to the internal system include search path with implicit extern "C" semantics; these are assumed to not be user-provided and are used to model system and standard headers' paths.
#   -internal-isystem <directory>
#                           Add directory to the internal system include search path; these are assumed to not be user-provided and are used to model system and standard headers' paths.
#   -iprefix <dir>          Set the -iwithprefix/-iwithprefixbefore prefix
#   -iquote <directory>     Add directory to QUOTE include search path
#   -isysroot <dir>         Set the system root directory (usually /)
#   -isystem <directory>    Add directory to SYSTEM include search path
#   -ivfsoverlay <value>    Overlay the virtual filesystem described by file over the real file system
#   -iwithprefixbefore <dir>
#                           Set directory to include search path with prefix
#   -iwithprefix <dir>      Set directory to SYSTEM include search path with prefix
#   -iwithsysroot <directory>
#                           Add directory to SYSTEM include search path, absolute paths are relative to -isysroot
#   -I <dir>                Add directory to include search path
#   --linker-option=<value> Add linker option
#   -load <dsopath>         Load the named plugin (dynamic shared object)
#   -main-file-name <value> Main file name to use for debug info
#   -malign-double          Align doubles to two words in structs (x86 only)
#   -masm-verbose           Generate verbose assembly output
#   -massembler-fatal-warnings
#                           Make assembler warnings fatal
#   -mbackchain             Link stack frames through backchain on System Z
#   -mcode-model <value>    The code model to use
#   -mconstructor-aliases   Emit complete constructors and destructors as aliases when possible
#   -mdebug-pass <value>    Enable additional debug output
#   -mdisable-fp-elim       Disable frame pointer elimination optimization
#   -mdisable-tail-calls    Disable tail call optimization, keeping the call stack accurate
#   -meabi <value>          Set EABI type, e.g. 4, 5 or gnu (default depends on triple)
#   -menable-no-infs        Allow optimization to assume there are no infinities.
#   -menable-no-nans        Allow optimization to assume there are no NaNs.
#   -menable-unsafe-fp-math Allow unsafe floating-point math optimizations which may decrease precision
#   -mfentry                Insert calls to fentry at function entry (x86 only)
#   -mfloat-abi <value>     The float ABI to use
#   -mfpmath <value>        Which unit to use for fp math
#   -mglobal-merge          Enable merging of globals
#   -MG                     Add missing headers to depfile
#   -migrate                Migrate source code
#   -mincremental-linker-compatible
#                           (integrated-as) Emit an object file which can be used with an incremental linker
#   -mlimit-float-precision <value>
#                           Limit float precision to the given value
#   -mlink-bitcode-file <value>
#                           Link the given bitcode file before performing optimizations.
#   -mlink-cuda-bitcode <value>
#                           Link and internalize needed symbols from the given bitcode file before performing optimizations.
#   -mllvm <value>          Additional arguments to forward to LLVM's option processing
#   -mms-bitfields          Set the default structure layout to be compatible with the Microsoft compiler standard
#   -mno-global-merge       Disable merging of globals
#   -mno-stack-arg-probe    Disable stack probes which are enabled by default
#   -mno-zero-initialized-in-bss
#                           Do not put zero initialized data in the BSS
#   -mnoexecstack           Mark the file as not needing an executable stack
#   -module-dependency-dir <value>
#                           Directory to dump module dependencies to
#   -module-file-deps       Include module files in dependency output
#   -module-file-info       Provide information about a particular module file
#   -momit-leaf-frame-pointer
#                           Omit frame pointer setup for leaf functions
#   -moutline               Enable function outlining (AArch64 only)
#   -mpie-copy-relocations  Use copy relocations support for PIE builds
#   -mprefer-vector-width=<value>
#                           Specifies preferred vector width for auto-vectorization. Defaults to 'none' which allows target specific decisions.
#   -MP                     Create phony target for each dependency (other than main file)
#   -mqdsp6-compat          Enable hexagon-qdsp6 backward compatibility
#   -MQ <value>             Specify name of main file output to quote in depfile
#   -mreassociate           Allow reassociation transformations for floating-point instructions
#   -mregparm <value>       Limit the number of registers available for integer arguments
#   -mrelax-all             (integrated-as) Relax all machine instructions
#   --mrelax-relocations    Use relaxable elf relocations
#   -mrelocation-model <value>
#                           The relocation model to use
#   -mrtd                   Make StdCall calling convention the default
#   -msave-temp-labels      Save temporary labels in the symbol table. Note this may change .s semantics and shouldn't generally be used on compiler-generated code.
#   -msoft-float            Use software floating point
#   -mstack-alignment=<value>
#                           Set the stack alignment
#   -mstack-probe-size=<value>
#                           Set the stack probe size
#   -mstackrealign          Force realign the stack at entry to every function
#   -mstrict-align          Force all memory accesses to be aligned (same as mno-unaligned-access)
#   -mt-migrate-directory <value>
#                           Directory for temporary files produced during ARC or ObjC migration
#   -mthread-model <value>  The thread model to use, e.g. posix, single (posix by default)
#   -mtp <value>            Mode for reading thread pointer
#   -MT <value>             Specify name of main file output in depfile
#   -munwind-tables         Generate unwinding tables for all functions
#   -MV                     Use NMake/Jom format for the depfile
#   -new-struct-path-tbaa   Enable enhanced struct-path aware Type Based Alias Analysis
#   -no-code-completion-globals
#                           Do not include global declarations in code-completion results.
#   -no-code-completion-ns-level-decls
#                           Do not include declarations inside namespaces (incl. global namespace) in the code-completion results.
#   -no-emit-llvm-uselists  Don't preserve order of LLVM use-lists when serializing
#   -no-finalize-removal    Do not remove finalize method in gc mode
#   -no-implicit-float      Don't generate implicit floating point instructions
#   -no-ns-alloc-error      Do not error on use of NSAllocateCollectable/NSReallocateCollectable
#   -no-struct-path-tbaa    Turn off struct-path aware Type Based Alias Analysis
#   --no-system-header-prefix=<prefix>
#                           Treat all #include paths starting with <prefix> as not including a system header.
#   -nobuiltininc           Disable builtin #include directories
#   -nostdinc++             Disable standard #include directories for the C++ standard library
#   -nostdsysteminc         Disable standard system #include directories
#   -objc-isystem <directory>
#                           Add directory to the ObjC SYSTEM include search path
#   -objcmt-atomic-property Make migration to 'atomic' properties
#   -objcmt-migrate-all     Enable migration to modern ObjC
#   -objcmt-migrate-annotation
#                           Enable migration to property and method annotations
#   -objcmt-migrate-designated-init
#                           Enable migration to infer NS_DESIGNATED_INITIALIZER for initializer methods
#   -objcmt-migrate-instancetype
#                           Enable migration to infer instancetype for method result type
#   -objcmt-migrate-literals
#                           Enable migration to modern ObjC literals
#   -objcmt-migrate-ns-macros
#                           Enable migration to NS_ENUM/NS_OPTIONS macros
#   -objcmt-migrate-property-dot-syntax
#                           Enable migration of setter/getter messages to property-dot syntax
#   -objcmt-migrate-property
#                           Enable migration to modern ObjC property
#   -objcmt-migrate-protocol-conformance
#                           Enable migration to add protocol conformance on classes
#   -objcmt-migrate-readonly-property
#                           Enable migration to modern ObjC readonly property
#   -objcmt-migrate-readwrite-property
#                           Enable migration to modern ObjC readwrite property
#   -objcmt-migrate-subscripting
#                           Enable migration to modern ObjC subscripting
#   -objcmt-ns-nonatomic-iosonly
#                           Enable migration to use NS_NONATOMIC_IOSONLY macro for setting property's 'atomic' attribute
#   -objcmt-returns-innerpointer-property
#                           Enable migration to annotate property with NS_RETURNS_INNER_POINTER
#   -objcmt-whitelist-dir-path=<value>
#                           Only modify files with a filename contained in the provided directory path
#   -objcxx-isystem <directory>
#                           Add directory to the ObjC++ SYSTEM include search path
#   -opt-record-file <value>
#                           File name to use for YAML optimization record output
#   -o <file>               Write output to <file>
#   -pg                     Enable mcount instrumentation
#   -pic-is-pie             File is for a position independent executable
#   -pic-level <value>      Value for __PIC__
#   -plugin-arg-<name> <arg>
#                           Pass <arg> to plugin <name>
#   -plugin <name>          Use the named plugin action instead of the default action (use "help" to list available options)
#   -preamble-bytes=<value> Assume that the precompiled header is a precompiled preamble covering the first N bytes of the main file
#   -print-decl-contexts    Print DeclContexts and their Decls
#   -print-ivar-layout      Enable Objective-C Ivar layout bitmap print trace
#   -print-preamble         Print the "preamble" of a file, which is a candidate for implicit precompiled headers.
#   -print-stats            Print performance metrics and statistics
#   -pthread                Support POSIX threads in generated code
#   -P                      Disable linemarker output in -E mode
#   -relaxed-aliasing       Turn off Type Based Alias Analysis
#   -relocatable-pch        Whether to build a relocatable precompiled header
#   -remap-file <from>;<to> Replace the contents of the <from> file with the contents of the <to> file
#   -resource-dir <value>   The directory which holds the compiler resource files
#   -rewrite-macros         Expand macros without full preprocessing
#   -rewrite-objc           Rewrite Objective-C source to C++
#   -rewrite-test           Rewriter playground
#   -Rpass-analysis=<value> Report transformation analysis from optimization passes whose name matches the given POSIX regular expression
#   -Rpass-missed=<value>   Report missed transformations by optimization passes whose name matches the given POSIX regular expression
#   -Rpass=<value>          Report transformations performed by optimization passes whose name matches the given POSIX regular expression
#   -R<remark>              Enable the specified remark
#   -serialize-diagnostic-file <filename>
#                           File for serializing diagnostics in a binary format
#   --show-includes         Print cl.exe style /showIncludes to stdout
#   -skip-unused-modulemap-deps
#                           Include module map files only for imported modules in dependency output
#   -split-dwarf-file <value>
#                           File name to use for split dwarf debug info output
#   -split-dwarf            Split out the dwarf .dwo sections
#   -split-stacks           Try to use a split stack if possible.
#   -stack-protector-buffer-size <value>
#                           Lower bound for a buffer to be considered for stack protection
#   -stack-protector <value>
#                           Enable stack protectors
#   -static-define          Should __STATIC__ be defined
#   -stats-file=<value>     Filename to write statistics to
#   -std=<value>            Language standard to compile for
#   -stdlib=<value>         C++ standard library to use
#   -sys-header-deps        Include system headers in dependency output
#   --system-header-prefix=<prefix>
#                           Treat all #include paths starting with <prefix> as including a system header.
#   -S                      Only run preprocess and compilation steps
#   -target-abi <value>     Target a particular ABI type
#   -target-cpu <value>     Target a specific cpu type
#   -target-feature <value> Target specific attributes
#   -target-linker-version <value>
#                           Target linker version
#   -target-variant-triple <value>
#                           Specify the target variant triple
#   -test-coverage          Do not generate coverage files or remove coverage changes from IR
#   -token-cache <path>     Use specified token cache file
#   -traditional-cpp        Enable some traditional CPP emulation
#   -trim-egraph            Only show error-related paths in the analysis graph
#   -triple <value>         Specify target triple (e.g. i686-apple-darwin9)
#   -undef                  undef all system defines
#   -unoptimized-cfg        Generate unoptimized CFGs for all analyses
#   -U <macro>              Undefine macro <macro>
#   -vectorize-loops        Run the Loop vectorization passes
#   -vectorize-slp          Run the SLP vectorization passes
#   -verify-ignore-unexpected=<value>
#                           Ignore unexpected diagnostic messages
#   -verify-ignore-unexpected
#                           Ignore unexpected diagnostic messages
#   -verify-pch             Load and verify that a pre-compiled header file is not stale
#   -verify=<prefixes>      Verify diagnostic output using comment directives that start with prefixes in the comma-separated sequence <prefixes>
#   -verify                 Equivalent to -verify=expected
#   --version               Print version information
#   -version                Print the compiler version
#   -vtordisp-mode=<value>  Control vtordisp placement on win32 targets
#   -v                      Show commands to run and use verbose output
#   -Wdeprecated            Enable warnings for deprecated constructs and define __DEPRECATED
#   -Wno-rewrite-macros     Silence ObjC rewriting warnings
#   -working-directory <value>
#                           Resolve file paths relative to the specified directory
#   -W<warning>             Enable the specified warning
#   -w                      Suppress all warnings
#   -x <language>           Treat subsequent input files as having type <language>
