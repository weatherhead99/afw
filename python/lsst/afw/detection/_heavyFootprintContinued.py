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
import numpy as np

from lsst.utils import TemplateMeta

from ._detection import HeavyFootprintI, HeavyFootprintF, HeavyFootprintD, HeavyFootprintU

__all__ = []  # import this module only for its side effects


class HeavyFootprint(metaclass=TemplateMeta):  # noqa: F811
    def addTo(self, image):
        """Add this heavy footprint to an image.

        Parameters
        ----------
        image : `lsst.afw.image`
        """
        indices = self.spans.indices()
        image.array[indices[0, :] - image.getY0(),
                    indices[1, :] - image.getX0()] += self.getImageArray()

    def subtractFrom(self, image):
        """Subtract this heavy footprint from an image.

        Parameters
        ----------
        image : `lsst.afw.image`
        """
        indices = self.spans.indices()
        image.array[indices[0, :] - image.getY0(),
                    indices[1, :] - image.getX0()] -= self.getImageArray()


HeavyFootprint.register(np.int32, HeavyFootprintI)
HeavyFootprint.register(np.float32, HeavyFootprintF)
HeavyFootprint.register(np.float64, HeavyFootprintD)
HeavyFootprint.register(np.uint16, HeavyFootprintU)
