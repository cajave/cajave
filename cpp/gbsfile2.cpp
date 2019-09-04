
#!/bin/sh
if [ "$*" = "" ]; then
echo âusage: $0 [args]â
exit
fi
_CMDFILE=/tmp/`basename $0`.$$
_PROG=$1
shift
echo "Writing GDB commands file"
# Write the temporary GDB commands file
echo "set args $*" > $_CMDFILE
for i in $(nm -f posix $_PROG | awk '$2 == "T" {print $1}'); do
(echo break $i
echo command
echo silent
echo backtrace 1
echo continue
echo end
echo
) >> $_CMDFILE
done
echo run >> $_CMDFILE
echo q >> $_CMDFILE
# Now do the run
echo Starting GDB
gdb -quiet -command=$_CMDFILE $_PROG
# and clean up
echo GDB run finished
rm -f $_CMDFILE
