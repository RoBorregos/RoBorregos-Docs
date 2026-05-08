# Applying filters

Filters in preprocessing are the key to a successful identification. Without filters, the pixels color may vary 
significantly in a way that noise or even the smallest change in illumination can completely ruin an identification. 

## Frame journey
The frame journey looks like this, filters will be explained at a deeper level in the following sections

1. Frame is received
2. Color boosts are applied 
3. The frame color format is converted from BGR to Lab (see explanation in the following sections)
4. Lab channels are split
5. Contrast Limited AHE is applied to the L channel
6. Channels are merged
7. Bilateral filter is applied
8. The image is converted from Lab to HSV
9. The channels are split
10. In the saturation channel, a multiplicator is applied (called saturation)
11. A Gamma filter for Saturation and Value is applied
12. The image is merged
13. In some cases, a manual exposure boost may be needed (see following sections)
14. Image is converted from HSV to BGR

