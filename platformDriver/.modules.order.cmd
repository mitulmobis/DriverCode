cmd_/home/lenovo/WorkSpace/Code/platformDriver/modules.order := {   echo /home/lenovo/WorkSpace/Code/platformDriver/pcdPlatformDriver.ko;   echo /home/lenovo/WorkSpace/Code/platformDriver/platformDevice.ko; :; } | awk '!x[$$0]++' - > /home/lenovo/WorkSpace/Code/platformDriver/modules.order
