# CMake generated Testfile for 
# Source directory: D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner
# Build directory: D:/projects/c++/miniShark/cmake-build-debug/extern/jsoncpp/src/jsontestrunner
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(jsoncpp_readerwriter "C:/msys64/mingw64/bin/python3.10.exe" "-B" "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/runjsontests.py" "D:/projects/c++/miniShark/cmake-build-debug/bin/jsontestrunner_exe.exe" "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter PROPERTIES  WORKING_DIRECTORY "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/CMakeLists.txt;43;add_test;D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/CMakeLists.txt;0;")
add_test(jsoncpp_readerwriter_json_checker "C:/msys64/mingw64/bin/python3.10.exe" "-B" "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/runjsontests.py" "--with-json-checker" "D:/projects/c++/miniShark/cmake-build-debug/bin/jsontestrunner_exe.exe" "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter_json_checker PROPERTIES  WORKING_DIRECTORY "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/CMakeLists.txt;47;add_test;D:/projects/c++/miniShark/extern/jsoncpp/src/jsontestrunner/CMakeLists.txt;0;")
