UBUNTU INSTALLATION
-------------------
* install `hh` on Ubuntu (12.04LTS/12.10/13.10/14.04LTS):
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
```

* optionally configure `hh`: 
```bash
hh --show-configuration >> ~/.bashrc
```

INSTALLATION FROM SOURCE CODE
-----------------------------
* Clone Git repository:
```bash
git clone https://github.com/dvorka/hstr.git
```

* create build files using:
```bash
cd ./dist && ./1-dist.sh && cd ..
```

* install `hh` using:
```bash
./configure && make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```


DISTRIBUTION INSTALLATION 
-------------------------
* download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

* expand the archive and install `hh` using:
```bash
./configure && make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```


DEBIAN/MINT INSTALLATION
-------------------
* install `hh` on Debian (Wheezy) or Mint (13) by registering the key:
```bash
wget www.clfh.de/frankh.asc
apt-key add frankh.asc
```
adding the repository:
```bash
deb http://www.clfh.de/debian wheezy main
deb-src http://www.clfh.de/debian wheezy main
```
and installing it:
```bash
apt-get update
apt-get install hh
```

* optionally configure `hh`: 
```bash
hh --show-configuration >> ~/.bashrc
```


ARCH LINUX
----------
* download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

* use PKGBUILD in the root of the distribution to build package using `makepkg`:

https://wiki.archlinux.org/index.php/PKGBUILD

* install `hh`
* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```

MAC OS
------
* install MacPorts:

   http://www.macports.org

* set the environment:
```bash
env CFLAGS="-I/opt/local/include -I/usr/include" LDFLAGS="-L/opt/local/lib -L/usr/lib" ./configure
```

* finish the installation
```bash
make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```

CYGWIN/WINDOWS
--------------
* install from source
* configure `hh` (required for for terminal autocompletion, as TIOCSTI ioctl syscall is missing.):
```bash
hh --show-configuration >> ~/.bashrc
```
