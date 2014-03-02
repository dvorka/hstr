# Maintainer: Ricardo Band <me@xengi.de>
pkgname=hh
pkgver=1.2.1
pkgrel=1
pkgdesc="A command line utility that brings improved BASH command completion from the history. It aims to make completion easier and more efficient than Ctrl-r."
arch=('any')
url="https://github.com/dvorka/hstr"
license=('Apache')
makedepends=('readline' 'ncurses')
_pkgver=$(echo "$pkgver" | sed 's/.[0-9]*$//')
source=(https://github.com/dvorka/hstr/releases/download/$pkgver/$pkgname-$_pkgver-src.tgz)
md5sums=('a6b7ab128fff5604374166b488c7b8b7')

build() {
    cd "$srcdir/hstr"
    ./configure --prefix=/usr
    make
}

package() {
    cd "$srcdir/hstr"
    make DESTDIR="$pkgdir/" install
}
