#set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
load build/stm32f4_usb_cdc_ecp_openssl.elf
set logging off
quit





