SmdHistGen takes an input prdf and reads the SMD data to determine the horizontal and vertical position of the SMD hits. Outputs a ROOT file with histograms of the hit centroid distributions.

To-do: add asymmetry calculation and plot L/R and U/D asymmetries

Notes on using 2 input files: it seems like the event combiner does not like getting input prdfs from different runs. This is a problem since currently there are no runs with both GL1 and SMD/ZDC packets saved. So for now (as of 3/30/24), I input only the SMD data, set the blue and yellow spin patterns to dummy values (+-+- for blue, ++-- for yellow), and set the bunch number to 0 for all events. However I did test that the spin patterns are read correctly when using the GL1 data only. Reading the bunch number does NOT work -- the packet is missing.
