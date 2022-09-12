CC = g++
CARGS = -g -Wall

gen_image: raytrace
	./raytrace > image.ppm

raytrace: raytracing.cpp # utils.o
	${CC} ${CARGS} $^ -o $@

# raytracing.o: raytracing.cpp
# 	${CC} ${CARGS} $^ -c $@

# utils.o: utils.cpp