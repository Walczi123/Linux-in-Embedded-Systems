git clone https://github.com/wzab/BR_Internet_Radio
cd BR_Internet_Radio
git checkout gpio_simple_USB
cd QemuVirt64
set -e
wget https://buildroot.org/downloads/buildroot-2020.02.tar.bz2
tar -xjf buildroot-2020.02.tar.bz2
cp BR_config buildroot-2020.02/.config
cd buildroot-2020.02
for i in ../patches/* ; do
   patch -p1 < $i
done
make host-qemu
cd ../../..
echo '#!/bin/bash
(
 OWRT='$PWD'
 ./buildroot-2020.02/output/host/bin/qemu-system-aarch64 -M virt -m 1024M -cpu cortex-a53 -nographic -smp 1 -kernel ${OWRT}/openwrt-19.07.2-armvirt-64-Image -append "rootwait root=/dev/vda console=ttyAMA0" -netdev user,id=eth0,hostfwd=tcp::8888-:8810,hostfwd=tcp::2222-:25 -device virtio-net-device,netdev=eth0 -drive file=${OWRT}/root.ext4,if=none,format=raw,id=hd0 -device virtio-blk-device,drive=hd0 -soundhw hda -audiodev id=pa,driver=pa
#  -device usb-ehci \
#  -device piix4-usb-uhci \
#  -usb -device usb-hub \
#  -device usb-host,vendorid=0x0d8c,productid=0x000c
)' > ./BR_Internet_Radio/QemuVirt64/run_script
chmod 777 ./BR_Internet_Radio/QemuVirt64/run_script
wget https://downloads.openwrt.org/releases/19.07.2/targets/armvirt/64/openwrt-19.07.2-armvirt-64-Image
wget https://downloads.openwrt.org/releases/19.07.2/targets/armvirt/64/openwrt-19.07.2-armvirt-64-root.ext4.gz
wget https://downloads.openwrt.org/releases/19.07.2/targets/armvirt/64/openwrt-sdk-19.07.2-armvirt-64_gcc-7.5.0_musl.Linux-x86_64.tar.xz
gzip -c -d openwrt-19.07.2-armvirt-64-root.ext4.gz > root.ext4
truncate -s \>512M root.ext4\n 
/sbin/resize2fs root.ext4
python3 -m http.server
