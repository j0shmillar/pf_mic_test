idf_component_register(SRCS "${CMAKE_CURRENT_LIST_DIR}/src/Mainboard/Mainboard.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/Mainboard/BQ2562x.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/Mainboard/LC709204F.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/Utils/MasterI2C.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/Utils/ArduinoMasterI2C.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/Utils/Mutex.cpp"
                       INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src" 
                       PRIV_REQUIRES spi_flash nvs_flash
                       REQUIRES driver esp_timer)