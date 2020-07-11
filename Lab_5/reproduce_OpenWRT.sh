wget http://10.0.2.2:8000/kmod-drv-mpc8xxx_4.14.171-1_aarch64_generic.ipk
opkg update
opkg install kmod-drv-mpc8xxx_4.14.171-1_aarch64_generic.ipk
modprobe gpio-mpc8xxx
opkg install libgpiod
opkg install gpiod-tools
gpiodetect
opkg install python
wget http://10.0.2.2:8000/python3-gpiod_1.3-1_aarch64_generic.ipk
opkg install python3-gpiod_1.3-1_aarch64_generic.ipk
opkg install alsa-utils
opkg install pciutils
opkg install kmod-sound-hda-intel
opkg install mpd-full
opkg install mpc
opkg install python3-pip
pip3 install python-mpd2
wget http://10.0.2.2:8000/mpd.conf
rm /etc/mpd.conf
mv mpd.conf /etc/mpd.conf
mkdir /root/music
/etc/init.d/mpd restart
opkg install python3-flask
opkg install screen
wget http://10.0.2.2:8000/webservice.py
wget http://10.0.2.2:8000/music_player.py
wget http://10.0.2.2:8000/run_after_start
cp ./run_after_start /etc/init.d/run_after_start
chmod 777 ./etc/init.d/run_after_start
/etc/init.d/run_after_start enable


