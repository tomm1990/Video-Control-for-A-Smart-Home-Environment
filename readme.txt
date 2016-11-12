Exercise 3:UNIX system programming
by Tom Goldberg (ID 302815279)


# unzip package into a folder and open terminal from within
# execute the following commands

1) sudo mknod /dev/cdev_example c 245 0	
2) change the file permmisions of step 1 to 777 (in File System/dev/cdev_example)
2) make
3) sudo insmod CharDev.ko
4) gcc ioctl_user.c -o a.out

* The executable file receives 2 arguments : 
	arg1 is the existing file that you wanna store (will be erased after execution)
	arg2 is the restored file

# to run the program :
./a.out hello.txt hello2.txt
