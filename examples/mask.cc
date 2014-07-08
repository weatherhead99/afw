/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
#include <cstdio>
#include <string>
#include <algorithm>

#include "lsst/utils/Utils.h"
#include "lsst/pex/exceptions.h"
#include "lsst/afw/image.h"

namespace afwGeom = lsst::afw::geom;
namespace afwImage = lsst::afw::image;

/************************************************************************************************************/

int main() {
    afwImage::Mask<afwImage::MaskPixel> img(afwGeom::Extent2I(10, 6));
    // This is equivalent to mask = 100:
    for (afwImage::Mask<afwImage::MaskPixel>::iterator ptr = img.begin(); ptr != img.end(); ++ptr) {
        (*ptr)[0] = 100;
    }
    // so is this, but fills backwards
    for (afwImage::Mask<afwImage::MaskPixel>::reverse_iterator ptr = img.rbegin(); ptr != img.rend(); ++ptr) {
        (*ptr)[0] = 100;
    }
    // so is this, but tests a different way of choosing begin()
    for (afwImage::Mask<afwImage::MaskPixel>::iterator ptr = img.at(0, 0); ptr != img.end(); ++ptr) {
        (*ptr)[0] = 100;
    }

    afwImage::Mask<afwImage::MaskPixel> jmg = img;

    printf("%dx%d\n", img.getWidth(), img.getHeight());

    *img.y_at(7, 2) = 999;
    *img.x_at(0, 0) = 0;
    img(img.getWidth() - 1, img.getHeight() - 1) = 100;

    printf("sub Mask<afwImage::MaskPixel>s\n");

    // img will be modified
    afwImage::Mask<afwImage::MaskPixel> simg1(
        img, 
        afwGeom::Box2I(afwGeom::Point2I(1, 1), afwGeom::Extent2I(7, 3)),
        afwImage::LOCAL
    );
    afwImage::Mask<afwImage::MaskPixel> simg(
        simg1, 
        afwGeom::Box2I(afwGeom::Point2I(0, 0), afwGeom::Extent2I(5, 2)),
        afwImage::LOCAL
    );

    {
        afwImage::Mask<afwImage::MaskPixel> nimg(simg.getDimensions());
        nimg = 1;
        simg <<= nimg;
    }

    for (int r = 0; r != img.getHeight(); ++r) {
        std::fill(img.row_begin(r), img.row_end(r), 100*(1 + r));
    }

    std::string maskedImagePath;
    try {
        std::string dataDir = lsst::utils::eups::productDir("afwdata");
        maskedImagePath = dataDir + "/data/small.fits";
    } catch (lsst::pex::exceptions::NotFoundError) {
        std::cerr << "Usage: mask [fitsFile]" << std::endl;
        std::cerr << "fitsFile is the path to a masked image" << std::endl;
        std::cerr << "\nError: setup afwdata or specify fitsFile.\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    afwImage::MaskedImage<float> mi = afwImage::MaskedImage<float>(maskedImagePath);
    printf("mask(0,0) = %d\n", (*(mi.getMask()))(0,0));
    printf("image(0,0) = %f\n", (*(mi.getImage()))(0,0));

    return 0;
}
