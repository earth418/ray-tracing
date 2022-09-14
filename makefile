CC = g++
CARGS = -g -Wall

gen_video: clean raytrace
	./raytrace ; ffmpeg -y -r 24 -f image2 -i \
    images/image%04d.ppm -vcodec libx264 -crf 25  -pix_fmt yuv420p test.mp4

clean:
	rm -f ./images/*

raytrace: raytracing.cpp # utils.o
	${CC} ${CARGS} $^ -o $@

# raytracing.o: raytracing.cpp
# 	${CC} ${CARGS} $^ -c $@

# utils.o: utils.cpp