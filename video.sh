

make

./raytrace ${1}

ffmpeg -y -r 24 -f image2 -i images/image%04d.ppm -vcodec libx264 -crf 25 -pix_fmt yuv420p test.mp4