# This file is part of afw.
#
# Developed for the LSST Data Management System.
# This product includes software developed by the LSST Project
# (https://www.lsst.org).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

__all__ = ["clipImage", "projectImage", "getProjectionIndices"]

import lsst.afw.detection as afwDetect
from .maskedImage import MaskedImage, makeMaskedImage
from .image import Mask


def clipImage(im, minClip, maxClip):
    """Clip an image to lie between minClip and maxclip (None to ignore)"""
    if isinstance(im, MaskedImage):
        mi = im
    else:
        mi = makeMaskedImage(im, Mask(im.getDimensions()))

    if minClip is not None:
        ds = afwDetect.FootprintSet(
            mi, afwDetect.Threshold(-minClip, afwDetect.Threshold.VALUE, False))
        afwDetect.setImageFromFootprintList(
            mi.getImage(), ds.getFootprints(), minClip)

    if maxClip is not None:
        ds = afwDetect.FootprintSet(mi, afwDetect.Threshold(maxClip))
        afwDetect.setImageFromFootprintList(
            mi.getImage(), ds.getFootprints(), maxClip)


def getProjectionIndices(imageBBox, targetBBox):
    """Get the indices to project an image

    Given an image and target bounding box,
    calculate the indices needed to appropriately
    slice the input image and target image to
    project the image to the target.

    Parameters
    ----------
    imageBBox: Box2I
        Bounding box of the input image
    targetBBox: Box2I
        Bounding box of the target image

    Returns
    -------
    targetSlices: tuple
        Slices of the target image in the form (by, bx), (iy, ix).
    imageIndices: tuple
        Slices of the input image in the form (by, bx), (iy, ix).
    """
    def getMin(dXmin):
        """Get minimum indices"""
        if dXmin < 0:
            bxStart = -dXmin
            ixStart = 0
        else:
            bxStart = 0
            ixStart = dXmin
        return bxStart, ixStart

    def getMax(dXmax):
        """Get maximum indices"""
        if dXmax < 0:
            bxEnd = None
            ixEnd = dXmax
        elif dXmax != 0:
            bxEnd = -dXmax
            ixEnd = None
        else:
            bxEnd = ixEnd = None
        return bxEnd, ixEnd

    dXmin = targetBBox.getMinX() - imageBBox.getMinX()
    dXmax = targetBBox.getMaxX() - imageBBox.getMaxX()
    dYmin = targetBBox.getMinY() - imageBBox.getMinY()
    dYmax = targetBBox.getMaxY() - imageBBox.getMaxY()

    bxStart, ixStart = getMin(dXmin)
    byStart, iyStart = getMin(dYmin)
    bxEnd, ixEnd = getMax(dXmax)
    byEnd, iyEnd = getMax(dYmax)

    bx = slice(bxStart, bxEnd)
    by = slice(byStart, byEnd)
    ix = slice(ixStart, ixEnd)
    iy = slice(iyStart, iyEnd)
    return (by, bx), (iy, ix)


def projectImage(image, bbox, fill=0):
    """Project an image into a bounding box

    Return a new image whose pixels are equal to those of
    `image` within `bbox`, and equal to `fill` outside.

    Parameters
    ----------
    image: `afw.Image` or `afw.MaskedImage`
        The image to project
    bbox: `Box2I`
        The bounding box to project onto.
    fill: number
        The value to fill the region of the new
        image outside the bounding box of the original.

    Returns
    -------
    newImage: `afw.Image` or `afw.MaskedImage`
        The new image with the input image projected
        into its bounding box.
    """
    if image.getBBox() == bbox:
        return image
    (by, bx), (iy, ix) = getProjectionIndices(image.getBBox(), bbox)

    if isinstance(image, MaskedImage):
        newImage = type(image.image)(bbox)
        newImage.array[by, bx] = image.image.array[iy, ix]
        newMask = type(image.mask)(bbox)
        newMask.array[by, bx] = image.mask.array[iy, ix]
        newVariance = type(image.image)(bbox)
        newVariance.array[by, bx] = image.variance.array[iy, ix]
        newImage = MaskedImage(image=newImage, mask=newMask, variance=newVariance, dtype=newImage.array.dtype)
    else:
        newImage = type(image)(bbox)
        if fill != 0:
            newImage.set(fill)
        newImage.array[by, bx] = image.array[iy, ix]
    return newImage
