#!/bin/sh

rm -rf temp-test
mkdir temp-test
#
# test img_to_rgb and back
#
./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/01_test.jpg.rgb"
./01_img_to_rgb/img_to_rgb --rgb="./temp-test/01_test.jpg.rgb" --img="./temp-test/01_test.jpg.rgb.jpg" --reverse=1
#
# test JPG --> RGB --> YUV --> RGB --> JPG
#
./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.yuv" -c YUV
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.yuv" -o "./temp-test/02_test.rgb.yuv.rgb" -c RGB
./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.yuv.rgb" --img="./temp-test/02_test.rgb.yuv.rgb.jpg" --reverse=1
#
# test JPG --> RGB --> XYZ --> RGB --> JPG
#
./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.xyz" -c XYZ
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.xyz" -o "./temp-test/02_test.rgb.xyz.rgb" -c RGB
./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.xyz.rgb" --img="./temp-test/02_test.rgb.xyz.rgb.jpg" --reverse=1
#
# test JPG --> RGB --> LAB (over XYZ) --> RGB (over XYZ)
#
./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.lab" -c LAB
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.lab" -o "./temp-test/02_test.rgb.lab.rgb" -c RGB
./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.lab.rgb" --img="./temp-test/02_test.rgb.lab.rgb.jpg" --reverse=1



