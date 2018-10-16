Periodic noise removing filter
==========================

Goal
----

In this tutorial you will learn:

-   how to remove periodic noise in the Fourier domain

Theory
------

Periodic noise produces spikes in the Fourier domain that can often be detected by visual analysis.

### How to remove periodic noise in the Fourier domain?

Periodic noise can be reduced significantly via frequency domain filtering. On this page we use a notch reject filter with an appropriate radius to completely enclose the noise spikes in the Fourier domain. The notch filter rejects frequencies in predefined neighborhoods around a center frequency. The number of notch filters is arbitrary. The shape of the notch areas can also be arbitrary (e.g. rectangular or circular). On this page we use three circular shape notch reject filters. Power spectrum densify of an image is used for the noise spike’s visual detection.

Result
------

The figure below shows an image heavily corrupted by periodical noise of various frequencies.
![Image corrupted by periodic noise](/www/images/period_input.jpg)

The noise components are easily seen as bright dots (spikes) in the Power spectrum density shown in the figure below. 
![Power spectrum density showing periodic noise](/www/images/period_psd.jpg)

The figure below shows a notch reject filter with an appropriate radius to completely enclose the noise spikes. A radius = 21.
![Notch reject filter](/www/images/period_filter.jpg)

The result of processing the image with the notch reject filter is shown below.
![Result of filtering](/www/images/period_output.jpg)

The improvement is quite evident. This image contains significantly less visible periodic noise than the original image.

You can also find a quick video demonstration of this filtering idea on [YouTube](https://youtu.be/Qne51TcWwAc).