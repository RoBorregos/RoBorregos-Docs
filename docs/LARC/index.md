# Latin American Robotics Competition Open Challenge

The competition context is aimed at automating an environment with a large number of packages
to be organized, Figure 1. The essence of the competition is extracted from environments such
as warehouse, product distribution center, store stock, etc.

Warehouses automation is already a reality in large companies like Amazon and Alibaba, but it
should be a reality in midsize companies soon. Think of a possible solution. Participants must
build an agile and fast robot to organize as many packages as possible in a limited time.

![image](https://github.com/RoBorregos/RoBorregos-Docs/assets/117100165/26c51e5c-5eba-4a80-ac3d-0598fa39a195)

## The goal

The robot can move freely in the scenario but cannot collide or push a package out of the
package's area. To reach the challenges of the competition, the robot must take each package
and leave it to its destination. The robot will not know his initial position in the scenario either the
position of the packs in the package's area. The objective is to take packages from a specific
location and take them to predefined locations, so that, at the end, the packages are in a desired
arrangement in the proposed scenario. The specific objectives are:
  1. Take colored packages (yellow, red, green, blue) and move them to the unloading regions
  with equivalent colors.
  2. Pick up packages containing 2D codes and move them to any respective position on the
  shelves.
  3. Pick up packages with alphabetical values and take them to any respective position on
  the shelves.

## Packages

Packages can be marked by color, 2D codes or alphabetical values. The possible colors for the
packages are green, yellow, blue and red. The 2D code is a bi-dimensional representation
containing 9 combinations, from one to 9, according to the markers that can be obtained from the
site https://chev.me/arucogen. The alphabetical packages are white and the 2D code packages
are black. There is a specific region in the scenario where the packages are initially positioned,
called the loading region.
