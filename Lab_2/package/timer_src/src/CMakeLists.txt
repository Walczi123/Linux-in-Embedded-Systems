add_executable (timer timer.c)
install (TARGET timer DESTINATION bin)
find_library(LIBGPIOD libgpiod.so ../../../buildroot-2020.02/output/target/usr/lib)
target_link_libraries (timer ${LIBGPIOD})