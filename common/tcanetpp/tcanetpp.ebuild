# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

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
	emake || die "Make failed"
}

src_install() {
	insinto /usr/lib
	doins lib/libtcanetpp.so || die "File copy failed"
	insinto /usr/include/tcanetpp
	doins include/* || die "File copy failed"
}

