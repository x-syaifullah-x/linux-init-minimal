tee /etc/systemd/system/home-overlay@.service << "EOF"
[Unit]
Description=Overlay /home using /dev/%i as backend
DefaultDependencies=no
After=local-fs.target
Before=getty@tty1.service

[Service]
Type=oneshot
ExecStart=/bin/sh -c '\
MD_DEV=%i; \
MD_DIR=/sys/class/block/$MD_DEV/md; \
modprobe md_mod legacy_async_del_gendisk=off; \
echo $MD_DEV > /sys/module/md_mod/parameters/new_array; \
echo raid0 > /$MD_DIR/level; \
echo 2 > /$MD_DIR/raid_disks; \
echo 65536 > /$MD_DIR/chunk_size; \
echo 1.2 > /$MD_DIR/metadata_version; \
echo  8:17 > $MD_DIR/new_dev; \
echo 0 > $MD_DIR/dev-sdb1/slot; \
echo  8:33 > $MD_DIR/new_dev; \
echo 1 > $MD_DIR/dev-sdc1/slot; \
echo active > $MD_DIR/array_state; \
LOWER_DIR=/run/o_lower; \
UPPER_DIR=/run/o_upper; \
WORK_DIR=/run/o_work; \
DATA_X=$LOWER_DIR/data; \
mkdir -p $DATA_X -m 0755; \
mkdir -p $UPPER_DIR/home -m 0755; \
mkdir -p $WORK_DIR/home -m 0755; \
mount /dev/$MD_DEV $DATA_X -o ro,noatime; \
mount -t overlay none /home -o noatime,lowerdir=$DATA_X/home,upperdir=$UPPER_DIR/home,workdir=$WORK_DIR/home; \
'
RemainAfterExit=yes

[Install]
WantedBy=basic.target
EOF