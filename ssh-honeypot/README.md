# SSH Honeypot

Fake sshd that logs ip addresses, usernames, and passwords.

Original repo: https://github.com/droberson/ssh-honeypot
Daniel Roberson Copyright (c) 2016-2022 Daniel Roberson

HASSH, PCAP and UDP sending are removed

## Quickstart

### Linux

Make sure headers/development packages are installed for:

- libssh
- openssl
- libjson-c

```
apt install libssh-dev libjson-c-dev libssl-dev
```

Build and Run

```
make
ssh-keygen -t rsa -f ./ssh-honeypot.rsa
bin/ssh-honeypot -r ./ssh-honeypot.rsa
```


### Docker (experimental)

Please take a look at our [Docker documentation](docker/README.md).

## Dropping Privileges

As of version 0.0.8, you can drop root privileges of this program
after binding to a privileged port. You can now run this as _nobody_
on port 22 for example instead of root, but have to initially start it
as root:

```
sudo bin/ssh-honeypot -p 22 -u nobody
```

Beware that this chowns the logfile to the user specified as well.


## Changing the Banner

ssh-honeypot allows you to change the server's banner to blend in with
other hosts on your network or mimic a specific device.

Set banner string

```
bin/ssh-honeypot -b "OpenSSH_X.Y"
```

## JSON Logging

The `-j` CLI flag specifies the path to log results in JSON
format. This feature can make log analytics much easier because many
languages have robust JSON support.

## Systemd Integration

On Linux you can install ssh-honeypot as a Systemd service so that it
automatically runs at system startup:

```
make install
systemctl enable --now ssh-honeypot
```

Before installing, check `ssh-honeypot.service` and modify it to run
with the options you want.

