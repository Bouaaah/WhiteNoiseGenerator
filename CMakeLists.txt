cmake_minimum_required(VERSION 3.22)
project("WhiteNoise Generator" VERSION 0.0.1)

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12   
)
FetchContent_MakeAvailable(juce)

juce_add_gui_app(WNGenerator
            ICON_BIG assets/wave-sound.png)

juce_generate_juce_header(WNGenerator)

target_link_libraries(WNGenerator
    PRIVATE
        juce::juce_gui_basics
        juce::juce_audio_utils
        juce::juce_events)

include_directories(includes)

target_sources(WNGenerator
    PRIVATE
        src/App.cpp
        src/Controller.cpp
        src/Model.cpp
        src/View.cpp
)

target_compile_definitions(WNGenerator
    PUBLIC
        APP_VERSION="${PROJECT_VERSION}"
        APP_NAME="${PROJECT_NAME}")