project(waterrings)

add_library(${PROJECT_NAME} MODULE
	TIVBufferManager.cpp
	TIVBufferManager.h
	WaterRings.cpp
	WaterRings.h
	
	${STORM_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)