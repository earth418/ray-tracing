NVCC = nvcc

NVCCFLAGS = -g -O3 -rdc=true # -fopenmp # -Wall

LFLAGS = -lm -Wno-deprecated-gpu-targets -g 

GENCODE_SM37 = -gencode=arch=compute_37,code=\"sm_37,compute_37\"
GENCODE = $(GENCODE_SM37)

.SUFFIXES : .cu .ptx

image : clean raytrace
	./raytrace 1

video: clean raytrace
	./raytrace 24 ; ffmpeg -y -r 24 -f image2 -i \
    images/image%04d.ppm test.mp4

clean:
	rm raytrace ; rm -f ./images/* ; rm -f *.o

raytrace: gpu_raytrace.cu scene.o utils.o
	${NVCC} $(GENCODE) ${NVCCFLAGS} $(LFLAGS) $^ -o $@

.cu.o:
	$(NVCC) $(GENCODE) $(NVCCFLAGS) -o $@ -c $<

# utils.o: utils.cu
# 	${CC} ${NVCCFLAGS} $^ -c -o $@

# scene.o: scene.cu
# 	${CC} ${NVCCFLAGS} $^ -c -o $@