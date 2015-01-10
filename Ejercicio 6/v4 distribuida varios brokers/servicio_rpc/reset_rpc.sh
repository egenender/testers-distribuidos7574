#correr con sudo
sudo -i service portmap stop
sudo -i rpcbind -i -w
sudo -i service portmap start

