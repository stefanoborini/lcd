mod_lcd.o: mod_lcd.c
	gcc -O2 -DLINUX -D__KERNEL__ -DMODULE -DDEBUG -c mod_lcd.c
	rm -f /dev/lcd
	mknod /dev/lcd c 60 0
	@echo
	@echo "-------------------------------------------------------"
	@echo "Done. now do an insmod mod_lcd.o and play with /dev/lcd"
	@echo "using echo \"something\" >/dev/lcd"
	@echo "It will be displayed on the lcd device (i hope)"
	@echo "To remove the module do an rmmod mod_lcd."
	@echo "^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-"
	@echo "BE WARNED: umount all your filesystems (except /) before"
	@echo "starting play. A kernel panic (i'm not Alan Cox, but i'm"
	@echo "working on ;) ) reboot your machine without umount."
	@echo "So, if you want to see your data growing happy and peaceful"
	@echo "umount and sync. Believe me"
	@echo "however... my filesystem is sane, after a lot of crashes"
	@echo "(a LOT!). I simply used these simple rules."
	@echo "In every case, i'm not responsible of any damage."
	@echo "i've not forced you to play with my module. It was"
	@echo "your choice."
	@echo "^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-^-._.-"
	@echo "if you experience some problems/patches/suggestion"
	@echo "please email me"
	@echo "i'm just learning kernel stuff, and i'm just at the"
	@echo "beginning. I hope to learn something more than this."
	@echo "-------------------------------------------------------"
clean:
	rm -f *.o lcd

