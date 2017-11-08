file(GLOB SOURCES "src/*.cpp")

# Define an executable
add_executable(server
  ${SOURCES})

# Define the libraries this project depends upon
#target_link_libraries(app lib)
