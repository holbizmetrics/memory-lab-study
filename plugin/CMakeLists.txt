cmake_minimum_required(VERSION 3.22)
project(MemoryLabPlugin VERSION 0.4.0 LANGUAGES C CXX)
set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../outputs/deps/JUCE" CACHE PATH "Pinned JUCE source checkout")
add_subdirectory("${JUCE_PATH}" "${CMAKE_BINARY_DIR}/juce")
juce_add_plugin(MemoryLab
 COMPANY_NAME "Memory Lab Research" BUNDLE_ID "org.memorylab.study"
 PLUGIN_MANUFACTURER_CODE MLab PLUGIN_CODE Mlm1
 FORMATS VST3 Standalone PRODUCT_NAME "Memory Lab Study"
 IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE NEEDS_MIDI_OUTPUT FALSE
 IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE
 COPY_PLUGIN_AFTER_BUILD FALSE VST3_CATEGORIES Instrument Synth)
target_sources(MemoryLab PRIVATE Processor.cpp Processor.hpp Editor.cpp Editor.hpp)
target_compile_features(MemoryLab PUBLIC cxx_std_17)
target_compile_definitions(MemoryLab PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(MemoryLab PRIVATE juce::juce_audio_utils PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
juce_add_console_app(MemoryLabSmoke PRODUCT_NAME "MemoryLabSmoke")
target_sources(MemoryLabSmoke PRIVATE smoke.cpp)
target_link_libraries(MemoryLabSmoke PRIVATE MemoryLab juce::juce_audio_utils)
enable_testing()
add_test(NAME wrapper_smoke COMMAND MemoryLabSmoke)
add_test(NAME automation_foundation COMMAND MemoryLabSmoke --automation-test)
if(WIN32)
 target_link_libraries(MemoryLabSmoke PRIVATE ole32 oleaut32)
 add_test(NAME uia_editor_lifecycle COMMAND MemoryLabSmoke --uia-lifecycle-test)
 set_tests_properties(uia_editor_lifecycle PROPERTIES TIMEOUT 180)
endif()
juce_add_console_app(MemoryLabValidate PRODUCT_NAME "MemoryLabValidate")
target_sources(MemoryLabValidate PRIVATE validate-vst3.cpp)
target_compile_features(MemoryLabValidate PRIVATE cxx_std_17)
target_compile_definitions(MemoryLabValidate PRIVATE JUCE_PLUGINHOST_VST3=1 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(MemoryLabValidate PRIVATE juce::juce_audio_processors_headless)
