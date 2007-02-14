// -*- lsst-c++ -*-
#include "lsst/Mask.h"

using namespace lsst;

bool testCrFunc(unsigned char pixVal)
{
    return ((pixVal & 0x1)!=0);
}

int main(int argc, char *argv[])
{
// ------------- Test constructors

     typedef PixelGray<uint8> MaskPixelType;

     ImageView<MaskPixelType > maskImage(300,400);

     Mask<MaskPixelType > testMask(maskImage);

     typedef PixelGray<uint16> MaskPixelType2;

     ImageView<MaskPixelType2 > maskImage2(300,400);

     Mask<MaskPixelType2 > testMask2(maskImage2);

// ------------- Test mask plane addition

     int iPlane;

     iPlane = testMask.addMaskPlane("CR");
     cout << "Assigned CR to plane " << iPlane << endl;

     iPlane = testMask.addMaskPlane("BP");
     cout << "Assigned BP to plane " << iPlane << endl;

     int planeCR, planeBP;

     if (testMask.findMaskPlane("CR", planeCR) == false) {
	  cout << "No CR plane found" << endl;
     } else {
	  cout << "CR plane is " << planeCR << endl;
     }

     if (testMask.findMaskPlane("BP", planeBP) == false) {
	  cout << "No BP plane found" << endl;
     } else {
	  cout << "BP plane is " << planeBP << endl;
     }


// ------------ Test mask plane operations

     testMask.clearMaskPlane(planeCR);

     PixelCoord coord;
     list<PixelCoord> pixelList;

     for (int x=0; x<300; x+=1) {
	  for (int y=300; y<400; y+=20) {
	       coord.x = x;
	       coord.y = y;
	       pixelList.push_back(coord);
	  }
     }

     testMask.setMaskPlaneValues(planeCR, pixelList);

     testMask.setMaskPlaneValues(planeBP, pixelList);

     for (int x=250; x<300; x+=10) {
	  for (int y=300; y<400; y+=20) {
	       cout << x << " " << y << " " << (int)testMask(x, y) << " " << testMask(x, y, planeCR) << endl;
	  }
     }

     testMask.clearMaskPlane(planeCR);

     cout << endl;
     for (int x=250; x<300; x+=10) {
	  for (int y=300; y<400; y+=20) {
	       cout << x << " " << y << " " << (int)testMask(x, y) << " " << testMask(x, y, planeCR) << endl;
	  }
     }

// -------------- Test mask plane removal

     testMask.clearMaskPlane(planeBP);
     testMask.removeMaskPlane("BP");

     if (testMask.findMaskPlane("CR", planeCR) == false) {
	  cout << "No CR plane found" << endl;
     } else {
	  cout << "CR plane is " << planeCR << endl;
     }

     if (testMask.findMaskPlane("BP", planeBP) == false) {
	  cout << "No BP plane found" << endl;
     } else {
	  cout << "BP plane is " << planeBP << endl;
     }

// --------------- Test submask methods

     testMask.setMaskPlaneValues(planeCR, pixelList);

     Mask<MaskPixelType >* subTestMask;

     BBox2i region(100, 300, 10, 40);
     subTestMask = testMask.getSubMask(region);

     testMask.clearMaskPlane(planeCR);

     testMask.replaceSubMask(region, *subTestMask);

     cout << endl;
     for (int x=90; x<120; x+=1) {
	  for (int y=295; y<350; y+=5) {
	       cout << x << " " << y << " " << (int)testMask(x, y) << " " << testMask(x, y, planeCR) << endl;
	  }
     }

     int count = testMask.countMask(testCrFunc, region);
     cout << count << " pixels had CR set in region" << endl;

     // This should generate a vw exception - dimensions of region and submask must be =

     cout << "This should throw an exception:" << endl;

     region.expand(10);
     testMask.replaceSubMask(region, *subTestMask);


}
