add_library(${GAME_NAME}_example SHARED
            Main.cpp
)

target_link_libraries(${GAME_NAME}_example PUBLIC lua ${GAME_NAME})