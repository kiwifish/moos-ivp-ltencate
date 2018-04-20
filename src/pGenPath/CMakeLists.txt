#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Lauren TenCate
#--------------------------------------------------------

SET(SRC
  GenPath.cpp
  GenPath_Info.cpp
  main.cpp
  Point.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   ${MOOS_LIBRARIES}
   apputil
   geometry
   mbutil
   m
   pthread
   )

