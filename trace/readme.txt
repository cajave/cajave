gdbserver --attach ip:port `pidof program` #pidof program=>program's pid

gdb #start gdb
cd workingdir #step into working dir
file program #load program symbol file
target remote ip:port #connect to gdbserver

gdb --command=cmds
