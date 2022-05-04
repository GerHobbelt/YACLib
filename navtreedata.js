/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "YACLib", "index.html", [
    [ "Table of Contents", "index.html#autotoc_md1", null ],
    [ "About YACLib", "index.html#autotoc_md2", null ],
    [ "Getting started", "index.html#autotoc_md3", null ],
    [ "Examples", "index.html#autotoc_md4", null ],
    [ "Requirements", "index.html#autotoc_md15", null ],
    [ "Releases", "index.html#autotoc_md16", null ],
    [ "Contributing", "index.html#autotoc_md17", null ],
    [ "Contacts", "index.html#autotoc_md18", null ],
    [ "License", "index.html#autotoc_md19", null ],
    [ "Dependency", "md_doc_dependency.html", [
      [ "<a href=\"https://github.com/google/googletest\" >Google Test</a>", "md_doc_dependency.html#autotoc_md21", [
        [ "Version", "md_doc_dependency.html#autotoc_md22", null ],
        [ "Why do we need it?", "md_doc_dependency.html#autotoc_md23", null ]
      ] ],
      [ "<a href=\"https://github.com/google/benchmark\" >Google Benchmark</a>", "md_doc_dependency.html#autotoc_md24", [
        [ "Version", "md_doc_dependency.html#autotoc_md25", null ]
      ] ]
    ] ],
    [ "Design", "md_doc_design.html", [
      [ "Rationale", "md_doc_design.html#autotoc_md27", [
        [ "Zero cost abstraction", "md_doc_design.html#autotoc_md28", null ],
        [ "Easy to use", "md_doc_design.html#autotoc_md29", null ],
        [ "Easy to build", "md_doc_design.html#autotoc_md30", null ],
        [ "Good test coverage", "md_doc_design.html#autotoc_md31", null ]
      ] ],
      [ "Why should I use YACLib instead of ... ?", "md_doc_design.html#autotoc_md32", [
        [ "std", "md_doc_design.html#autotoc_md33", null ],
        [ "OpenMP", "md_doc_design.html#autotoc_md34", null ],
        [ "oneTBB", "md_doc_design.html#autotoc_md35", null ],
        [ "Boost.Asio", "md_doc_design.html#autotoc_md36", null ],
        [ "Folly", "md_doc_design.html#autotoc_md37", null ],
        [ "HPX", "md_doc_design.html#autotoc_md38", null ],
        [ "Boost.Fiber and Marl", "md_doc_design.html#autotoc_md39", null ],
        [ "Taskflow", "md_doc_design.html#autotoc_md40", null ],
        [ "libunifex and cppcoro", "md_doc_design.html#autotoc_md41", null ]
      ] ]
    ] ],
    [ "Дизайн", "md_doc_design_ru.html", [
      [ "Мотивация:", "md_doc_design_ru.html#autotoc_md53", [
        [ "Easy to use:", "md_doc_design_ru.html#autotoc_md54", null ],
        [ "Zero cost abstraction:", "md_doc_design_ru.html#autotoc_md55", null ],
        [ "Easy to build:", "md_doc_design_ru.html#autotoc_md56", null ],
        [ "Good test coverage:", "md_doc_design_ru.html#autotoc_md57", null ]
      ] ],
      [ "Что уже сделано:", "md_doc_design_ru.html#autotoc_md58", [
        [ "Были написаны следующие абстракции:", "md_doc_design_ru.html#autotoc_md59", null ],
        [ "Future/Promise/Run abstraction", "md_doc_design_ru.html#autotoc_md60", null ],
        [ "WhenAll Combinator", "md_doc_design_ru.html#autotoc_md61", null ],
        [ "ThreadFactory", "md_doc_design_ru.html#autotoc_md63", null ]
      ] ],
      [ "Что планируется сделать", "md_doc_design_ru.html#autotoc_md64", [
        [ "Lazy Future", "md_doc_design_ru.html#autotoc_md65", null ]
      ] ],
      [ "Разные реализации ThreadPool и их бенчмарки", "md_doc_design_ru.html#autotoc_md66", [
        [ "План реализации:", "md_doc_design_ru.html#autotoc_md67", null ]
      ] ],
      [ "Fibers", "md_doc_design_ru.html#autotoc_md68", [
        [ "План реализации:", "md_doc_design_ru.html#autotoc_md69", null ]
      ] ],
      [ "Concurrent algorithms", "md_doc_design_ru.html#autotoc_md70", null ],
      [ "Почему я должен использовать YACLib?", "md_doc_design_ru.html#autotoc_md71", [
        [ "STL", "md_doc_design_ru.html#autotoc_md73", null ],
        [ "OpenMP", "md_doc_design_ru.html#autotoc_md76", null ],
        [ "oneAPI TBB", "md_doc_design_ru.html#autotoc_md78", null ],
        [ "Boost.Asio", "md_doc_design_ru.html#autotoc_md79", null ],
        [ "Folly", "md_doc_design_ru.html#autotoc_md81", null ],
        [ "HPX", "md_doc_design_ru.html#autotoc_md83", null ],
        [ "Boost.Fiber и marl", "md_doc_design_ru.html#autotoc_md85", null ],
        [ "Taskflow", "md_doc_design_ru.html#autotoc_md87", null ],
        [ "libunifex/cppcoro", "md_doc_design_ru.html#autotoc_md88", null ]
      ] ]
    ] ],
    [ "Install", "md_doc_install.html", [
      [ "Prerequisites", "md_doc_install.html#autotoc_md44", [
        [ "Linux", "md_doc_install.html#autotoc_md45", null ],
        [ "macOS", "md_doc_install.html#autotoc_md46", null ],
        [ "Windows", "md_doc_install.html#autotoc_md47", null ]
      ] ],
      [ "As library", "md_doc_install.html#autotoc_md48", [
        [ "CMake with FetchContent", "md_doc_install.html#autotoc_md49", null ]
      ] ],
      [ "As project for develop", "md_doc_install.html#autotoc_md50", [
        [ "Console install and CMake/build options", "md_doc_install.html#autotoc_md51", [
          [ "Example", "md_doc_install.html#autotoc_md52", null ]
        ] ]
      ] ]
    ] ],
    [ "pr_guide", "md_doc_pr_guide.html", null ],
    [ "Sanitizers", "md_doc_sanitizer.html", [
      [ "Address Sanitizer", "md_doc_sanitizer.html#autotoc_md80", [
        [ "Tips", "md_doc_sanitizer.html#autotoc_md72", null ],
        [ "Useful links", "md_doc_sanitizer.html#autotoc_md74", null ],
        [ "Options", "md_doc_sanitizer.html#autotoc_md75", [
          [ "Useful options", "md_doc_sanitizer.html#autotoc_md77", null ]
        ] ],
        [ "Useful links", "md_doc_sanitizer.html#autotoc_md82", null ],
        [ "Overhead", "md_doc_sanitizer.html#autotoc_md84", null ],
        [ "ASAN_OPTIONS", "md_doc_sanitizer.html#autotoc_md86", [
          [ "Useful options", "md_doc_sanitizer.html#autotoc_md89", null ]
        ] ]
      ] ],
      [ "Thread Sanitizer", "md_doc_sanitizer.html#autotoc_md90", [
        [ "Useful links", "md_doc_sanitizer.html#autotoc_md91", null ],
        [ "Overhead", "md_doc_sanitizer.html#autotoc_md92", null ],
        [ "TSAN_OPTIONS", "md_doc_sanitizer.html#autotoc_md93", [
          [ "Useful options", "md_doc_sanitizer.html#autotoc_md94", null ]
        ] ],
        [ "DSAN_OPTIONS", "md_doc_sanitizer.html#autotoc_md95", null ]
      ] ],
      [ "Undefined Behavior Sanitizer", "md_doc_sanitizer.html#autotoc_md96", null ],
      [ "Memory Sanitizer", "md_doc_sanitizer.html#autotoc_md97", null ]
    ] ],
    [ "Style guide", "md_doc_style_guide.html", [
      [ "Formatting", "md_doc_style_guide.html#autotoc_md99", null ],
      [ "Code style", "md_doc_style_guide.html#autotoc_md100", null ]
    ] ],
    [ "Targets", "md_doc_target.html", [
      [ "yaclib", "md_doc_target.html#autotoc_md102", [
        [ "CLion", "md_doc_target.html#autotoc_md103", null ]
      ] ]
    ] ],
    [ "Todo List", "todo.html", null ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"classyaclib_1_1Promise.html#a537be055e8a85784941aec8169abc23d",
"classyaclib_1_1detail_1_1AtomicIntegralBase_3_01Impl_00_01T_00_01true_01_4.html#a6896ae9da12c9459a9d98c2216cef0e7",
"classyaclib_1_1detail_1_1MutexEvent.html#aa3a9c7e990858ca31264382f154d0976",
"functions_l.html",
"namespaceyaclib.html#a931c767f78cbcd6634cbedbddf405017a902b0d55fddef6f8d651fe1035b7d4bd",
"std_2detail_2atomic__flag_8hpp.html",
"wait__for_8hpp.html#a796e0128328cc5d7112b77af2f6b7c53"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';