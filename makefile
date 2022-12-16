CC = g++
CPPFLAGS = -g -O3 -fopenmp # -Wall

image : clean raytrace
	./raytrace 1

video: clean raytrace
	./raytrace 24 ; ffmpeg -y -r 24 -f image2 -i \
    images/image%04d.ppm test.mp4

clean:
	rm raytrace ; rm -f ./images/* ; rm -f *.o

raytrace: raytracing.cpp scene.o utils.o
	${CC} ${CPPFLAGS} $^ -o $@

utils.o: utils.cpp
	${CC} ${CPPFLAGS} $^ -c -o $@

scene.o: scene.cpp
	${CC} ${CPPFLAGS} $^ -c -o $@