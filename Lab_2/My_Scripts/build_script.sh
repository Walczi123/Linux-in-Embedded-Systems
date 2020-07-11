(
rm ../overlay/usr/bin/script
export PATH=../buildroot-2020.02/output/host/usr/bin:$PATH
make ARCH=arm CROSS_COMPILE=\arm-none-linux-gnueabihf- script
cp script ../overlay/usr/bin/script
)
