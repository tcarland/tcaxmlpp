# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $
EAPI=4

inherit eutils

DESCRIPTION="The 'tcanetpp' C++ Network Library"
HOMEPAGE=""
SRC_URI="http://dev.ratnest.org/distfiles/${P}.tar.gz"

LICENSE="lgpl"
SLOT="0"
KEYWORDS="x86 amd64"
IUSE=""

export USE_PTHREADS=1
export USE_LIBRT=1

DEPEND=""
RDEPEND="${DEPEND}"

src_compile() {
	emake solib || die "Make failed"
}

src_install() {
	insinto /usr/lib
	dolib.so lib/libtcanetpp.so.${PVR} || die "File copy failed"
	dolib.so lib/libtcanetpp.so || die "File copy failed"
	insinto /usr/include/tcanetpp
	doins include/* || die "File copy failed"
}

