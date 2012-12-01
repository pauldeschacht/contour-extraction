#!/bin/sh

rm -rf temp-test
mkdir temp-test

function t1() {
#
# test img_to_rgb and back
#
    ./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/01_test.jpg.rgb"
    ./01_img_to_rgb/img_to_rgb --rgb="./temp-test/01_test.jpg.rgb" --img="./temp-test/01_test.jpg.rgb.jpg" --reverse=1
}
function t2() {
#
# test JPG --> RGB --> YUV --> RGB --> JPG
#
    ./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.yuv" -c YUV
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.yuv" -o "./temp-test/02_test.rgb.yuv.rgb" -c RGB
    ./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.yuv.rgb" --img="./temp-test/02_test.rgb.yuv.rgb.jpg" --reverse=1
}

function t3() {
#
# test JPG --> RGB --> XYZ --> RGB --> JPG
#
    ./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.xyz" -c XYZ
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.xyz" -o "./temp-test/02_test.rgb.xyz.rgb" -c RGB
    ./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.xyz.rgb" --img="./temp-test/02_test.rgb.xyz.rgb.jpg" --reverse=1
}

function t4() {
#
# test JPG --> RGB --> LAB (over XYZ) --> RGB (over XYZ)
#
./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.lab" -c LAB
./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.lab" -o "./temp-test/02_test.rgb.lab.rgb" -c RGB
./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.lab.rgb" --img="./temp-test/02_test.rgb.lab.rgb.jpg" --reverse=1
}

function t5() {
#
# test JPG --> HSV --> --> RGB --> JPG
#
    ./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/02_test.rgb"
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb" -o "./temp-test/02_test.rgb.hsv" -c HSV
    ./02_convert_color_space/convert_color_space -i "./temp-test/02_test.rgb.hsv" -o "./temp-test/02_test.rgb.hsv.rgb" -c RGB
    ./01_img_to_rgb/img_to_rgb --rgb="./temp-test/02_test.rgb.hsv.rgb" --img="./temp-test/02_test.rgb.hsv.rgb.jpg" --reverse=1
}

function t6() {
#
# test YUV --> blur --> RGB --> JPG
# 
    ./01_img_to_rgb/img_to_rgb --img="../data/test_1.jpg" --rgb="./temp-test/03_test.rgb"
    ./02_convert_color_space/convert_color_space -i "./temp-test/03_test.rgb" -o "./temp-test/03_test.hsv" -c HSV
    ./03_smooth/smooth --infile="./temp-test/03_test.hsv" --outfile="./temp-test/03_test.hsv.blur" --threshold=0.15
    ./02_convert_color_space/convert_color_space --infile="./temp-test/03_test.hsv.blur" --outfile="./temp-test/03_test.hsv.blur.rgb" -c RGB
    ./01_img_to_rgb/img_to_rgb --rgb="./temp-test/03_test.hsv.blur.rgb" --img="./temp-test/03_test.blur.jpg" --reverse=1
}


t6
