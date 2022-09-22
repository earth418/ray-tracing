CC = g++
CARGS = -g # -Wall

image : clean raytrace
	./raytrace 1

video: clean raytrace
	./raytrace 24 ; ffmpeg -y -r 24 -f image2 -i \
    images/image%04d.ppm -vcodec libx264 -crf 25  -pix_fmt yuv420p test.mp4

clean:
	rm raytrace ; rm -f ./images/* ;

raytrace: raytracing.cpp # utils.o
	${CC} ${CARGS} $^ -o $@

# raytracing.o: raytracing.cpp
# 	${CC} ${CARGS} $^ -c $@

# utils.o: utils.cpp
# 	${CC} ${CARGS} $^ -c $@

# scene.o: scene.cpp
# 	${CC} ${CARGS} $^ -c $@